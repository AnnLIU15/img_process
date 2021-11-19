#include"../include/complex_func.h"

/* slow transpose(may develop in the future if I remember it)
 * data_ptr(inout): matrix
 * height(in): origin height
 * width(in): dst height
 * conjugate_trans(in): is conjugate(1->yes,0->normal trans)
 */
void transpose_complex(Complex* data_ptr, const int32_t height, const int32_t width, const uint8_t conjugate_trans)
{
	Complex* out_ptr = (Complex*)malloc(sizeof(Complex));
	int32_t i, j;
	int32_t dst_width = height;
	out_ptr->real = (double_t*)malloc(sizeof(double_t) * height * width);
	out_ptr->imag = (double_t*)malloc(sizeof(double_t) * height * width);
	for (i = height - 1; i >= 0; i--)
	{
		for (j = width - 1; j >= 0; j--)
		{
			*(out_ptr->real + j * dst_width + i) = *(data_ptr->real + i * width + j);
			*(out_ptr->imag + j * dst_width + i) = *(data_ptr->imag + i * width + j);
		}
	}
	/* conjugate transpose */
	if (conjugate_trans == 1)
	{
		for (i = height - 1; i >= 0; i--)
			for (j = width - 1; j >= 0; j--)
			{
				//	out[j][i]=in[i][j]
				*(data_ptr->real + i * width + j) = *(out_ptr->real + i * width + j);
				*(data_ptr->imag + i * width + j) = -*(out_ptr->imag + i * width + j);
			}
	}
	else
	{
		for (i = height - 1; i >= 0; i--)
			for (j = width - 1; j >= 0; j--)
			{
				//	out[j][i]=in[i][j]
				*(data_ptr->real + i * width + j) = *(out_ptr->real + i * width + j);
				*(data_ptr->imag + i * width + j) = *(out_ptr->imag + i * width + j);
			}
	}
	free_complex(out_ptr);
}

/* using fft2d (padding in this way)
 * data_ptr(in): data
 * height,width(in): x, y dimension length
 */
Complex* fftshift(const Complex* data_ptr, const uint8_t dimension, const int32_t* dimension_length)
{
	Complex* out_ptr = NULL;
	Complex* tmp_ptr = NULL;
	Complex* tmp_ptr_1 = NULL;
	double_t real, imag;
	int32_t begin_idx, middle_idx;
	int32_t i, j;
	if (0 == dimension)
	{
		printf("Dimension is lower than 0, please check it! Return NULL pointer!\n");
	}
	else
	{
		/* *(dimension_length + 1) -> width *(dimension_length) -> height  */
		out_ptr = (Complex*)malloc(sizeof(Complex));
		if (1 == dimension)
		{
			out_ptr->real = (double_t*)malloc(sizeof(double_t) * *(dimension_length));
			out_ptr->imag = (double_t*)malloc(sizeof(double_t) * *(dimension_length));
			middle_idx = *(dimension_length) >> 1;
			if (*(dimension_length) % 2 == 0)
			{	/* swap(i,i+dimension_length/2)
				 *				0		<->	dimension_length/2
				 * \vdots
				 * dimension_length/2-1 <-> dimension_length - 1
				 */
				for (i = middle_idx - 1; i >= 0; i--)
				{
					*(out_ptr->real + i + middle_idx) = *(data_ptr->real + i);
					*(out_ptr->imag + i + middle_idx) = *(data_ptr->imag + i);
					*(out_ptr->real + i) = *(data_ptr->real + i + middle_idx);
					*(out_ptr->imag + i) = *(data_ptr->imag + i + middle_idx);
				}
			}
			else
			{
				/* swap(i,i+dimension_length/2)
				 *				0		-> dimension_length/2(5/2=2->3th place)
				 * dimension_length/2 + 1 \cdots dimension_length - 1
				 *						-> 0 \cdots dimension_length/2 -1
				 * 1 \cdots dimension_length/2
				 *						-> dimension_length/2 + 1 \cdots dimension_length - 1
				 */
				*(out_ptr->real + middle_idx) = *(data_ptr->real);
				*(out_ptr->imag + middle_idx) = *(data_ptr->imag);
				for (i = middle_idx; i >= 1; i--)
				{

					*(out_ptr->real + i + middle_idx) = *(data_ptr->real + i);
					*(out_ptr->imag + i + middle_idx) = *(data_ptr->imag + i);
					*(out_ptr->real + i - 1) = *(data_ptr->real + i + middle_idx);
					*(out_ptr->imag + i - 1) = *(data_ptr->imag + i + middle_idx);
				}
			}
		}
		else if (2 == dimension)
		{
			/* *(dimension_length) -> height
			 * *(dimension_length + 1) -> width
			 */
			real = *(dimension_length) * *(dimension_length + 1);
			out_ptr->real = (double_t*)malloc(sizeof(double_t) * real);
			out_ptr->imag = (double_t*)malloc(sizeof(double_t) * real);
			tmp_ptr = (Complex*)malloc(sizeof(Complex));
			tmp_ptr->real = (double_t*)malloc(sizeof(double_t) * *(dimension_length + 1));
			tmp_ptr->imag = (double_t*)malloc(sizeof(double_t) * *(dimension_length + 1));
			/* fftshift for row elements(like 1d fftshift) */
			for (i = *(dimension_length)-1; i >= 0; i--)
			{
				for (j = *(dimension_length + 1) - 1; j >= 0; j--)
				{
					*(tmp_ptr->real + j) = *(data_ptr->real + i * *(dimension_length + 1) + j);
					*(tmp_ptr->imag + j) = *(data_ptr->imag + i * *(dimension_length + 1) + j);
				}
				tmp_ptr_1 = fftshift(tmp_ptr, 1, (dimension_length + 1));
				for (j = *(dimension_length + 1) - 1; j >= 0; j--)
				{
					*(out_ptr->real + i * *(dimension_length + 1) + j) = *(tmp_ptr_1->real + j);
					*(out_ptr->imag + i * *(dimension_length + 1) + j) = *(tmp_ptr_1->imag + j);
				}
				free_complex(tmp_ptr_1);
			}
			free(tmp_ptr->real); free(tmp_ptr->imag);
			tmp_ptr->real = (double_t*)malloc(sizeof(double_t) * *(dimension_length));
			tmp_ptr->imag = (double_t*)malloc(sizeof(double_t) * *(dimension_length));
			/* fftshift for col(Treat each col(the vector) as an element and do the fftshift for these elements)
			 * I transpose the matrix and its flow will be same as the fftshift for row
			 */
			for (j = *(dimension_length + 1) - 1; j >= 0; j--)
			{
				for (i = *(dimension_length)-1; i >= 0; i--)
				{
					*(tmp_ptr->real + i) = *(out_ptr->real + i * *(dimension_length + 1) + j);
					*(tmp_ptr->imag + i) = *(out_ptr->imag + i * *(dimension_length + 1) + j);
				}
				tmp_ptr_1 = fftshift(tmp_ptr, 1, (dimension_length));
				for (i = *(dimension_length)-1; i >= 0; i--)
				{
					*(out_ptr->real + i * *(dimension_length + 1) + j) = *(tmp_ptr_1->real + i);
					*(out_ptr->imag + i * *(dimension_length + 1) + j) = *(tmp_ptr_1->imag + i);
				}
				free_complex(tmp_ptr_1);
			}
			free_complex(tmp_ptr);
		}
		else
		{
			printf("Dimension(%d) is bigger than 2 which is not supported now! Return NULL pointer!\n", dimension);
		}
	}
	return out_ptr;
}

