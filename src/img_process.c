#include"../include/img_process.h"
#pragma warning(disable:4996)

/* HW1 */

/* the kernel of biCubic
 * file_header_ptr(inout): file header pointer
 * info_header_ptr(inout): info header pointer
 */
void biCubic(float_t* rt_value)
{
	double_t a = -1;
	double_t x = *rt_value;
	x = x > 0 ? x : -x;
	if (x <= 1)
	{
		*rt_value = (float_t)(1 - (3 + a) * pow(x, 2) + (2 + a) * pow(x, 3));
	}
	else if (x < 2)
	{
		*rt_value = (float_t)(-4 * a + 8 * a * x - 5 * a * pow(x, 2) + a * pow(x, 3));
	}
	else
	{
		*rt_value = 0;
	}
}

/* interpolation
 * data_ptr(in): origin image
 * type_interpolation(in): which type interpolation you want to use
		0 -> nearest
		1 -> bilinear
		else bicubic
 * scale_factor(in): scale factor of image from (H_component,W)->(H_component*factor,W*factor)
 */
BmpImage* interpolation(const BmpImage* data_ptr, const uint8_t type_interpolation, const float scale_factor)
{
	BmpImage* dst_data_ptr = copyBmpImagePtr(data_ptr, 255);
	float_t times;
	int32_t src_height, src_width, dst_height, dst_width;
	int32_t i, j;
	uint8_t a, b, c, d;
	double_t tmp_x, tmp_y, u, v;
	int32_t x0, y0;
	double_t pixel = 0;
	float_t dx_cubic, dy_cubic;
	int32_t idx_row, idx_col;
	if (1 == scale_factor)
	{
		printf("No need to interpolation!\n");
	}
	else
	{
		times = 1 / scale_factor;
		src_height = data_ptr->info_header->bi_height;
		src_width = data_ptr->info_header->bi_width;
		dst_height = (int32_t)round((double_t)data_ptr->info_header->bi_height * scale_factor);
		dst_width = (int32_t)round((double_t)data_ptr->info_header->bi_width * scale_factor);
		dst_data_ptr->info_header->bi_height = dst_height;
		dst_data_ptr->info_header->bi_width = dst_width;
		dst_data_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * dst_width * dst_height);
		if (0 == type_interpolation)
		{
			/* 0 stands for Nearest neighbor interpolation 最邻近插值 */
			printf("Making Nearest neighbor interpolation,time_x=%f,time_y=%f\n", scale_factor, scale_factor);

			for (i = dst_height - 1; i >= 0; i--)
				for (j = dst_width - 1; j >= 0; j--)
				{
					x0 = (int32_t)round((double_t)times * i - 1e-10);
					y0 = (int32_t)round((double_t)times * j - 1e-10);
					*(dst_data_ptr->DATA + i * dst_width + j)
						= *(data_ptr->DATA + x0 * src_width + y0);
				}
		}
		else if (1 == type_interpolation)
		{
			/* 1 stands for Bilinear interpolation 双线性插值 */
			printf("Making Bilinear interpolation,time_x=%f,time_y=%f\n", scale_factor, scale_factor);
			for (i = dst_height - 1; i >= 0; i--)
				for (j = dst_width - 1; j >= 0; j--)
				{
					tmp_x = ((double_t)i + 1) * times - 1;
					tmp_y = ((double_t)j + 1) * times - 1;
					x0 = (int32_t)tmp_x; y0 = (int32_t)tmp_y;
					u = tmp_x - x0; v = tmp_y - y0;
					a = *(data_ptr->DATA + x0 * src_width + y0);
					if (x0 + 1 >= src_height && y0 + 1 >= src_width)
					{
						b = c = d = a;
					}
					else if (x0 + 1 >= src_height)
					{
						b = a;
						d = c = *(data_ptr->DATA + x0 * src_width + y0 + 1);
					}
					else if (y0 + 1 >= src_width)
					{
						d = b = *(data_ptr->DATA + (x0 + 1) * src_width + y0);
						c = a;
					}
					else
					{
						b = *(data_ptr->DATA + (x0 + 1) * src_width + y0);
						c = *(data_ptr->DATA + x0 * src_width + y0 + 1);
						d = *(data_ptr->DATA + (x0 + 1) * src_width + y0 + 1);
					}
					*(dst_data_ptr->DATA + i * dst_width + j) = (uint8_t)(a * (1 - u) * (1 - v) + b * u * (1 - v) +
						c * v * (1 - u) + u * v * d + 0.001);
				}
		}
		else
		{
			/* other stands for bicubic interpolation 双三次插值 */
			printf("Making bicubic interpolation,time_x=%f,time_y=%f\n", scale_factor, scale_factor);
			for (i = dst_height - 1; i >= 0; i--)
				for (j = dst_width - 1; j >= 0; j--)
				{
					tmp_x = ((double_t)i * times);
					tmp_y = ((double_t)j * times);
					x0 = (int32_t)tmp_x; y0 = (int32_t)tmp_y;
					// dx dy
					u = tmp_x - x0; v = tmp_y - y0;
					pixel = 0;
					for (idx_row = 2; idx_row >= -1; idx_row--)
					{
						dx_cubic = (float_t)(idx_row - u);
						biCubic(&dx_cubic);
						for (idx_col = 2; idx_col >= -1; idx_col--)
						{
							if ((x0 + idx_row) < 0 ||
								(y0 + idx_col) < 0 ||
								(x0 + idx_row) >= src_height ||
								(y0 + idx_col) >= src_width)
							{
								continue;
							}
							dy_cubic = (float_t)(idx_col - v);
							biCubic(&dy_cubic);

							pixel += (double_t)(*(data_ptr->DATA + (x0 + idx_row) * src_width + y0 + idx_col))
								* ((double_t)dx_cubic * dy_cubic);

						}
					}
					pixel = round(pixel) >= 0 ? round(pixel) : 0;
					*(dst_data_ptr->DATA + i * dst_width + j) = (uint8_t)(pixel > 255 ? 255 : pixel);
				}
		}
	}
	printf("Finish interpolated from(%d,%d) to (%d,%d)\n", data_ptr->info_header->bi_height,
		data_ptr->info_header->bi_width, dst_data_ptr->info_header->bi_height, dst_data_ptr->info_header->bi_width);

	return dst_data_ptr;
}

