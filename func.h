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

BmpImage* dct(const BmpImage* data_ptr);

BmpImage* dwt(const BmpImage* data_ptr);

void fft_real(double_t* Signal, double_t* Re_F, double_t* Im_F, int32_t N, int32_t t);

void fft_complex(double_t* Re_Signal, double_t* Im_Signal, double_t* Re_F, double_t* Im_F, int32_t N, int32_t t);

void free_complex(Complex* x);

#endif /* FUNC_H_ */