/* using fft2d (padding in this function) only support 2^n length so padding it
 * data_ptr(in): data
 * height,width(in): x, y dimension length
 */
Complex* fft2d(const Complex* data_ptr, const int32_t height, const int32_t width)
{
	Complex* out_ptr = NULL;
	Complex* tmp_ptr = NULL;
	Complex* tmp_ptr_1 = NULL;
	Complex* wds_ptr = NULL;
	int32_t i, j, dst_height, dst_width, idx_pow;
	double_t length_d;
	if (height <= 0 && width <= 0)
	{
		printf("Dimension is lower than 2, please check it! Return NULL pointer!\n");
	}
	else
	{
		out_ptr = (Complex*)malloc(sizeof(Complex));
		tmp_ptr = (Complex*)malloc(sizeof(Complex));
		/* (height,width)->(2^(ceil(log2(height))),2^(ceil(log2(width)))) */
		length_d = height;
		idx_pow = 0;
		for (; length_d > 1; )
		{
			length_d *= 0.5;
			idx_pow++;
		}
		dst_height = pow(2, idx_pow);
		length_d = width;
		idx_pow = 0;
		for (; length_d > 1; )
		{
			length_d *= 0.5;
			idx_pow++;
		}
		dst_width = pow(2, idx_pow);
		idx_pow = dst_height * dst_width;
		out_ptr->real = (double_t*)malloc(sizeof(double_t) * idx_pow);
		out_ptr->imag = (double_t*)malloc(sizeof(double_t) * idx_pow);
		tmp_ptr->real = (double_t*)malloc(sizeof(double_t) * dst_width);
		tmp_ptr->imag = (double_t*)malloc(sizeof(double_t) * dst_width);
		/* padding */
		for (i = dst_height - 1; i >= 0; i--)
		{
			for (j = dst_width - 1; j >= 0; j--)
			{
				if (j > width - 1 || i > height - 1)
				{
					*(out_ptr->real + i * dst_width + j) = 0;
					*(out_ptr->imag + i * dst_width + j) = 0;
				}
				else
				{
					*(out_ptr->real + i * dst_width + j) = *(data_ptr->real + i * width + j);
					*(out_ptr->imag + i * dst_width + j) = *(data_ptr->imag + i * width + j);
				}
			}
		}
		/* row fft */
		wds_ptr = fft_windows(dst_width);
		/*printf("\ndst_width(%d):\n", dst_width);

		for (j = 0; j < dst_width>>1; j++)
		{
			printf("%f+1i*(%f) ", *(wds_ptr->real  + j), *(wds_ptr->imag  + j));
		}
		printf(";\n");*/

		for (i = dst_height - 1; i >= 0; i--)
		{
			for (j = dst_width - 1; j >= 0; j--)
			{
				*(tmp_ptr->real + j) = *(out_ptr->real + i * dst_width + j);
				*(tmp_ptr->imag + j) = *(out_ptr->imag + i * dst_width + j);

			}
			/*if (i == 0)
			{
				printf("data:\n");
				for (j = 0; j < dst_width - 1; j++)
				{
					printf("%f+1i*(%f) ", *(tmp_ptr->real + j), *(tmp_ptr->imag + j));
				}
				printf(";\n");
			}*/
			tmp_ptr_1 = fft1d(tmp_ptr, dst_width, wds_ptr);
			/*if (i == 0)
			{
			printf("fft:\n");
			for (j = 0; j < dst_width - 1; j++)
			{
				printf("%f+1i*(%f) ", *(tmp_ptr_1->real + j), *(tmp_ptr_1->imag + j));
			}
			printf(";\n");
			}*/
			for (j = dst_width - 1; j >= 0; j--)
			{
				*(out_ptr->real + i * dst_width + j) = *(tmp_ptr_1->real + j);
				*(out_ptr->imag + i * dst_width + j) = *(tmp_ptr_1->imag + j);
			}
			free_complex(tmp_ptr_1);
		}
		free_complex(wds_ptr);
		free(tmp_ptr->real); free(tmp_ptr->imag);
		tmp_ptr->real = (double_t*)malloc(sizeof(double_t) * dst_height);
		tmp_ptr->imag = (double_t*)malloc(sizeof(double_t) * dst_height);
		// transpose_complex(out_ptr, dst_height, dst_width, 0);
		/* col fft */
		wds_ptr = fft_windows(dst_height);
		for (j = dst_width - 1; j >= 0; j--)
		{
			for (i = dst_height - 1; i >= 0; i--)
			{
				*(tmp_ptr->real + i) = *(out_ptr->real + i * dst_width + j);
				*(tmp_ptr->imag + i) = *(out_ptr->imag + i * dst_width + j);
			}
			tmp_ptr_1 = fft1d(tmp_ptr, dst_height, wds_ptr);
			for (i = dst_height - 1; i >= 0; i--)
			{
				*(out_ptr->real + i * dst_width + j) = *(tmp_ptr_1->real + i);
				*(out_ptr->imag + i * dst_width + j) = *(tmp_ptr_1->imag + i);
			}

			free_complex(tmp_ptr_1);
		}
		// transpose_complex(out_ptr, dst_width, dst_height, 0);
		free_complex(tmp_ptr);
		free_complex(wds_ptr);

	}
	return out_ptr;
}

/* fft1d
 * data_ptr(in): data
 * ptr_length(in): data_init_length init_length -> true length (padding zeros)
 * wds_ptr(in): W_N^k
 */