/* HW2 */

/* histogram equalization
 * data_ptr(in): origin image
 */
BmpImage* histogramEqualization(const BmpImage* data_ptr)
{
	BmpImage* dst_data_ptr = copyBmpImagePtr(data_ptr, 0);
	int32_t width = dst_data_ptr->info_header->bi_width;
	int32_t height = dst_data_ptr->info_header->bi_height;
	int32_t pixel_number = width * height;
	double_t pixel_number_inverse = 1.0 / (double_t)pixel_number;
	double_t gray_prob_equal[256];
	double_t gray_gray_distribution[256];
	int32_t i, j;
	HSIInfo* hsi_ptr = NULL;
	uint8_t* tmp_data = (uint8_t*)malloc(sizeof(uint8_t) * width * height);
	/* initialize array */
	for (i = 255; i >= 0; i--)
	{
		gray_prob_equal[i] = 0;
		gray_gray_distribution[i] = 0;
	}
	if (8 == data_ptr->info_header->bi_bit_count) // 1 channel
	{
		/* 统计概率pdf -> gray_prob p_r(r_k)=\frac{n_k}{MN}*/
		for (i = height - 1; i >= 0; i--)
			for (j = width - 1; j >= 0; j--)
				*(tmp_data + i * width + j) = *(data_ptr->DATA + i * width + j);

	}
	else if (24 == data_ptr->info_header->bi_bit_count) // 3 channel
	{
		hsi_ptr = bgr2hsi(data_ptr);
		for (i = height - 1; i >= 0; i--)
			for (j = width - 1; j >= 0; j--)
				*(tmp_data + i * width + j) = round(*(hsi_ptr->I_component + i * width + j) * 255.0);
	}
	else
	{
		;
	}
	for (i = height - 1; i >= 0; i--)
		for (j = width - 1; j >= 0; j--)
			gray_prob_equal[*(tmp_data + i * width + j)] += 1;
	for (i = 255; i >= 0; i--)
		gray_prob_equal[i] = (double_t)gray_prob_equal[i] * pixel_number_inverse;
	/* 计算累计密度cdf -> gray_gray_distribution*/
	/*
	均衡 s_k = T(r_k)=(L-1)\sum\limits_{j=0}^{k}p_r(r_j) k = 0,1,...,L-1
	L = 256 -> L-1 = 255 -> 255 * CDF +1e-1(避免浮点数带来的误差导致的像素不正确)
	*/
	gray_gray_distribution[0] = gray_prob_equal[0];
	for (i = 254; i >= 0; i--)
	{
		j = 255 - i;
		gray_gray_distribution[j] = gray_gray_distribution[j - 1] + gray_prob_equal[j];
	}
	for (i = 255; i >= 0; i--)
		gray_prob_equal[i] = round(255 * gray_gray_distribution[i]);
	/* 赋值给图像，生成均衡结果 */
	if (8 == data_ptr->info_header->bi_bit_count) // 1 channel
	{
		for (i = height - 1; i >= 0; i--)
			for (j = width - 1; j >= 0; j--)
				*(dst_data_ptr->DATA + i * width + j)
				= (uint8_t)gray_prob_equal[*(tmp_data + i * width + j)];
	}
	else if (24 == data_ptr->info_header->bi_bit_count) // 3 channel
	{
		for (i = height - 1; i >= 0; i--)
			for (j = width - 1; j >= 0; j--)
				*(hsi_ptr->I_component + i * width + j)
				= ((uint8_t)gray_prob_equal[*(tmp_data + i * width + j)]) / 255.0;
		hsi2bgr(dst_data_ptr, hsi_ptr);
		free_HSI(hsi_ptr);
	}
	else
	{
		;
	}
	free(tmp_data);
	return dst_data_ptr;
}

/* HW3 */

/* bubble sort(smallest->largest)
 * data_array(inout): array which should be sorted
 * length(in): array length
 */
void sort_array(uint8_t* data_array, const uint64_t length)
{
	uint8_t processed = 1;
	int32_t largest_position = 0;
	uint8_t tmp_data = 0;
	int32_t i, j;
	for (i = length - 1; i > 0; i--)
	{
		processed = 0;
		largest_position = i;
		for (j = i - 1; j >= 0; j--)
		{
			if (*(data_array + largest_position) < *(data_array + j))
			{
				largest_position = j;
				processed = 1;
			}
		}
		if (processed == 1)
		{
			tmp_data = *(data_array + largest_position);
			*(data_array + largest_position) = *(data_array + i);
			*(data_array + i) = tmp_data;
		}
		else
		{
			;
		}
	}
}

