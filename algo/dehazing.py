import cv2
import numpy as np
import sys
from pathlib import Path
sys.path.append(Path(__file__).resolve().parent.parent)
from utils import getAr, getArHe, getArCEP
from algo import calTransmission, calTransmissionHe, guidedFilter, calTransmissionCEP


def dehazingRaikwar(hazy_img: np.ndarray,
                    zeta: float = 4) -> np.ndarray:
    r'''dehazing the hazy image

    Args:
        hazy_img (np.ndarray): haze img
        zeta (float): constant parameter of eq. 7
        Parameter ζ plays an important role in Eq. (7). 
        A high value of ζ will result in tr(x,y)=1 . 
        However, a low value of ζ will lower down the 
        transmission. Hence, ζ behaves like a controlling 
        parameter for the tightness of the bound. Therefore,
        ζ should be selected such that a tight bound on 
        transmission can be obtained. The value of ζ can 
        be used as a quality control parameter (to control
        contrast) for SID. 
        S. C. Raikwar and S. Tapaswi, "Lower Bound on Transmission Using Non-Linear 
        Bounding Function in Single Image Dehazing," in IEEE Transactions on Image Processing
        , vol. 29, pp. 4832-4847, 2020, doi: 10.1109/TIP.2020.2975909.
        
    Returns:
        np.ndarray: dehazd img
    '''
    try:
        row, col, dim = hazy_img.shape  # opencv bgr
    except ValueError as e:
        raise ValueError(f'{e}, you should input the rgb image!')
    Ar = getAr(hazy_img, normalized=True)
    # assume each channel is equal
    Ar_max = np.max(Ar)
    normalized_img = hazy_img/255.0  # I_h^c
    min_Ih_xy = np.min(normalized_img, axis=-1)
    # \min I_h(x,y) = \underset{c\in(r,g,b)}{\min}(I^c_h(x,y))
    # minimum color channels of hazy
    I_max = np.max(min_Ih_xy)
    # equation 7
    delta = zeta / (min_Ih_xy**0.5 + 1e-10)
    
    tr_low_bound = 1 / (1 + (I_max * (10**(-0.05 * delta)))/(
        Ar_max - min_Ih_xy + 1e-10))
    # eq. 14
    try:
        z = np.max(np.abs(tr_low_bound[Ar_max <= min_Ih_xy]))
    except ValueError:
        # z = 0   # none
        z = 1
    # z = 1 if z == 0 else z
    tr_hat = np.where(Ar_max > min_Ih_xy,
                      tr_low_bound,
                      np.abs(tr_low_bound)/z)

    kernel = np.ones((3, 3), np.float)
    # close opr
    est_tr_val = cv2.morphologyEx(src=tr_hat, op=cv2.MORPH_CLOSE,
                                  kernel=kernel, iterations=1)

    est_tr_val = calTransmission(normalized_img=normalized_img,
                                 est_tr_val=est_tr_val,
                                 lambda_=1,
                                 sigma=0.5)

    rt_val = deFog(normalized_img, est_tr_val, Ar, 0.85)
    rt_val = np.floor(rt_val * 255)
    return rt_val

def dehazingMeng(hazy_img: np.ndarray) -> np.ndarray:
    r'''dehazing the hazy image

    Args:
        hazy_img (np.ndarray): haze img
        G. Meng, Y. Wang, J. Duan, S. Xiang and C. Pan, "Efficient
        Image Dehazing with Boundary Constraint and Contextual Regularization,"
        2013 IEEE International Conference on Computer Vision, 2013, pp. 617-624.
        
    Returns:
        np.ndarray: dehazd img
    '''
    try:
        row, col, dim = hazy_img.shape  # opencv bgr
    except ValueError as e:
        raise ValueError(f'{e}, you should input the rgb image!')
    Ar = getAr(hazy_img, normalized=True, windowSize = 15)

    normalized_img = hazy_img/255.0  # I_h^c
    
    C0 = 20/255         # Default value = 20 (as recommended in the paper)
    C1 = 300/255        # Default value = 300 (as recommended in the paper)
    # BoundCon - Computing the Transmission using equation (7) in the paper
    
    t_b = np.maximum((Ar[0] - normalized_img[..., 0]) / (Ar[0] - C0),
                     (normalized_img[..., 0] - Ar[0]) / (C1 - Ar[0]))
    t_g = np.maximum((Ar[1] - normalized_img[..., 1]) / (Ar[1] - C0),
                     (normalized_img[..., 1] - Ar[1]) / (C1 - Ar[1]))
    t_r = np.maximum((Ar[2] - normalized_img[..., 2]) / (Ar[2] - C0),
                     (normalized_img[..., 2] - Ar[2]) / (C1 - Ar[2]))

    max_val = np.maximum(t_b, t_g, t_r)
    tr_hat = np.minimum(max_val, 1)
    # close opr
    kernel = np.ones((3, 3), np.float)
    est_tr_val = cv2.morphologyEx(src=tr_hat, op=cv2.MORPH_CLOSE,
                                  kernel=kernel, iterations=1)
    #####
    est_tr_val = calTransmission(normalized_img=normalized_img,
                                 est_tr_val=est_tr_val,
                                 lambda_=1,
                                 sigma=0.25,
                                 kernel_type = 1)

    rt_val = deFog(normalized_img, est_tr_val, Ar, 0.85)
    rt_val = np.floor(rt_val * 255)
    return rt_val