Complex* fft1d(const Complex* data_ptr, const int32_t ptr_length, const Complex* wds_ptr)
{
	int32_t ptr_power_idx, ptr_power_half;
	double_t length_d = ptr_length;
	Complex* out_ptr = NULL;
	int32_t i, j, k, l, idx0, idx1, idx_pow;
	double_t treal, timag, ureal, uimag;
	if (ptr_length <= 0)
	{
		printf("data length(%d) is smaller than 1, please check it! Return NULL pointer!\n", ptr_length);
	}
	else
	{
		/* padding for data */
		idx_pow = 0;
		for (; length_d > 1; )
		{
			length_d *= 0.5;
			idx_pow++;
		}
		ptr_power_idx = pow(2, idx_pow);
		out_ptr = (Complex*)malloc(sizeof(Complex));
		out_ptr->real = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
		out_ptr->imag = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
		/* padding from 2^n fft */
		for (i = ptr_power_idx - 1; i >= 0; i--)
		{
			if ((ptr_length - 1) >= i)
			{
				*(out_ptr->real + i) = *(data_ptr->real + i);
				*(out_ptr->imag + i) = *(data_ptr->imag + i);
			}
			else
			{
				*(out_ptr->real + i) = 0;
				*(out_ptr->imag + i) = 0;
			}
		}
		// 位反转置换 Bit-reversal Permutation
		for (i = 0; i < ptr_power_idx; i++)
		{
			k = i; l = 0;
			for (j = 0; j < idx_pow; j++)
			{
				l = (l << 1) + (k & 1);    // l = l * 2 + k % 2;  
				k >>= 1;        // k = k / 2;  
			}
			if (l > i)
			{
				treal = *(out_ptr->real + i);
				timag = *(out_ptr->imag + i);
				*(out_ptr->real + i) = *(out_ptr->real + l);
				*(out_ptr->imag + i) = *(out_ptr->imag + l);
				*(out_ptr->real + l) = treal;
				*(out_ptr->imag + l) = timag;
			}
		}
		/* fft calculate */
		for (i = 2; i <= ptr_power_idx; i = (i << 1))
		{
			for (k = 0; k < ptr_power_idx; k += i)
			{
				for (j = 0; j < (i >> 1); j++)
				{
					idx0 = k + j;
					idx1 = idx0 + (i >> 1);
					l = ptr_power_idx * j / i;
					treal = *(wds_ptr->real + l) * *(out_ptr->real + idx1) - *(wds_ptr->imag + l) * *(out_ptr->imag + idx1);
					timag = *(wds_ptr->real + l) * *(out_ptr->imag + idx1) + *(wds_ptr->imag + l) * *(out_ptr->real + idx1);
					ureal = *(out_ptr->real + idx0);
					uimag = *(out_ptr->imag + idx0);
					*(out_ptr->real + idx0) = ureal + treal;
					*(out_ptr->imag + idx0) = uimag + timag;
					*(out_ptr->real + idx1) = ureal - treal;
					*(out_ptr->imag + idx1) = uimag - timag;
				}
			}
		}
	}
	return out_ptr;
}

/* fft_windows calculate the W_N^(n)
 * ptr_power_idx(in): length(log2(length)=Z)
 */
Complex* fft_windows(const int32_t ptr_power_idx)
{
	double_t treal, timag;
	int32_t i, ptr_power_half = ptr_power_idx >> 1;
	Complex* wds_ptr = (Complex*)malloc(sizeof(Complex));
	wds_ptr->real = (double_t*)malloc(sizeof(double_t) * ptr_power_half);
	wds_ptr->imag = (double_t*)malloc(sizeof(double_t) * ptr_power_half);
	*wds_ptr->real = 1;
	*wds_ptr->imag = 0;
	treal = 2 * M_PI / ptr_power_idx;
	timag = -sin(treal);
	treal = cos(treal);
	/* calculate the W_N^k */
	for (i = 1; i < ptr_power_half; i++)
	{
		*(wds_ptr->real + i) = *(wds_ptr->real + i - 1) * treal - *(wds_ptr->imag + i - 1) * timag;
		*(wds_ptr->imag + i) = *(wds_ptr->real + i - 1) * timag + *(wds_ptr->imag + i - 1) * treal;
	}
	return wds_ptr;
}

