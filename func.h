/*
 * func.h
 *
 *  Created on: 2021-10-13
 *      Author: Ann
 */

#ifndef FUNC_H_
#define FUNC_H_
#include"bmpInfo.h"
#include<stdio.h>
#include<stdlib.h>
#define _USE_MATH_DEFINES
#include<math.h>

BmpImage* read(const char* img_path);

void free_ptr(BmpImage* data_ptr);

BmpImage* copyBmpImagePtr(const BmpImage* data_ptr,const uint8_t copy_data);

void copyBmpImageInfo(const BmpImage* data_ptr, BmpImage* new_ptr);

void save(const char* save_path,BmpImage* data_ptr);

const void showInfo(const PBITMAPFILEHEADER file_header, const PBITMAPINFOHEADER info_header);

void biCubic(float_t *x);

BmpImage* interpolation(const BmpImage* data_ptr, const uint8_t type_interpolation, const float scale_factor);

BmpImage* histogramEqualization(const BmpImage* data_ptr);

void sort_array(uint8_t* data_array, const uint64_t length);

BmpImage* medianFilter(const BmpImage* data_ptr, const int32_t kernel_size);

BmpImage* GaussianLowPassFilter(const BmpImage* data_ptr, const int32_t kernel_size, const double_t sigma);

HSIInfo* bgr2hsi(const BmpImage* data_ptr);

void hsi2bgr(BmpImage* data_ptr, const HSIInfo* hsi_ptr);

void free_HSI(HSIInfo* data_ptr);

BmpImage* fft(const BmpImage* data_ptr);

void transpose_complex(Complex* data_ptr, const int32_t height, const int32_t width, const uint8_t conjugate_trans);

Complex* fftshift(const Complex* data_ptr, const uint8_t dimension,const int32_t* dimension_length);

Complex* fft2d(const Complex* data_ptr, const int32_t height, const int32_t width);

Complex* fft1d(const Complex* data_ptr, const int32_t ptr_length, const Complex* wds_ptr);

Complex* fft_windows(const int32_t ptr_power_idx);

BmpImage* dct(const BmpImage* data_ptr);

Complex* dct2d(const Complex* data_ptr, const int32_t height, const int32_t width, const uint8_t is_real);

Complex* dct1d(const Complex* data_ptr, const int32_t ptr_length, const uint8_t is_real,
	const Complex* wds_ptr, const Complex* fft_wds_ptr);

Complex* dct_windows(const int32_t ptr_power_idx, const uint8_t is_real);

BmpImage* dwt(const BmpImage* data_ptr);

Complex* dwt2D(const Complex* data_ptr, const int32_t in_height, const int32_t in_width);

Complex* convdown2d(const Complex* data_ptr, const Complex* conv_kernel, const uint8_t* kernel_size, const int32_t* length, const int32_t* first, const int32_t* last);

Complex* Conv(const Complex* data_ptr, const Complex* conv_kernel, const uint8_t* kernel_size, const int32_t* length, const uint8_t dimension, const uint8_t* padding, const uint8_t padding_mode);

void free_complex(Complex* x);

#endif /* FUNC_H_ */