def dehazingHe(hazy_img: np.ndarray) -> np.ndarray:
    r'''dehazing the hazy image

    Args:
        hazy_img (np.ndarray): haze img
        K. He, J. Sun and X. Tang, "Single Image Haze Removal Using Dark
        Channel Prior," in IEEE Transactions on Pattern Analysis and
        Machine Intelligence, vol. 33, no. 12, pp. 2341-2353, 
        Dec. 2011, doi: 10.1109/TPAMI.2010.168.
        
    Returns:
        np.ndarray: dehazd img
    '''
    try:
        row, col, dim = hazy_img.shape  # opencv bgr
    except ValueError as e:
        raise ValueError(f'{e}, you should input the rgb image!')
    
    normalized_img = hazy_img/255.0  # I_h^c
    Ar = getArHe(normalized_img, windowSize = 15)
    gray_img = cv2.cvtColor(hazy_img, cv2.COLOR_BGR2GRAY) / 255.0
    
    tr_hat = calTransmissionHe(normalized_img,Ar=Ar,windowSize=15,omega=0.95)
    
    est_tr_val = guidedFilter(tr_hat, gray_img, 60, 0.0001)
    rt_val = deFog(normalized_img, est_tr_val, Ar, delta= 1, epsilon=0.1)
    rt_val = np.floor(rt_val * 255)
    return rt_val


def dehazingCEP(hazy_img: np.ndarray,
                is_fast: bool = False) -> np.ndarray:
    r'''dehazing the hazy image

    Args:
        hazy_img (np.ndarray): haze img
        T. M. Bui and W. Kim, "Single Image Dehazing Using Color 
        Ellipsoid Prior," in IEEE Transactions on Image Processing,
        vol. 27, no. 2, pp. 999-1009, Feb. 2018, doi: 10.1109/TIP.2017.2771158.

        
    Returns:
        np.ndarray: dehazd img
    '''
    try:
        row, col, dim = hazy_img.shape  # opencv bgr
    except ValueError as e:
        raise ValueError(f'{e}, you should input the rgb image!')
    hazy_img = hazy_img.astype('float')
    normalized_img = hazy_img/255.0  # I_h^c
    Ar = getArCEP(normalized_img, windowSize = 15,percent=1e-3)
    est_tr_val = calTransmissionCEP(normalized_img, Ar, windowSize=15,omega=0.95,is_fast = is_fast)
    rt_val = deFog(normalized_img, est_tr_val, Ar, delta= 1, epsilon=0)
    rt_val = np.floor(rt_val * 255)
    return rt_val

def deFog(normalized_img, t, Ar, delta, epsilon: float = 1e-5):
    t = np.maximum(abs(t), epsilon) ** delta
    if len(Ar) == 1:
        Ar = Ar * np.ones((3,))
    normalized_img[..., 0] = (normalized_img[..., 0] - Ar[0]) / t + Ar[0]
    normalized_img[..., 1] = (normalized_img[..., 1] - Ar[1]) / t + Ar[1]
    normalized_img[..., 2] = (normalized_img[..., 2] - Ar[2]) / t + Ar[2]
    normalized_img = np.clip(normalized_img, 0, 1)
    return normalized_img