Complex* dct2d(const Complex* data_ptr, const int32_t height, const int32_t width, const uint8_t is_real)
{
	Complex* out_ptr = NULL;
	Complex* tmp_ptr = NULL;
	Complex* tmp_ptr_1 = NULL;
	Complex* wds_ptr = NULL;
	Complex* fft_wds_ptr = NULL;

	int32_t i, j, dst_height, dst_width, idx_pow, fft_length;
	double_t length_d;
	if (height <= 0 && width <= 0)
	{
		printf("Dimension is lower than 2, please check it! Return NULL pointer!\n");
	}
	else
	{
		out_ptr = (Complex*)malloc(sizeof(Complex));
		tmp_ptr = (Complex*)malloc(sizeof(Complex));
		/* (height,width)->(2^(ceil(log2(height))),2^(ceil(log2(width)))) */
		length_d = height;
		idx_pow = 0;
		for (; length_d > 1; )
		{
			length_d *= 0.5;
			idx_pow++;
		}
		dst_height = pow(2, idx_pow);
		length_d = width;
		idx_pow = 0;
		for (; length_d > 1; )
		{
			length_d *= 0.5;
			idx_pow++;
		}
		dst_width = pow(2, idx_pow);
		idx_pow = dst_height * dst_width;
		out_ptr->real = (double_t*)malloc(sizeof(double_t) * idx_pow);
		out_ptr->imag = (double_t*)malloc(sizeof(double_t) * idx_pow);
		tmp_ptr->real = (double_t*)malloc(sizeof(double_t) * dst_width);
		tmp_ptr->imag = (double_t*)malloc(sizeof(double_t) * dst_width);
		/* padding */
		for (i = dst_height - 1; i >= 0; i--)
		{
			for (j = dst_width - 1; j >= 0; j--)
			{
				if (j > width - 1 || i > height - 1)
				{
					*(out_ptr->real + i * dst_width + j) = 0;
					*(out_ptr->imag + i * dst_width + j) = 0;
				}
				else
				{
					*(out_ptr->real + i * dst_width + j) = *(data_ptr->real + i * width + j);
					*(out_ptr->imag + i * dst_width + j) = *(data_ptr->imag + i * width + j);
				}
			}
		}
		/* row dct */
		fft_length = is_real == 1 ? dst_width : dst_width * 2;
		fft_wds_ptr = fft_windows(fft_length);
		wds_ptr = dct_windows(dst_width, is_real);
		for (i = dst_height - 1; i >= 0; i--)
		{
			for (j = dst_width - 1; j >= 0; j--)
			{
				*(tmp_ptr->real + j) = *(out_ptr->real + i * dst_width + j);
				*(tmp_ptr->imag + j) = *(out_ptr->imag + i * dst_width + j);
			}

			tmp_ptr_1 = dct1d(tmp_ptr, dst_width, is_real, wds_ptr, fft_wds_ptr);

			for (j = dst_width - 1; j >= 0; j--)
			{
				*(out_ptr->real + i * dst_width + j) = *(tmp_ptr_1->real + j);
				*(out_ptr->imag + i * dst_width + j) = *(tmp_ptr_1->imag + j);
			}
			free_complex(tmp_ptr_1);
		}
		/*printf("\nAfter row dct:\n");
		for (i = 0; i < dst_height; i++)
		{
			for (j = 0; j < dst_width; j++)
			{
				printf("%f,%f\t", *(out_ptr->real + i * dst_width + j), *(out_ptr->imag + i * dst_width + j));
			}
			printf("\n");
		}*/
		free_complex(wds_ptr);
		free_complex(fft_wds_ptr);
		free(tmp_ptr->real); free(tmp_ptr->imag);
		tmp_ptr->real = (double_t*)malloc(sizeof(double_t) * dst_height);
		tmp_ptr->imag = (double_t*)malloc(sizeof(double_t) * dst_height);
		/* col dct */
		fft_length = is_real == 1 ? dst_height : dst_height * 2;
		fft_wds_ptr = fft_windows(fft_length);
		wds_ptr = dct_windows(dst_height, is_real);
		for (j = dst_width - 1; j >= 0; j--)
		{
			for (i = dst_height - 1; i >= 0; i--)
			{
				*(tmp_ptr->real + i) = *(out_ptr->real + i * dst_width + j);
				*(tmp_ptr->imag + i) = *(out_ptr->imag + i * dst_width + j);
			}
			/*printf("col %d:\n", j);
			for (i = 0; i < dst_height; i++)
				printf("%f,%f\t", *(tmp_ptr->real + i), *(tmp_ptr->imag + i));
			printf("\n");*/
			tmp_ptr_1 = dct1d(tmp_ptr, dst_height, is_real, wds_ptr, fft_wds_ptr);
			/*printf("Cal result:\n");
			for (i = 0; i < dst_height; i++)
				printf("%f,%f\t", *(tmp_ptr_1->real + i), *(tmp_ptr_1->imag + i));
			printf("\n");*/
			for (i = dst_height - 1; i >= 0; i--)
			{
				*(out_ptr->real + i * dst_width + j) = *(tmp_ptr_1->real + i);
				*(out_ptr->imag + i * dst_width + j) = *(tmp_ptr_1->imag + i);
			}

			free_complex(tmp_ptr_1);
		}
		free_complex(tmp_ptr);
		free_complex(wds_ptr);
		free_complex(fft_wds_ptr);
		/*printf("\nAfter col dct:\n");
		for (i = 0; i < dst_height; i++)
		{
			for (j = 0; j < dst_width; j++)
			{
				printf("%f,%f\t", *(out_ptr->real + i * dst_width + j), *(out_ptr->imag + i * dst_width + j));
			}
			printf("\n");
		}*/
	}
	return out_ptr;
}

Complex* dct1d(const Complex* data_ptr, const int32_t ptr_length, const uint8_t is_real,
	const Complex* wds_ptr, const Complex* fft_wds_ptr)
{
	Complex* out_ptr = NULL;
	Complex* tmp_ptr_1 = NULL;
	Complex* tmp_ptr_2 = NULL;
	double_t length_d = ptr_length;
	int32_t i, j, ptr_power_idx, idx_pow;
	if (ptr_length <= 0)
	{
		printf("data length(%d) is smaller than 1, please check it! Return NULL pointer!\n", ptr_length);
	}
	else
	{
		/* padding for data */
		idx_pow = 0;
		for (; length_d > 1; )
		{
			length_d *= 0.5;
			idx_pow++;
		}
		ptr_power_idx = pow(2, idx_pow);
		out_ptr = (Complex*)malloc(sizeof(Complex));
		tmp_ptr_1 = (Complex*)malloc(sizeof(Complex));
		out_ptr->real = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
		out_ptr->imag = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
		/* padding from 2^n fft */
		for (i = ptr_power_idx - 1; i >= 0; i--)
		{
			if ((ptr_length - 1) >= i)
			{
				*(out_ptr->real + i) = *(data_ptr->real + i);
				*(out_ptr->imag + i) = *(data_ptr->imag + i);
			}
			else
			{
				*(out_ptr->real + i) = 0;
				*(out_ptr->imag + i) = 0;
			}
		}
		if (!is_real  /* ||ptr_power_idx % 2 == 1 always false after padding*/)
		{
			tmp_ptr_1->real = (double_t*)malloc(sizeof(double_t) * (ptr_power_idx << 1));
			tmp_ptr_1->imag = (double_t*)malloc(sizeof(double_t) * (ptr_power_idx << 1));
			for (i = ptr_power_idx - 1; i >= 0; i--)
			{
				*(tmp_ptr_1->real + i) = *(tmp_ptr_1->real + 2 * ptr_power_idx - 1 - i) = *(out_ptr->real + i);
				*(tmp_ptr_1->imag + i) = *(tmp_ptr_1->imag + 2 * ptr_power_idx - 1 - i) = *(out_ptr->imag + i);
			}
			tmp_ptr_2 = fft1d(tmp_ptr_1, 2 * ptr_power_idx, fft_wds_ptr);

		}
		else
		{
			tmp_ptr_1->real = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
			tmp_ptr_1->imag = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
			for (i = ptr_power_idx - 1; i >= 0; i -= 2)
			{
				*(tmp_ptr_1->real + (int32_t)(ptr_power_idx - ((i + 1) >> 1))) = *(out_ptr->real + i);
				*(tmp_ptr_1->imag + (int32_t)(ptr_power_idx - ((i + 1) >> 1))) = *(out_ptr->imag + i);
				*(tmp_ptr_1->real + (int32_t)((i - 1) >> 1)) = *(out_ptr->real + i - 1);
				*(tmp_ptr_1->imag + (int32_t)((i - 1) >> 1)) = *(out_ptr->imag + i - 1);
			}
			tmp_ptr_2 = fft1d(tmp_ptr_1, ptr_power_idx, fft_wds_ptr);

		}
		for (i = ptr_power_idx - 1; i >= 0; i--)
		{
			*(out_ptr->real + i) = *(wds_ptr->real + i) * *(tmp_ptr_2->real + i) -
				*(wds_ptr->imag + i) * *(tmp_ptr_2->imag + i);
			*(out_ptr->imag + i) = is_real == 1 ? 0 : *(wds_ptr->real + i) * *(tmp_ptr_2->imag + i) +
				*(wds_ptr->imag + i) * *(tmp_ptr_2->real + i);
		}
		free_complex(tmp_ptr_1); free_complex(tmp_ptr_2);
	}
	return out_ptr;
}