/*
* 中值滤波代码
* 输入: 图像数据	->	const BmpImage* data_ptr
*		滤波核大小	->	const int32_t kernel_size
* 输出: 中值滤波后的数据 -> BmpImage* dst_data_ptr
*/
BmpImage* medianFilter(const BmpImage* data_ptr, const int32_t kernel_size)
{
	BmpImage* dst_data_ptr = copyBmpImagePtr(data_ptr, 0);
	const int32_t src_width = dst_data_ptr->info_header->bi_width;
	const int32_t src_height = dst_data_ptr->info_header->bi_height;
	const int32_t edge_size = (int32_t)((kernel_size - 1.0) / 2);
	const int32_t dst_width = src_width + kernel_size - 1;
	const int32_t dst_height = src_height + kernel_size - 1;
	uint8_t* padding_matrix;
	uint8_t* median_array;
	int32_t i, j, k, l;
	uint8_t var_search_median = 0;
	int32_t position = 0;
	uint8_t search = 0;
	if (kernel_size <= 0)
	{
		printf("kernel_size(%d) should larger than 0\n", kernel_size);
	}
	else if (((src_height - 1) <= 2 * edge_size)
		|| ((src_width - 1) <= 2 * edge_size))
	{
		printf("kernel_size(%d) to large, return the no processed pictures\n", kernel_size);
	}
	else if (kernel_size % 2 != 1)
	{
		printf("kernel_size(%d) should be odd, return the no processed pictures\n", kernel_size);
	}
	else
	{
		/* 填充 */
		padding_matrix = (uint8_t*)malloc(sizeof(uint8_t) * dst_width * dst_height);
		printf("Pic padding from (%d,%d)->(%d,%d)\n", src_height, src_width, dst_height, dst_width);

		for (i = dst_height - 1; i >= 0; i--)
			for (j = dst_width - 1; j >= 0; j--)
				*(padding_matrix + i * dst_width + j) = 0;
		for (i = src_height - 1; i >= 0; i--)
			for (j = src_width - 1; j >= 0; j--)
				*(padding_matrix + (i + edge_size) * dst_width + j + edge_size)
				= *(data_ptr->DATA + src_width * i + j);
		// 声明核
		median_array = (uint8_t*)malloc(sizeof(uint8_t) * kernel_size * kernel_size);
		for (i = src_height - 1; i >= 0; i--)
		{
			for (j = src_width - 1; j >= 0; j--)
			{
				// 找中值
				for (k = kernel_size - 1; k >= 0; k--)
					for (l = kernel_size - 1; l >= 0; l--)
						*(median_array + k * kernel_size + l)
						= *(padding_matrix + (i + k) * dst_width + j + l);
				sort_array(median_array, (uint64_t)kernel_size * kernel_size);
				position = (kernel_size * kernel_size - 1) / 2;
				search = 0;
				var_search_median = *(median_array + position);
				/* 清除密集异常点处的点的椒盐噪声 */
				if (var_search_median == *(median_array))
				{
					search = 1;
					while (var_search_median == *(median_array))
					{
						if (position == kernel_size * kernel_size - 1)
						{
							search = 2;
							break;
						}
						position += 1;
						var_search_median = *(median_array + position);
					}

				}
				if (var_search_median == *(median_array + kernel_size * kernel_size - 1))
				{
					if (1 <= search)
						search = 2;
					else
					{
						while (var_search_median == *(median_array + kernel_size * kernel_size - 1))
						{
							if (position == 0)
							{
								search = 2;
								break;
							}
							position -= 1;
							var_search_median = *(median_array + position);
						}
					}
				}
				if (search >= 2)
					var_search_median = (uint8_t)(*(median_array + kernel_size * kernel_size - 1)
						+ *(median_array)) / 2;
				*(dst_data_ptr->DATA + i * src_width + j) = var_search_median;
			}
		}
		free(padding_matrix);
		free(median_array);
	}
	return dst_data_ptr;
}

