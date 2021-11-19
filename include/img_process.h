
#ifndef IMG_PROCESS_H_
#define IMG_PROCESS_H_
#include"bmp_func.h"
#define _USE_MATH_DEFINES
#include<math.h>
#include"complex_func.h"

void biCubic(float_t* x);

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

BmpImage* get_boundary(const BmpImage* data_ptr);

uint8_t* divide_uint8_data(const BmpImage* data_ptr);

uint8_t* corrosion(const uint8_t* data, const int32_t height, const int32_t width, const int32_t kernel);

uint8_t* get_boundary(const uint8_t* data, const uint8_t* corrosion_arr, const int32_t height, const int32_t width);

void back2BmpImage(BmpImage* data_ptr,const uint8_t* DATA);

#endif