/* dct kernel
 * ptr_power_idx(in):length
 * is_real(in): real signal?
 */
Complex* dct_windows(const int32_t ptr_power_idx, const uint8_t is_real)
{
	double_t treal, timag;
	int32_t i;
	double_t sqrt_2_inverse = 0.707106781186548;
	double_t mul_2n = 0.5 / (ptr_power_idx);
	double_t sqrt_2n_inverse = sqrt(mul_2n);
	double_t power_real = is_real == 1 ? 2 * sqrt_2n_inverse : sqrt_2n_inverse;
	/*
	 * complex
	 * ww = (exp(-1i*(0:n-1)*pi/(2*n))/sqrt(2*n)).';
	 * ww(1) = ww(1) / sqrt(2);
	 * real
	 *	ww = 2*exp(-1i*(0:n-1)'*pi/(2*n))/sqrt(2*n);
	 *	ww(1) = ww(1) / sqrt(2);
	 *  -> power_real*exp(-1i*(0:n-1)'*pi/(2*n))/sqrt(2*n)
	 *  -> power_real*sqrt_2n_inverse*exp(-1i*(0:n-1)'*pi/(2*n)) (0:n-1)'->k\in[0,n-1]
	 *  -> power_real*sqrt_2n_inverse* exp(-i*k*pi*mul_2n)
	 *  -> power_real*sqrt_2n_inverse*(cos(k*pi*mul_2n)-jsin(k*pi*mul_2n))
	 *  -> power_real**(cos(k*pi*mul_2n)-jsin(k*pi*mul_2n))
	 */
	Complex* wds_ptr = (Complex*)malloc(sizeof(Complex));
	wds_ptr->real = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
	wds_ptr->imag = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
	treal = M_PI * mul_2n;
	timag = -sin(treal);
	treal = cos(treal);
	/* calculate the W_N^k */
	*wds_ptr->real = power_real;
	*wds_ptr->imag = 0;
	for (i = 1; i < ptr_power_idx; i++)
	{
		*(wds_ptr->real + i) = *(wds_ptr->real + i - 1) * treal - *(wds_ptr->imag + i - 1) * timag;
		*(wds_ptr->imag + i) = *(wds_ptr->real + i - 1) * timag + *(wds_ptr->imag + i - 1) * treal;
	}
	*wds_ptr->real *= sqrt_2_inverse;
	*wds_ptr->imag *= sqrt_2_inverse;
	/*for (i = ptr_power_idx - 1; i >= 0; i--)
	{
		treal = cos(i * M_PI * mul_2n);
		timag = -sin(i * M_PI * mul_2n)
		*(wds_ptr->real+i)=*power_real;
	}*/

	return wds_ptr;
}

/* dwt2D is implemented according to matlab dwt2
 * data_ptr(in): data
 * in_height(in): img_height
 * in_width(in): img_width
 */