/*
* 高斯低通滤波器代码
* 输入: 图像数据	->	const BmpImage* data_ptr
*		滤波核大小	->	const int32_t kernel_size
*		方差        ->  const double_t sigma
* 输出: 中值滤波后的数据 -> BmpImage* dst_data_ptr
*/
BmpImage* GaussianLowPassFilter(const BmpImage* data_ptr, const int32_t kernel_size, const double_t sigma)
{
	BmpImage* dst_data_ptr = copyBmpImagePtr(data_ptr, 0);
	const int32_t src_width = dst_data_ptr->info_header->bi_width;
	const int32_t src_height = dst_data_ptr->info_header->bi_height;
	const int32_t edge_size = (int32_t)((kernel_size - 1.0) / 2);
	const int32_t dst_width = src_width + kernel_size - 1;
	const int32_t dst_height = src_height + kernel_size - 1;
	double_t* GaussianTemplate;
	uint8_t* padding_matrix;
	int32_t x, y;
	double_t sum_of_GaussianTemplate = 0;
	int32_t i, j, k, l;
	if (sigma <= 0)
	{
		printf("sigma(%lf) should larger than 0\n", sigma);
	}
	else
	{
		if (kernel_size <= 0)
		{
			printf("kernel_size(%d) should larger than 0\n", kernel_size);
		}
		else
		{
			/* 高斯核声明 */
			GaussianTemplate = (double_t*)malloc(sizeof(double_t) * kernel_size * kernel_size);
			/* 填充边缘 直接end2end */
			padding_matrix = (uint8_t*)malloc(sizeof(uint8_t) * dst_width * dst_height);
			printf("Pic padding from (%d,%d)->(%d,%d)\n", src_height, src_width, dst_height, dst_width);
			for (i = dst_height - 1; i >= 0; i--)
				for (j = dst_width - 1; j >= 0; j--)
					*(padding_matrix + i * dst_width + j) = 0;
			for (i = src_height - 1; i >= 0; i--)
				for (j = src_width - 1; j >= 0; j--)
					*(padding_matrix + (i + edge_size) * dst_width + j + edge_size)
					= *(data_ptr->DATA + src_width * i + j);


			/* 求高斯核 */
			for (i = kernel_size - 1; i >= 0; i--)
				for (j = kernel_size - 1; j >= 0; j--)
				{
					x = i - edge_size;
					y = j - edge_size;
					*(GaussianTemplate + i * kernel_size + j) =
						exp(-(x * x + y * y) / (2 * sigma * sigma));
					sum_of_GaussianTemplate += *(GaussianTemplate + i * kernel_size + j);
				}
			/* 归一化 */
			for (i = kernel_size * kernel_size - 1; i >= 0; i--)
				*(GaussianTemplate + i) /= sum_of_GaussianTemplate;
			/* 计算 */
			for (i = src_height - 1; i >= 0; i--)
				for (j = src_width - 1; j >= 0; j--)
				{
					sum_of_GaussianTemplate = 0;
					for (k = kernel_size - 1; k >= 0; k--)
						for (l = kernel_size - 1; l >= 0; l--)
						{
							sum_of_GaussianTemplate +=
								*(padding_matrix + (i + k) * dst_width + j + l)
								* (*(GaussianTemplate + k * kernel_size + l));
						}
					*(dst_data_ptr->DATA + i * src_width + j) = (uint8_t)(sum_of_GaussianTemplate + 0.01);
				}
			free(padding_matrix);
			free(GaussianTemplate);
		}
	}
	return dst_data_ptr;
}

/* HW4 */

/* bgr image to hsi image
 * data_ptr(in): bgr data
 */
HSIInfo* bgr2hsi(const BmpImage* data_ptr)
{
	HSIInfo* hsi_ptr = (HSIInfo*)malloc(sizeof(HSIInfo));
	double_t B, G, R, theta;
	int32_t i, j;
	int32_t height = data_ptr->info_header->bi_height;
	int32_t width = data_ptr->info_header->bi_width;
	int32_t equal_width = width * 3;
	int32_t size = height * width;

	hsi_ptr->H_component = (double_t*)malloc(sizeof(double_t) * size);
	hsi_ptr->S_component = (double_t*)malloc(sizeof(double_t) * size);
	hsi_ptr->I_component = (double_t*)malloc(sizeof(double_t) * size);
	for (i = height - 1; i >= 0; i--)
	{
		for (j = width - 1; j >= 0; j--)
		{
			B = (double_t) * (data_ptr->DATA + i * equal_width + j * 3) / 255.0;
			G = (double_t) * (data_ptr->DATA + i * equal_width + j * 3 + 1) / 255.0;
			R = (double_t) * (data_ptr->DATA + i * equal_width + j * 3 + 2) / 255.0;

			/* theta = ((R-G)+(R-B))/2
			 * theta = theta/sqrt((R-G)^2+(R-B)(G-B))
			 * theta = arccos(theta)
			 */
			theta = ((R - G) + (R - B)) * 0.5;
			theta = theta / (sqrt(pow((R - G), 2) + (R - B) * (G - B)) + 1e-15);
			theta = acos(theta);
			*(hsi_ptr->S_component + i * width + j) = 1 - 3.0 * min(min(R, B), G) / (R + G + B + 1e-15);
			*(hsi_ptr->H_component + i * width + j) = abs(*(hsi_ptr->S_component + i * width + j)) < 1e-15 ? 0 : ((B > G)
				? 2 * M_PI - theta : theta);
			*(hsi_ptr->I_component + i * width + j) = (R + G + B) / 3.0;
			//printf("Row:%d,Col:%d\tH=%f,S_component=%f,I_component=%f\n", i, j, *(hsi_ptr->H_component + i * width + j),
			//	*(hsi_ptr->S_component + i * width + j), *(hsi_ptr->I_component + i * width + j));
		}
	}
	return hsi_ptr;
}

/* hsi image to bgr image
 * data_ptr(out): bgr data
 * hsi_ptr(in): hsi data
 */
