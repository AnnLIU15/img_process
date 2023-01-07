import numpy as np
import sys
import cv2
from pathlib import Path
sys.path.append(Path(__file__).resolve().parent.parent)
from utils import getFiltersKernel, getDarkChannel
from algo import psf2otf, circularFilter
from algo.guidedFilter import guidedFilter

def calTransmission(normalized_img: np.ndarray,
                    est_tr_val: np.ndarray,
                    lambda_: float = 1,
                    sigma: float = 0.5,
                    beta: float = 1,            # cyclic looping for refining t
                    beta_rate: float = 2**1.5,
                    beta_max: float = 2**8,
                    kernel_type: int = 0  # algo kernel
                    ):  # results are found from 8
    filters_kernel = getFiltersKernel(kernel_type=kernel_type)
    filters_kernel_rot90_2 = np.zeros_like(filters_kernel)
    kernel_nums = filters_kernel.shape[0]
    for idx in range(kernel_nums):
        filters_kernel_rot90_2[idx, ...] = np.rot90(
            filters_kernel[idx, ...], 2)

    row, col, dim = normalized_img.shape  # opencv bgr
    d_rgb = np.zeros((row, col, dim))
    WFun = np.zeros((row, col, kernel_nums))
    for idx in range(kernel_nums):
        # kernel[3:6, 3:6] = filters_kernel[idx, ...]
        d_rgb[..., 0] = circularFilter(
            normalized_img[..., 0], filters_kernel[idx, ...])
        d_rgb[..., 1] = circularFilter(
            normalized_img[..., 1], filters_kernel[idx, ...])
        d_rgb[..., 2] = circularFilter(
            normalized_img[..., 2], filters_kernel[idx, ...])
        WFun[..., idx] = np.exp(-np.sum(d_rgb**2, axis=-1)/2*sigma)

    Tf = np.fft.fft2(est_tr_val)
    DS = np.zeros((row, col))
    D_k = np.zeros((row, col, kernel_nums), dtype=complex)

    for idx in range(kernel_nums):
        D_k[..., idx] = psf2otf(psf=filters_kernel[idx, ...],
                                shape=(row, col))
        DS += np.abs(D_k[..., idx])**2

    outer_idx = 0
    while beta < beta_max:
        gamma = lambda_ / beta
        outer_idx += 1
        # fixing t, solving u
        D_U = 0

        for idx in range(kernel_nums):
            dt_k = circularFilter(est_tr_val, filters_kernel[idx, ...])

            u_k = np.maximum(
                (abs(dt_k) - (WFun[..., idx] / (kernel_nums*beta))), 0) * np.sign(dt_k)

            filter_out = circularFilter(u_k, filters_kernel_rot90_2[idx, ...])
            D_U += np.fft.fft2(filter_out)

        # fixing u, solving t;
        est_tr_val = np.abs(np.fft.ifft2((gamma * Tf + D_U) / (gamma + DS)))
        # increasing beta
        beta = beta * beta_rate
    return est_tr_val


def calTransmissionHe(normalized_img: np.ndarray,
                      Ar: float,
                      windowSize: int = 15,
                      omega: float = 0.95  # aerial perspective adaptively keep more haze for the distant objects
                      ) -> np.ndarray:
    img_div_Ar = np.zeros_like(normalized_img)
    for idx in range(normalized_img.shape[-1]):
        img_div_Ar[..., idx] = normalized_img[..., idx]/Ar[idx]
    est_tr_val = 1 - omega * getDarkChannel(img_div_Ar, windowSize=windowSize)
    return est_tr_val


def calTransmissionCEP(normalized_img: np.ndarray,
                       Ar: float,
                       windowSize: int = 15,
                       # aerial perspective adaptively keep more haze for the distant objects
                       omega: float = 0.95,
                       is_fast: bool = 0) -> np.ndarray:
    img_div_Ar = np.zeros_like(normalized_img)
    for idx in range(normalized_img.shape[-1]):
        img_div_Ar[..., idx] = normalized_img[..., idx]/Ar[idx]

    if is_fast:
        min_I = np.min(img_div_Ar, axis=-1)
        min_e = getMine(min_I, windowSize=windowSize,eps=0.008, is_fast=is_fast)
    else:
        min_e = np.zeros_like(normalized_img)
        for idx in range(normalized_img.shape[-1]):
            min_e[..., idx] = getMine(
                img_div_Ar[..., idx], windowSize=windowSize,eps=0.008, is_fast=is_fast)
        min_e = np.min(min_e, axis=-1)
    est_tr_val = np.clip(1 - omega * min_e, 0.001, 1) 
    return est_tr_val


def getMine(normalized_img,
            windowSize: int = 15,
            eps: float = 0.008,
            is_fast: bool = 0):
    u = guidedFilter(normalized_img, normalized_img, windowSize, eps)
    sig = (normalized_img - u) * (normalized_img - u)
    if is_fast:
        ksize = (2*windowSize + 1, 2*windowSize + 1)
        sig = cv2.boxFilter(sig, -1, ksize = ksize)
    else:
        sig = guidedFilter(sig, normalized_img, windowSize, eps)
    return u - np.abs(np.sqrt(sig))