Complex* dwt2D(const Complex* data_ptr, const int32_t in_height, const int32_t in_width)
{
	int32_t i, j, dst_height, dst_width, half_height, half_width;
	double_t max_ca, min_ca, max_ca_i, min_ca_i;
	int32_t* length_ptr = NULL;
	int32_t* first = NULL; int32_t* last = NULL;
	Complex* kernel_ptr = NULL;
	Complex* conv_out = NULL;
	Complex* ca = NULL; Complex* ch = NULL; Complex* cv = NULL; Complex* cd = NULL;
	Complex* out_ptr = NULL;
	uint8_t* kernel_size = NULL;
	uint8_t* padding = NULL;
	// printf("\nheight=%d\twidth=%d\n", in_height, in_width);
	if (1 == in_height || 1 == in_width)
	{
		out_ptr = (Complex*)malloc(sizeof(Complex));
		out_ptr->real = (double_t*)malloc(sizeof(double_t) * in_height * in_width);
		out_ptr->imag = (double_t*)malloc(sizeof(double_t) * in_height * in_width);
		for (i = in_height * in_width - 1; i >= 0; i--)
		{
			*(out_ptr->real + i) = *(data_ptr->real + i);
			*(out_ptr->imag + i) = *(data_ptr->imag + i);
		}
	}
	else
	{
		length_ptr = (int32_t*)malloc(sizeof(int32_t) * 2);
		*(length_ptr) = in_height; *(length_ptr + 1) = in_width;
		kernel_size = (uint8_t*)malloc(sizeof(uint8_t) * 2);
		*(kernel_size) = 1; *(kernel_size + 1) = 2;
		kernel_ptr = (Complex*)malloc(sizeof(Complex));
		kernel_ptr->real = (double_t*)malloc(sizeof(double_t) * *(kernel_size) * *(kernel_size + 1));
		kernel_ptr->imag = (double_t*)malloc(sizeof(double_t) * *(kernel_size) * *(kernel_size + 1));
		*(kernel_ptr->real) = 0.707106781186548; *(kernel_ptr->imag) = 0;
		*(kernel_ptr->real + 1) = 0.707106781186548; *(kernel_ptr->imag + 1) = 0;
		padding = (uint8_t*)malloc(sizeof(uint8_t) * 2);
		*(padding) = 0; *(padding + 1) = 1;
		dst_height = in_height;
		dst_width = in_width + 1;
		conv_out = Conv(data_ptr, kernel_ptr, kernel_size, length_ptr, 2, padding, 1);
		/*printf("\n\nconv_out Data:\n");
		for (i = 0; i < dst_height; i++)
		{
			for (j = 0; j < dst_width; j++)
				printf("%f,%f\t", *(conv_out->real + i * dst_width + j), *(conv_out->imag + i * dst_width + j));
			printf("\n");
		}
		printf("\n\n\n");*/
		first = (int32_t*)malloc(sizeof(int32_t) * 2); last = (int32_t*)malloc(sizeof(int32_t) * 2);
		*(first) = 1; *(first + 1) = 1; *(last) = dst_height; *(last + 1) = dst_width;
		*(length_ptr) = dst_height; *(length_ptr + 1) = dst_width;
		*(kernel_size) = 2; *(kernel_size + 1) = 1;
		half_height = dst_height >> 1; half_width = dst_width >> 1;
		ca = convdown2d(conv_out, kernel_ptr, kernel_size, length_ptr, first, last);
		/*printf("%d,%d\n", dst_height >> 1, dst_width >> 1);
		printf("\n\nca Data:\n");
		for (i = 0; i < dst_height >> 1; i++)
		{
			for (j = 0; j < dst_width >> 1; j++)
				printf("%f,%f\t", *(ca->real + i * (dst_width >> 1) + j), *(ca->imag + i * (dst_width >> 1) + j));
			printf("\n");
		}*/
		*(kernel_ptr->real) = -0.707106781186548;
		ch = convdown2d(conv_out, kernel_ptr, kernel_size, length_ptr, first, last);
		/*printf("\n\nch Data:\n");
		for (i = 0; i < dst_height >> 1; i++)
		{
			for (j = 0; j < dst_width >> 1; j++)
				printf("%f,%f\t", *(ch->real + i * (dst_width >> 1) + j), *(ch->imag + i * (dst_width >> 1) + j));
			printf("\n");
		}*/
		*(kernel_size) = 1; *(kernel_size + 1) = 2;
		*(length_ptr) = in_height; *(length_ptr + 1) = in_width;
		free_complex(conv_out);
		conv_out = Conv(data_ptr, kernel_ptr, kernel_size, length_ptr, 2, padding, 1);
		/*printf("\n\nconv_out Data:\n");
		for (i = 0; i < dst_height; i++)
		{
			for (j = 0; j < dst_width; j++)
				printf("%f,%f\t", *(conv_out->real + i * dst_width + j), *(conv_out->imag + i * dst_width + j));
			printf("\n");
		}
		printf("\n\n\n");*/
		*(length_ptr) = dst_height; *(length_ptr + 1) = dst_width;
		*(kernel_size) = 2; *(kernel_size + 1) = 1;
		cd = convdown2d(conv_out, kernel_ptr, kernel_size, length_ptr, first, last);
		/*printf("\n\ncd Data:\n");
		for (i = 0; i < dst_height >> 1; i++)
		{
			for (j = 0; j < dst_width >> 1; j++)
				printf("%f,%f\t", *(cd->real + i * (dst_width >> 1) + j), *(cd->imag + i * (dst_width >> 1) + j));
			printf("\n");
		}*/
		*(kernel_ptr->real) = 0.707106781186548;
		cv = convdown2d(conv_out, kernel_ptr, kernel_size, length_ptr, first, last);
		/*printf("\n\ncd Data:\n");
		for (i = 0; i < dst_height >> 1; i++)
		{
			for (j = 0; j < dst_width >> 1; j++)
				printf("%f,%f\t", *(cv->real + i * (dst_width >> 1) + j), *(cv->imag + i * (dst_width >> 1) + j));
			printf("\n");
		}*/
		// Complex* ca = NULL; Complex* ch = NULL; Complex* cv = NULL; Complex* cd = NULL;
		max_ca = 0; min_ca = 65534; max_ca_i = 0; min_ca_i = 65534;
		for (i = half_height * half_width - 1; i >= 0; i--)
		{
			max_ca = max_ca < *(ca->real + i) ? *(ca->real + i) : max_ca;
			min_ca = min_ca > *(ca->real + i) ? *(ca->real + i) : min_ca;
			max_ca_i = max_ca_i < *(ca->imag + i) ? *(ca->imag + i) : max_ca_i;
			min_ca_i = min_ca_i > *(ca->imag + i) ? *(ca->imag + i) : min_ca_i;
		}
		max_ca = 255.0 / (max_ca - min_ca + 1e-15);
		max_ca_i = 255.0 / (max_ca_i - min_ca_i + 1e-15);
		for (i = half_height * half_width - 1; i >= 0; i--)
		{
			*(ca->real + i) = round((*(ca->real + i) - min_ca) * max_ca);
			*(ca->imag + i) = round((*(ca->imag + i) - min_ca_i) * max_ca_i);
		}
		free_complex(conv_out);
		conv_out = dwt2D(ca, half_height, half_width);

		out_ptr = (Complex*)malloc(sizeof(Complex));
		out_ptr->real = (double_t*)malloc(sizeof(double_t) * (half_height << 1) * (half_width << 1));
		out_ptr->imag = (double_t*)malloc(sizeof(double_t) * (half_height << 1) * (half_width << 1));

		for (i = half_height - 1; i >= 0; i--)
		{
			for (j = half_width - 1; j >= 0; j--)
			{
				/* ca区 */
				*(out_ptr->real + i * (half_width << 1) + j) = *(conv_out->real + i * half_width + j);
				*(out_ptr->imag + i * (half_width << 1) + j) = *(conv_out->imag + i * half_width + j);
				/* ch区 */
				*(out_ptr->real + i * (half_width << 1) + j + half_width) = *(ch->real + i * half_width + j);
				*(out_ptr->imag + i * (half_width << 1) + j + half_width) = *(ch->imag + i * half_width + j);
				/* cv区 */
				*(out_ptr->real + (i + half_height) * (half_width << 1) + j) = *(cv->real + i * half_width + j);
				*(out_ptr->imag + (i + half_height) * (half_width << 1) + j) = *(cv->imag + i * half_width + j);
				/* cd区 */
				*(out_ptr->real + (i + half_height) * (half_width << 1) + j + half_width) = *(cd->real + i * half_width + j);
				*(out_ptr->imag + (i + half_height) * (half_width << 1) + j + half_width) = *(cd->imag + i * half_width + j);
			}
		}
		/*printf("\n\nout Data:\n");
		for (i = 0; i < (half_height<<1); i++)
		{
			for (j = 0; j < (half_width << 1); j++)
				printf("%f,%f\t", *(out_ptr->real + i * (half_width << 1) + j), *(out_ptr->imag + i * (half_width << 1) + j));
			printf("\n");
		}*/
		free(length_ptr); free(kernel_size); free(first); free(last); free(padding);
		free_complex(kernel_ptr);
		free_complex(conv_out);
		free_complex(ca); free_complex(ch); free_complex(cv); free_complex(cd);
	}
	return out_ptr;
}

/* matlab dwt2 Internal Function -> convdown implement
 * data_ptr(in): data
 * conv_kernel(in): convolution kernel
 * kernel_size(in): convolution kernel_size
 * length(in): data size
 * first(in): beign in
 * last(in): end with
 */