void hsi2bgr(BmpImage* data_ptr, const HSIInfo* hsi_ptr)
{
	int32_t i, j;
	double_t H_component, S_component, I_component, R, G, B;
	int32_t height = data_ptr->info_header->bi_height;
	int32_t width = data_ptr->info_header->bi_width;
	int64_t equal_width = (int64_t)(3.0 * width);
	double_t angle = 0.66666666666666667 * M_PI;
	for (i = height - 1; i >= 0; i--)
	{
		for (j = width - 1; j >= 0; j--)
		{
			H_component = *(hsi_ptr->H_component + i * width + j);
			S_component = *(hsi_ptr->S_component + i * width + j);
			I_component = *(hsi_ptr->I_component + i * width + j);
			/*
			 *	b = *(data_ptr->DATA + i * equal_width + j * 3) / 255.0;
			 *	g = *(data_ptr->DATA + i * equal_width + j * 3 + 1) / 255.0;
			 *	r = *(data_ptr->DATA + i * equal_width + j * 3 + 2) / 255.0;
			 */
			if (H_component <= angle)
			{
				B = I_component * (1 - S_component);
				R = I_component * (1 + (S_component * cos(H_component)) / cos(angle * 0.5 - H_component));
				G = 3 * I_component - (R + B);
			}
			else if (H_component <= 2.0 * angle && H_component > angle)
			{
				H_component = H_component - angle;
				R = I_component * (1 - S_component);
				G = I_component * (1 + (S_component * cos(H_component)) / cos(angle * 0.5 - H_component));
				B = 3 * I_component - (R + G);
			}
			else
			{
				H_component = H_component - 2 * angle;
				G = I_component * (1 - S_component);
				B = I_component * (1 + (S_component * cos(H_component)) / cos(angle * 0.5 - H_component));
				R = 3 * I_component - (G + B);
			}
			/* blue */
			*(data_ptr->DATA + i * equal_width + j * 3) = (uint8_t)(B * 255.0 + 0.001);
			/* green */
			*(data_ptr->DATA + i * equal_width + j * 3 + 1) = (uint8_t)(G * 255.0 + 0.001);
			/* red */
			*(data_ptr->DATA + i * equal_width + j * 3 + 2) = (uint8_t)(R * 255.0 + 0.001);
		}
	}
}

/* free hsi data
 * hsi_ptr(in): hsi data
 */
void free_HSI(HSIInfo* data_ptr)
{
	free(data_ptr->H_component);
	free(data_ptr->S_component);
	free(data_ptr->I_component);
	free(data_ptr);
}


/* HW5 */

/* fft(dft) out -> normalize_factor * log(abs(fft(padding(data))))
 * data_ptr(in): data
 */
BmpImage* fft(const BmpImage* data_ptr)
{
	BmpImage* out_ptr = copyBmpImagePtr(data_ptr, 255);
	int32_t idx_pow, src_height, src_width, dst_height, dst_width;
	int32_t i, j;
	int32_t* dimension = (int32_t*)malloc(sizeof(int32_t) * 2);
	double_t length_d, min_idx;
	Complex* complex_data_ptr = (Complex*)malloc(sizeof(Complex));
	Complex* fft_out_ptr = NULL;
	src_height = data_ptr->info_header->bi_height;
	/* find padding height and width, both of them should be 2^m,2^n
	 * I didn't implement  3, 5, 7-base FFT
	 */
	length_d = src_height;
	idx_pow = 0;
	for (; length_d > 1; )
	{
		length_d *= 0.5;
		idx_pow++;
	}
	out_ptr->info_header->bi_height = dst_height = pow(2, idx_pow);
	length_d = src_width = data_ptr->info_header->bi_width;
	idx_pow = 0;
	for (; length_d > 1; )
	{
		length_d *= 0.5;
		idx_pow++;
	}
	out_ptr->info_header->bi_width = dst_width = pow(2, idx_pow);
	idx_pow = dst_height * dst_width;
	*dimension = dst_height;
	*(dimension + 1) = dst_width;
	out_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * idx_pow);
	complex_data_ptr->real = (double_t*)malloc(sizeof(double_t) * idx_pow);
	complex_data_ptr->imag = (double_t*)malloc(sizeof(double_t) * idx_pow);
	/* copy data to complex array */
	for (i = dst_height - 1; i >= 0; i--)
	{
		for (j = dst_width - 1; j >= 0; j--)
		{
			*(complex_data_ptr->real + i * dst_width + j) = (i >= src_height || j >= src_width) ? 0 :
				*(data_ptr->DATA + i * src_width + j);
			*(complex_data_ptr->imag + i * dst_width + j) = 0;
		}
	}
	/* 2d fft */
	fft_out_ptr = fft2d(complex_data_ptr, dst_height, dst_width);
	free(complex_data_ptr->real); free(complex_data_ptr->imag);
	/* 2d fftshift */
	complex_data_ptr = fftshift(fft_out_ptr, 2, dimension);
	free(fft_out_ptr->imag);
	length_d = 0;
	min_idx = 255;
	/* find max value */
	for (i = out_ptr->info_header->bi_height - 1; i >= 0; i--)
	{
		for (j = out_ptr->info_header->bi_width - 1; j >= 0; j--)
		{
			*(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) = log(sqrt(
				pow(*(complex_data_ptr->real + i * out_ptr->info_header->bi_width + j), 2) +
				pow(*(complex_data_ptr->imag + i * out_ptr->info_header->bi_width + j), 2)
			));
			length_d = length_d < *(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) ?
				*(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) : length_d;
			min_idx = min_idx > *(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) ?
				*(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) : min_idx;
		}
	}
	//printf("\nmax=%f,min=%f\n", length_d, min_idx);
	/* let max value map to 0~255 */
	/* \hat{x} = (x-min)/(max-min) */
	length_d = 255.0 / (length_d - min_idx);
	/* map double to uint8 */
	for (i = out_ptr->info_header->bi_height - 1; i >= 0; i--)
	{
		for (j = out_ptr->info_header->bi_width - 1; j >= 0; j--)
		{
			*(out_ptr->DATA + i * out_ptr->info_header->bi_width + j) = (uint8_t)(
				length_d *
				(*(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) - min_idx)
				+ 0.1);
		}
	}
	free(fft_out_ptr->real);
	free(fft_out_ptr);
	free_complex(complex_data_ptr);
	return out_ptr;
}

