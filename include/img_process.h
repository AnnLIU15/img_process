
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

BmpImage* dilate(const BmpImage* data_ptr, const int32_t kernel);

uint8_t* dilate_u8(const uint8_t* DATA, const int32_t src_height, const int32_t src_width, const uint8_t kernel);

uint8_t* image2arr(const BmpImage* data_ptr);

void back2BmpImage(BmpImage* data_ptr,const uint8_t* DATA);

BmpImage* local_variance_threshold(const BmpImage* data_ptr);

double_t cal_std(const uint8_t* arr, const int32_t* arr_p);

BmpImage* segmentation_from_roi(const BmpImage* data_ptr, const BmpImage* roi_ptr);

#endif