Complex* convdown2d(const Complex* data_ptr, const Complex* conv_kernel, const uint8_t* kernel_size
	, const int32_t* length, const int32_t* first, const int32_t* last)
{
	Complex* out_ptr = NULL;
	Complex* submat_ptr = NULL;
	Complex* conv_out = NULL;
	uint8_t is_satisfied = 1;
	int32_t i, j, dst_height, dst_width, pixel_number;
	int32_t* dst_length = NULL;
	int32_t* padding = NULL;
	for (i = 1; i >= 0; i--)
	{
		if (*(first + i) < 0 || *(kernel_size + i) < 1 ||
			*(last + i) < 2 || *(length + i) < 1 ||
			*(last + i) > *(length + i) || *(first + i) > *(last + i))
		{
			is_satisfied = 0;
		}
		else
		{
			;
		}
	}
	if (0 == is_satisfied)
	{
		printf("\nSome information is wrong! It will be show below. All of them should greater or equal to 1.\n");
		printf("And Length>=Last>=First.\n");
		printf("Total Dimension = %d\n", 2);
		for (i = 1; i >= 0; i--)
		{
			printf("Dimension %d: kernel_size(%d) length(%d) first(%d) last(%d)\n",
				i + 1, *(kernel_size + i), *(length + i), *(first + i), *(last + i));
		}
		printf("Return NULL pointer.\n");
	}
	else
	{
		dst_height = *(length);

		pixel_number = (*(last + 1) - *(first + 1)) % 2;
		dst_width = (*(last + 1) - *(first + 1) + 1) >> 1;
		/*printf("From (%d,%d) to (%d,%d)\n", *(length), *(length + 1), dst_height, dst_width);
		printf("First (%d,%d) Last (%d,%d)\n", *(first), *(first + 1), *(last), *(last + 1));*/
		pixel_number = dst_height * dst_width;
		submat_ptr = (Complex*)malloc(sizeof(Complex));
		submat_ptr->real = (double_t*)malloc(sizeof(double_t) * pixel_number);
		submat_ptr->imag = (double_t*)malloc(sizeof(double_t) * pixel_number);
		for (i = dst_height - 1; i >= 0; i--)
		{
			for (j = dst_width - 1; j >= 0; j--)
			{
				*(submat_ptr->real + i * dst_width + j) = *(data_ptr->real + i * *(length + 1) + (j << 1) + *(first + 1));
				*(submat_ptr->imag + i * dst_width + j) = *(data_ptr->imag + i * *(length + 1) + (j << 1) + *(first + 1));
			}
		}
		/*printf("\n\n\bsub Data:\n");

		for (i = 0; i < dst_height; i++)
		{
			for (j = 0; j < dst_width; j++)
			{
				printf("%f,%f\t", *(submat_ptr->real + i * dst_width + j), *(submat_ptr->imag + i * dst_width + j));
			}
			printf("\n");
		}*/
		dst_length = (int32_t*)malloc(sizeof(int32_t) * 2);
		*(dst_length) = dst_height; *(dst_length + 1) = dst_width;
		padding = (int32_t*)malloc(sizeof(int32_t) * 2);
		*(padding) = 1; *(padding + 1) = 0; /* row padding */
		conv_out = Conv(submat_ptr, conv_kernel, kernel_size, dst_length, 2, padding, 1);
		free_complex(submat_ptr);
		*(dst_length) += (*(padding) << 1) - *(kernel_size)+1;
		*(dst_length + 1) += (*(padding + 1) << 1) - *(kernel_size + 1) + 1;
		/*printf("\n\nconv_out Data:\n");
		for (i = 0; i < *(dst_length); i++)
		{
			for (j = 0; j < *(dst_length + 1); j++)
				printf("%f,%f\t", *(conv_out->real + i * *(dst_length + 1) + j), *(conv_out->imag + i * *(dst_length + 1) + j));
			printf("\n");
		}
		printf("\n\n\n");*/
		dst_height = (*(last)-*(first)+1) >> 1;
		dst_width = *(dst_length + 1);
		out_ptr = (Complex*)malloc(sizeof(Complex));
		pixel_number = dst_height * dst_width;
		out_ptr->real = (double_t*)malloc(sizeof(double_t) * pixel_number);
		out_ptr->imag = (double_t*)malloc(sizeof(double_t) * pixel_number);
		//printf("Length From (%d,%d) to (%d,%d)\n", *(dst_length), *(dst_length + 1), dst_height, dst_width);
		for (i = dst_height - 1; i >= 0; i--)
		{
			for (j = dst_width - 1; j >= 0; j--)
			{
				pixel_number = (i << 1) + *(first);
				//printf("Data From (%d,%d) to (%d,%d)\n", pixel_number, j, i, j);
				*(out_ptr->real + i * dst_width + j) = *(conv_out->real + pixel_number * dst_width + j);
				*(out_ptr->imag + i * dst_width + j) = *(conv_out->imag + pixel_number * dst_width + j);
			}
		}
		/*printf("\n\n\out Data:\n");
		for (i = 0; i < dst_height; i++)
		{
			for (j = 0; j < dst_width; j++)
				printf("%f,%f\t", *(out_ptr->real + i * dst_width + j), *(out_ptr->imag + i * dst_width + j));
			printf("\n");
		}
		printf("\n\n\n");*/
		free_complex(conv_out);
		free(dst_length); free(padding);
	}
	return out_ptr;
}

/* convolution when height or width==1 is 1D conv   stride = 1
 * data_ptr(in): data
 * conv_kernel(in): conv kernel
 * kernel_size(in): kernel_size
 * length(in): data length
 * dimension: 1d 2d 3d
 * padding_mode: 0 -> padding zero 1 -> padding the nearest
 */