/* DCT Discrete cosine transform.
 * data_ptr(in): image information
 */
BmpImage* dct(const BmpImage* data_ptr)
{
	BmpImage* out_ptr = copyBmpImagePtr(data_ptr, 255);
	Complex* dct_ptr = NULL;
	Complex* complex_data_ptr = (Complex*)malloc(sizeof(Complex));
	int32_t idx_pow, src_height, src_width, dst_height, dst_width;
	double_t length_d, min_idx;
	int32_t i, j;
	src_height = data_ptr->info_header->bi_height;
	/* find padding height and width, both of them should be 2^m,2^n
	 * I didn't implement  3, 5, 7-base FFT
	 */
	length_d = src_height;
	idx_pow = 0;
	for (; length_d > 1; )
	{
		length_d *= 0.5;
		idx_pow++;
	}
	out_ptr->info_header->bi_height = dst_height = pow(2, idx_pow);
	length_d = src_width = data_ptr->info_header->bi_width;
	idx_pow = 0;
	for (; length_d > 1; )
	{
		length_d *= 0.5;
		idx_pow++;
	}
	out_ptr->info_header->bi_width = dst_width = pow(2, idx_pow);
	idx_pow = dst_height * dst_width;
	out_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * idx_pow);
	complex_data_ptr->real = (double_t*)malloc(sizeof(double_t) * idx_pow);
	complex_data_ptr->imag = (double_t*)malloc(sizeof(double_t) * idx_pow);
	/* copy data to complex array */
	for (i = dst_height - 1; i >= 0; i--)
	{
		for (j = dst_width - 1; j >= 0; j--)
		{
			*(complex_data_ptr->real + i * dst_width + j) = (i >= src_height || j >= src_width) ? 0 :
				*(data_ptr->DATA + i * src_width + j);
			*(complex_data_ptr->imag + i * dst_width + j) = 0;
		}
	}
	/*int32_t* dimension = (int32_t*)malloc(sizeof(int32_t) * 2);
	*dimension = dst_height;
	*(dimension + 1) = dst_width;
	Complex* dct_ptr1 = dct2d(complex_data_ptr, dst_height, dst_width, 1);
	 dct_ptr = fftshift(dct_ptr1, 2, dimension);*/
	dct_ptr = dct2d(complex_data_ptr, dst_height, dst_width, 1);
	length_d = 0;
	min_idx = 255;
	/* find max value */
	for (i = dst_height - 1; i >= 0; i--)
	{
		for (j = dst_width - 1; j >= 0; j--)
		{
			*(complex_data_ptr->real + i * out_ptr->info_header->bi_width + j) = log(sqrt(
				pow(*(dct_ptr->real + i * dst_width + j), 2) +
				pow(*(dct_ptr->imag + i * dst_width + j), 2)
			));
			length_d = length_d < *(complex_data_ptr->real + i * dst_width + j) ?
				*(complex_data_ptr->real + i * dst_width + j) : length_d;
			min_idx = min_idx > *(complex_data_ptr->real + i * dst_width + j) ?
				*(complex_data_ptr->real + i * dst_width + j) : min_idx;
		}
	}
	//printf("\nmax=%f,min=%f\n", length_d, min_idx);
	/* let max value map to 0~255 */
	/* \hat{x} = (x-min)/(max-min) */
	length_d = 255.0 / (length_d - min_idx);
	/* map double to uint8 */
	for (i = out_ptr->info_header->bi_height - 1; i >= 0; i--)
	{
		for (j = out_ptr->info_header->bi_width - 1; j >= 0; j--)
		{
			*(out_ptr->DATA + i * out_ptr->info_header->bi_width + j) = (uint8_t)(
				length_d *
				(*(complex_data_ptr->real + i * out_ptr->info_header->bi_width + j) - min_idx)
				+ 0.1);
		}
	}
	free_complex(complex_data_ptr);
	free_complex(dct_ptr);
	return out_ptr;

}


/* according to matlab dwt2->using conv block to dwt
 * data_ptr(in): data
 */
