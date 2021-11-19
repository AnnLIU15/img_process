#ifndef COMPLEX_FUNC_H_
#define COMPLEX_FUNC_H_
#include"img_process.h"
#include"data_type.h"

void transpose_complex(Complex* data_ptr, const int32_t height, const int32_t width, const uint8_t conjugate_trans);

Complex* fftshift(const Complex* data_ptr, const uint8_t dimension, const int32_t* dimension_length);

Complex* fft2d(const Complex* data_ptr, const int32_t height, const int32_t width);

Complex* fft1d(const Complex* data_ptr, const int32_t ptr_length, const Complex* wds_ptr);

Complex* fft_windows(const int32_t ptr_power_idx);

Complex* dct2d(const Complex* data_ptr, const int32_t height, const int32_t width, const uint8_t is_real);

Complex* dct1d(const Complex* data_ptr, const int32_t ptr_length, const uint8_t is_real,
	const Complex* wds_ptr, const Complex* fft_wds_ptr);

Complex* dct_windows(const int32_t ptr_power_idx, const uint8_t is_real);

Complex* dwt2D(const Complex* data_ptr, const int32_t in_height, const int32_t in_width);

Complex* convdown2d(const Complex* data_ptr, const Complex* conv_kernel, const uint8_t* kernel_size, const int32_t* length, const int32_t* first, const int32_t* last);

Complex* Conv(const Complex* data_ptr, const Complex* conv_kernel, const uint8_t* kernel_size, const int32_t* length, const uint8_t dimension, const uint8_t* padding, const uint8_t padding_mode);

void free_complex(Complex* x);

#endif