Complex* Conv(const Complex* data_ptr, const Complex* conv_kernel, const uint8_t* kernel_size,
	const int32_t* length, const uint8_t dimension, const uint8_t* padding, const uint8_t padding_mode)
{
	Complex* padding_ptr = NULL;
	Complex* out_ptr = NULL;
	int32_t i, j, k, l;
	double_t real, imag, tmp_real, tmp_imag;
	int32_t dst_height, dst_width, padding_height, padding_width;
	int32_t* idx_ptr = NULL;		// for geq 3D in the future
	int32_t* padding_length_ptr = NULL; // for geq 3D in the future
	int32_t* dst_length_ptr = NULL; // for geq 3D in the future
	double_t* real_ptr = NULL;		// for geq 3D in the future
	double_t* image_ptr = NULL;		// for geq 3D in the future
	int64_t pixel_num;
	uint8_t is_satisfied = 1;
	if (dimension == 0)
	{
		is_satisfied = 0;
	}
	else
	{
		for (i = dimension - 1; i >= 0; i--)
		{
			if (*(length + i) < 1 || *(kernel_size + i) < 1)
			{
				is_satisfied = 0;
			}
			else
			{
				;
			}
		}
	}
	if (0 == is_satisfied)
	{
		printf("\nSome information is wrong! It will be show below. All of them should greater or equal to 1.\n");
		printf("Total Dimension = %d\n", dimension);
		for (i = dimension - 1; i >= 0; i--)
		{
			printf("Dimension %d: data_length(%d) kernel_size(%d)\n", i + 1, *(length + i), *(kernel_size + i));
		}
		printf("Return NULL pointer.\n");
	}
	else
	{
		if (dimension <= 2)
		{
			pixel_num = (int64_t) * (padding) << 1;
			dst_height = pixel_num + *(length);
			pixel_num = (int64_t) * (padding + 1) << 1;
			dst_width = pixel_num + *(length + 1);
			pixel_num = (int64_t)dst_height * dst_width;
			// printf("\nPadding from (%d,%d)->(%d,%d)\n", *(length), *(length + 1), dst_height, dst_width);
			padding_ptr = (Complex*)malloc(sizeof(Complex));
			padding_ptr->real = (double_t*)malloc(sizeof(double_t) * pixel_num);
			padding_ptr->imag = (double_t*)malloc(sizeof(double_t) * pixel_num);
			/* padding */
			for (i = dst_height - *padding - 1; i >= *padding; i--)
			{
				for (j = dst_width - *(padding + 1) - 1; j >= *(padding + 1); j--)
				{
					*(padding_ptr->real + i * dst_width + j) = *(data_ptr->real + (i - *(padding)) * *(length + 1) + j - *(padding + 1));
					*(padding_ptr->imag + i * dst_width + j) = *(data_ptr->imag + (i - *(padding)) * *(length + 1) + j - *(padding + 1));
				}
			}
			if (padding_mode == 1)
			{
				/* for each row */
				for (i = *padding - 1; i >= 0; i--)
				{
					for (j = dst_width - 1; j >= 0; j--)
					{
						*(padding_ptr->real + i * dst_width + j) = *(padding_ptr->real + (i + 1) * dst_width + j);
						*(padding_ptr->imag + i * dst_width + j) = *(padding_ptr->imag + (i + 1) * dst_width + j);
						*(padding_ptr->real + (dst_height - 1 - i) * dst_width + j) = *(padding_ptr->real + (dst_height - 2 - i) * dst_width + j);
						*(padding_ptr->imag + (dst_height - 1 - i) * dst_width + j) = *(padding_ptr->imag + (dst_height - 2 - i) * dst_width + j);
					}
				}
				/* for each col */
				for (j = *(padding + 1) - 1; j >= 0; j--)
				{
					for (i = dst_height - 1; i >= 0; i--)
					{
						*(padding_ptr->real + i * dst_width + j) = *(padding_ptr->real + i * dst_width + j + 1);
						*(padding_ptr->imag + i * dst_width + j) = *(padding_ptr->imag + i * dst_width + j + 1);
						*(padding_ptr->real + i * dst_width + dst_width - 1 - j) = *(padding_ptr->real + i * dst_width + dst_width - 2 - j);
						*(padding_ptr->imag + i * dst_width + dst_width - 1 - j) = *(padding_ptr->imag + i * dst_width + dst_width - 2 - j);
					}
				}
			}
			else
			{
				for (i = *padding - 1; i >= 0; i--)
				{
					for (j = dst_width - 1; j >= 0; j--)
					{
						*(padding_ptr->real + i * dst_width + j) = 0;
						*(padding_ptr->imag + i * dst_width + j) = 0;
						*(padding_ptr->real + (dst_height - 1 - i) * dst_width + j) = 0;
						*(padding_ptr->imag + (dst_height - 1 - i) * dst_width + j) = 0;
					}
				}
				for (j = *(padding + 1) - 1; j >= 0; j--)
				{
					for (i = dst_height - 1; i >= 0; i--)
					{
						*(padding_ptr->real + i * dst_width + j) = 0;
						*(padding_ptr->imag + i * dst_width + j) = 0;
						*(padding_ptr->real + i * dst_width + dst_width - 1 - j) = 0;
						*(padding_ptr->imag + i * dst_width + dst_width - 1 - j) = 0;
					}
				}
			}
			/*printf("\n\n\nPadding Data:\n");
			for (i = 0; i < dst_height; i++)
			{
				for (j = 0; j < dst_width; j++)
					printf("%f,%f\t", *(padding_ptr->real + i * dst_width + j), *(padding_ptr->imag + i * dst_width + j));
				printf("\n");
			}
			printf("\n\n\n");*/
			padding_height = dst_height;
			padding_width = dst_width;
			pixel_num = (int64_t)(*(padding) << 1) - *(kernel_size)+1;
			dst_height = pixel_num + *(length);
			pixel_num = (int64_t)(*(padding + 1) << 1) - *(kernel_size + 1) + 1;
			dst_width = pixel_num + *(length + 1);
			pixel_num = (int64_t)dst_height * dst_width;
			out_ptr = (Complex*)malloc(sizeof(Complex));
			out_ptr->real = (double_t*)malloc(sizeof(double_t) * pixel_num);
			out_ptr->imag = (double_t*)malloc(sizeof(double_t) * pixel_num);
			//printf("Out (%d,%d)->(%d,%d)\n", *(length), *(length + 1), dst_height, dst_width);
			for (i = dst_height - 1; i >= 0; i--)
			{
				for (j = dst_width - 1; j >= 0; j--)
				{
					real = imag = 0;
					for (k = *(kernel_size)-1; k >= 0; k--)
					{
						for (l = *(kernel_size + 1) - 1; l >= 0; l--)
						{
							tmp_real = *(conv_kernel->real + k * *(kernel_size + 1) + l);
							tmp_imag = *(conv_kernel->imag + k * *(kernel_size + 1) + l);
							real += *(padding_ptr->real + (i + k) * padding_width + j + l) * tmp_real -
								*(padding_ptr->imag + (i + k) * padding_width + j + l) * tmp_imag;
							imag += *(padding_ptr->real + (i + k) * padding_width + j + l) * tmp_imag +
								*(padding_ptr->imag + (i + k) * padding_width + j + l) * tmp_real;
							//printf("kernel %d, %f,%f\n", k** (kernel_size + 1) + l, tmp_real, tmp_imag);
						}

					}
					*(out_ptr->real + i * dst_width + j) = real;
					*(out_ptr->imag + i * dst_width + j) = imag;
				}
			}
		}
		else
		{
			;
		}
	}
	free_complex(padding_ptr);
	return out_ptr;
}


/* free the complex pointer
 * x(inout): ptr
 */
void free_complex(Complex* x)
{
	free(x->real);
	free(x->imag);
	free(x);
}