BmpImage* dwt(const BmpImage* data_ptr)
{
	Complex* tmp_ptr = (Complex*)malloc(sizeof(Complex));
	Complex* complex_ptr = NULL;
	BmpImage* out_ptr = copyBmpImagePtr(data_ptr, 0);
	int32_t pixel_num = data_ptr->info_header->bi_height * data_ptr->info_header->bi_width;
	int32_t i;
	double_t max_var, min_var;
	tmp_ptr->real = (double_t*)malloc(sizeof(double_t) * pixel_num);
	tmp_ptr->imag = (double_t*)malloc(sizeof(double_t) * pixel_num);
	for (i = pixel_num - 1; i >= 0; i--)
	{
		*(tmp_ptr->real + i) = *(data_ptr->DATA + i);
		*(tmp_ptr->imag + i) = 0;
	}
	complex_ptr = dwt2D(tmp_ptr, data_ptr->info_header->bi_height, data_ptr->info_header->bi_width);
	min_var = 65536;
	max_var = 0;
	for (i = pixel_num - 1; i >= 0; i--)
	{
		max_var = max_var < *(complex_ptr->real + i) ? *(complex_ptr->real + i) : max_var;
		min_var = min_var > *(complex_ptr->real + i) ? *(complex_ptr->real + i) : min_var;
		//printf("\nmax=%f,min=%f,data=%f", max_var, min_var, *(complex_ptr->real + i));
	}
	min_var = min_var > min(-max_var, -255) ? min_var : min(-max_var, -255);
	max_var = 255.0 / (max_var - min_var);
	for (i = pixel_num - 1; i >= 0; i--)
	{
		*(out_ptr->DATA + i) = (uint8_t)(max_var * (*(complex_ptr->real + i) - min_var));
	}
	free_complex(tmp_ptr);
	free_complex(complex_ptr);
	return out_ptr;
}

/* HW6 */

BmpImage* get_boundary(const BmpImage* data_ptr)
{
	BmpImage* out_ptr = dilate(data_ptr, 3);
	int32_t i;
	int32_t src_width = data_ptr->info_header->bi_width;
	int32_t src_height = data_ptr->info_header->bi_height;
	for (i = src_height * (src_width >> 3) - 1; i > 0; i--)
	{
		//printf("(%x,%x,%x)\t", *(out_ptr->DATA + i), *(data_ptr->DATA + i), ~(*(out_ptr->DATA + i)^ *(data_ptr->DATA + i)));
		*(out_ptr->DATA+i)=~(*(out_ptr->DATA + i)^ *(data_ptr->DATA + i));
	}
	return out_ptr;
}

BmpImage* dilate(const BmpImage* data_ptr, const int32_t kernel)
{
	BmpImage* out_ptr = copyBmpImagePtr(data_ptr, 0);
	int32_t src_width = data_ptr->info_header->bi_width;
	int32_t src_height = data_ptr->info_header->bi_height;
	uint8_t* normal_data;
	uint8_t* dilate_data;
	if (1 == data_ptr->info_header->bi_bit_count)
	{
		normal_data = image2arr(data_ptr);
		dilate_data = dilate_u8(normal_data, src_height, src_width,1);
		back2BmpImage(out_ptr, dilate_data);
		free(normal_data);
		free(dilate_data);
	}
	else 
	{
		printf("current version is not supported %d-bit BMP!\n", data_ptr->info_header->bi_bit_count);
	}

	return out_ptr;
}

uint8_t* dilate_u8(const uint8_t* DATA, const int32_t src_height, const int32_t src_width, const uint8_t kernel)
{
	int32_t i, j;
	int32_t idx1, idx2, idx3, idx4,idx;
	uint8_t* out_data = (uint8_t*)malloc(sizeof(uint8_t) * src_height * src_width);
	for (i = src_height - 1; i >= 0; i--)
	{
		for (j = src_width - 1; j >= 0; j--)
		{
			idx = i * src_width + j;
			idx1 = (i - 1) > 0 ? (i - 1) * src_width +j : 0;
			idx2 = (i + 1) < src_height ? (i + 1) * src_width +j : (src_height-1) * src_width + j;
			idx3 = (j - 1) > 0 ? i * src_width + j - 1 : i * src_width;
			idx4 = (j + 1) < src_width ? i * src_width + j + 1 : i * src_width + src_width - 1;
			if (*(DATA + idx)==1 || *(DATA + idx1) == 1 ||
				*(DATA + idx2) == 1 || *(DATA + idx3) == 1 || *(DATA + idx4) == 1)
			{
				*(out_data + idx) = 1;
			}
			else
			{
				*(out_data + idx) = 0;
			}
		}
	}
	/* 写法二 https://www.cnblogs.com/hrlnw/p/5044402.html */
	return out_data;
}


uint8_t* image2arr(const BmpImage* data_ptr) 
{
	int32_t src_width = data_ptr->info_header->bi_width;
	int32_t src_height = data_ptr->info_header->bi_height;
	int32_t equal_width = src_width >> 3;
	int32_t i, j, k, idx;
	uint8_t* DATA = (uint8_t*)malloc(sizeof(uint8_t) * src_width * src_height);
	for (i = src_height - 1; i >= 0; i--)
	{
		for (j = equal_width - 1; j >= 0; j--)
		{
			idx = i * src_width + (j << 3);
			for (k = 7; k >= 0; k--)
			{
				*(DATA + idx + k) = (*(data_ptr->DATA + i * equal_width + j)>>k)&0x01;
			}
		}
	}
	return DATA;
}


void back2BmpImage(BmpImage* data_ptr, const uint8_t* DATA)
{
	int32_t src_width = data_ptr->info_header->bi_width;
	int32_t src_height = data_ptr->info_header->bi_height;
	int32_t equal_width = src_width >> 3;
	int32_t i, j, k, idx;
	for (i = src_height - 1; i >= 0; i--)
	{
		for (j = equal_width - 1; j >= 0; j--)
		{
			*(data_ptr->DATA + i * equal_width + j) = 0;
			idx = i * src_width + (j << 3);
			for (k = 7; k >= 0; k--)
			{
				*(data_ptr->DATA + i * equal_width + j) += (*(DATA + idx + k) << k);
			}
		}
	}
}

/* HW7 */
BmpImage* local_variance_threshold(const BmpImage* data_ptr )
{
	BmpImage* out_ptr = copyBmpImagePtr(data_ptr, 0);
	int32_t src_width = data_ptr->info_header->bi_width;
	int32_t src_height = data_ptr->info_header->bi_height;
	double_t* std_local = (double_t*)malloc(sizeof(double_t) * src_width * src_height);
	int32_t* power_local = (int32_t*)malloc(sizeof(double_t) * src_width * src_height);
	uint8_t* kernel = (uint8_t*)malloc(sizeof(uint8_t)*9);
	int32_t* kernel_p = (int32_t*)malloc(sizeof(int32_t) * 9);
	double_t mean = 0;
	uint8_t row_limit_sub, row_limit_add, col_limit_sub, col_limit_add;
	int32_t i, j;
	for (j = 8; j >= 0; j--)
		*(kernel + j) = 0;
	for (j = src_height * src_width - 1; j >= 0; j--)
		*(power_local + j) = *(data_ptr->DATA + j) * *(data_ptr->DATA + j);
	for (i = src_height - 1; i >= 0; i--)
	{
		row_limit_sub = i == 0 ? 0 : 1;
		row_limit_add = i == src_height - 1 ? 0 : 1;
		for (j = src_width - 1; j >= 0; j--)
		{
			col_limit_sub = j == 0 ? 0 : 1;
			col_limit_add = j == src_width - 1 ? 0 : 1;
			mean += *(data_ptr->DATA + i * src_width + j);
			*(kernel_p + 0) = *(power_local + (i - row_limit_sub) * src_width + (j - col_limit_sub));
			*(kernel_p + 1) = *(power_local + (i)*src_width + (j - col_limit_sub));
			*(kernel_p + 2) = *(power_local + (i + row_limit_add) * src_width + (j - col_limit_sub));
			*(kernel_p + 3) = *(power_local + (i - row_limit_sub) * src_width + (j));
			*(kernel_p + 4) = *(power_local + (i)*src_width + (j));
			*(kernel_p + 5) = *(power_local + (i + row_limit_add) * src_width + (j));
			*(kernel_p + 6) = *(power_local + (i - row_limit_sub) * src_width + (j + col_limit_add));
			*(kernel_p + 7) = *(power_local + (i)*src_width + (j + col_limit_add));
			*(kernel_p + 8) = *(power_local + (i + row_limit_add) * src_width + (j + col_limit_add));
			*(kernel + 0) = *(data_ptr->DATA + (i - row_limit_sub) * src_width + (j - col_limit_sub));
			*(kernel + 1) = *(data_ptr->DATA + (i)*src_width + (j - col_limit_sub));
			*(kernel + 2) = *(data_ptr->DATA + (i + row_limit_add) * src_width + (j - col_limit_sub));
			*(kernel + 3) = *(data_ptr->DATA + (i - row_limit_sub) * src_width + (j));
			*(kernel + 4) = *(data_ptr->DATA + (i)*src_width + (j));
			*(kernel + 5) = *(data_ptr->DATA + (i + row_limit_add) * src_width + (j));
			*(kernel + 6) = *(data_ptr->DATA + (i - row_limit_sub) * src_width + (j + col_limit_add));
			*(kernel + 7) = *(data_ptr->DATA + (i)*src_width + (j + col_limit_add));
			*(kernel + 8) = *(data_ptr->DATA + (i + row_limit_add) * src_width + (j + col_limit_add));
			*(std_local + i * src_width + j) = cal_std(kernel, kernel_p);
		}
	}
	mean = mean / (src_height * src_width) * 1.5;
	for (j = src_height * src_width - 1; j >= 0; j--)
		*(out_ptr->DATA + j) = (*(data_ptr->DATA + j) > mean) && (*(data_ptr->DATA + j) > 30 * *(std_local + j)) ? 255 : 0;

	free(std_local);
	free(kernel);
	free(kernel_p);
	free(power_local);
	return out_ptr;
}

double_t cal_std(const uint8_t* arr,const int32_t* arr_p)
{
	int32_t i;
	double_t sum_up = 0;
	double_t std_ = 0;
	for (i = 8; i >= 0; i--)
	{
		sum_up += *(arr + i);
	}
	sum_up = -sum_up * sum_up / 9;
	for (i = 8; i >= 0; i--)
	{
		sum_up+= *(arr_p + i);
	}
	sum_up = sqrt(sum_up / 8);
	return sum_up;
}


BmpImage* segmentation_from_roi(const BmpImage* data_ptr, const BmpImage* roi_ptr)
{
	BmpImage* out_ptr = copyBmpImagePtr(data_ptr, 1);
	int32_t src_width = data_ptr->info_header->bi_width;
	int32_t src_height = data_ptr->info_header->bi_height;
	int32_t i, j, k;
	for (j = src_height * src_width - 1; j >= 0; j--)
	{
		if (*(roi_ptr->DATA + j) == 255)
		{
			;
		}
		else
		{
			*(out_ptr->DATA + j) = 0;
		}
	}
	return out_ptr;
}