#pragma warning(disable:4996)
#include "func.h"
/* base function */

/* read BMP data
 * img_path(in): file path
 */
BmpImage* read(const char* img_path)
{
	BmpImage* data_ptr = (BmpImage*)malloc(sizeof(BmpImage));
	data_ptr->file_header = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
	data_ptr->info_header = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	int32_t i;
	int32_t src_height, src_width, offset_address;
	int64_t equal_width, equal_offset_address;
	uint8_t* offset_data;
	FILE* file_ptr = fopen(img_path, "rb");
	printf("Reading path: %s\n", img_path);
	fread(data_ptr->file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
	fread(data_ptr->info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
	if ((!file_ptr) || data_ptr->file_header->bf_type != 19778)
	{
		printf("It is not BMP file or haven't open file! Please check your input!\n");
		exit(-10);
	}
	else
	{
		showInfo(data_ptr->file_header, data_ptr->info_header);
		src_height = data_ptr->info_header->bi_height;
		src_width = data_ptr->info_header->bi_width;
		offset_address = 4 - (src_width % 4); /* symmetry */
		equal_width = src_width * 3;
		equal_offset_address = offset_address * 3;
		if (8 == data_ptr->info_header->bi_bit_count) // 1 channel
		{
			fread(data_ptr->ColorPalette, sizeof(BITMAPColorPalette), 256, file_ptr);
			data_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * src_height * src_width);
			offset_data = (uint8_t*)malloc(sizeof(uint8_t) * offset_address);
			for (i = src_height - 1; i >= 0; i--)
			{
				fread((uint8_t*)(data_ptr->DATA + i * src_width), sizeof(uint8_t), src_width, file_ptr);
				if (offset_address != 4)
				{
					fread(offset_data, sizeof(uint8_t), offset_address, file_ptr);
				}
				else
				{
					;
				}
			}
			free(offset_data);
		}
		else if (24 == data_ptr->info_header->bi_bit_count) // 3 channel
		{
			data_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * src_height * src_width * 3); // bgr排布
			offset_data = (uint8_t*)malloc(sizeof(uint8_t) * equal_offset_address);
			for (i = src_height - 1; i >= 0; i--)
			{
				fread((uint8_t*)(data_ptr->DATA + i * equal_width),
					sizeof(uint8_t), equal_width, file_ptr);
				if (offset_address != 4)
				{
					fread(offset_data, sizeof(uint8_t), equal_offset_address, file_ptr);
				}
				else
				{
					;
				}
			}
			free(offset_data);

		}
		else
		{
			;
		}
	}
	fclose(file_ptr);
	return data_ptr;
}

/* free the malloc data
 * data_ptr(inout): data pointer which should be free
 */
void free_ptr(BmpImage* data_ptr)
{
	free(data_ptr->DATA);
	free(data_ptr->file_header);
	free(data_ptr->info_header);
	free(data_ptr);
}

/* copy the infomation and data from older ptr
 * data_ptr(in): older pointer
 * copy_data(in): 0==copy_data?(data=0):(data=data_ptr->data) 255 == no creat data
 */
BmpImage* copyBmpImagePtr(const BmpImage* data_ptr, const uint8_t copy_data)
{
	/* file_header */
	int32_t i, j;
	BmpImage* new_ptr = (BmpImage*)malloc(sizeof(BmpImage));
	new_ptr->file_header = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
	new_ptr->info_header = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	copyBmpImageInfo(data_ptr, new_ptr);
	if (8 == data_ptr->info_header->bi_bit_count)
	{
		for (int32_t i = 255; i >= 0; i--)
			new_ptr->ColorPalette[i] = data_ptr->ColorPalette[i];
		if (copy_data != 255)
		{
			new_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * new_ptr->info_header->bi_width * new_ptr->info_header->bi_height);
			for (i = new_ptr->info_header->bi_height - 1; i >= 0; i--)
			{
				for (j = new_ptr->info_header->bi_width - 1; j >= 0; j--)
				{
					new_ptr->DATA[i * new_ptr->info_header->bi_width + j] = copy_data == 0 ? 0 :
						data_ptr->DATA[i * new_ptr->info_header->bi_width + j];
				}
			}
		}
		else
		{
			;
		}
	}
	else if (24 == data_ptr->info_header->bi_bit_count)
	{
		if (copy_data != 255)
		{
			new_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * new_ptr->info_header->bi_width
				* new_ptr->info_header->bi_height * 3);
			for (i = new_ptr->info_header->bi_height - 1; i >= 0; i--)
			{
				for (j = new_ptr->info_header->bi_width * 3 - 1; j >= 0; j--)
				{
					*(new_ptr->DATA + i * new_ptr->info_header->bi_width * 3 + j) = copy_data == 0 ? 0 :
						*(data_ptr->DATA + i * new_ptr->info_header->bi_width * 3 + j);
				}
			}
		}
		else
		{
			;
		}
	}
	else
	{
		;
	}
	return new_ptr;
}

/* copy the infomation from older ptr to new ptr
 * data_ptr(in): older pointer
 * new_ptr(inout): new pointer
 */
void copyBmpImageInfo(const BmpImage* data_ptr, BmpImage* new_ptr)
{
	/* file_header */
	new_ptr->file_header->bf_off_bits = data_ptr->file_header->bf_off_bits;
	new_ptr->file_header->bf_reserved_0 = data_ptr->file_header->bf_reserved_0;
	new_ptr->file_header->bf_reserved_1 = data_ptr->file_header->bf_reserved_1;
	new_ptr->file_header->bf_size = data_ptr->file_header->bf_size;
	new_ptr->file_header->bf_type = data_ptr->file_header->bf_type;
	/* info header */
	new_ptr->info_header->bi_bit_count = data_ptr->info_header->bi_bit_count;
	new_ptr->info_header->bi_clr_important = data_ptr->info_header->bi_clr_important;
	new_ptr->info_header->bi_clr_used = data_ptr->info_header->bi_clr_used;
	new_ptr->info_header->bi_compression = data_ptr->info_header->bi_compression;
	new_ptr->info_header->bi_height = data_ptr->info_header->bi_height;
	new_ptr->info_header->bi_planes = data_ptr->info_header->bi_planes;
	new_ptr->info_header->bi_size = data_ptr->info_header->bi_size;
	new_ptr->info_header->bi_size_image = data_ptr->info_header->bi_size_image;
	new_ptr->info_header->bi_width = data_ptr->info_header->bi_width;
	new_ptr->info_header->bi_x_pels_per_meter = data_ptr->info_header->bi_x_pels_per_meter;
	new_ptr->info_header->bi_y_pels_per_meter = data_ptr->info_header->bi_y_pels_per_meter;

}

/* write the file
 * save_path(in): save path
 * data_ptr(in): image information and data
 */
void save(const char* save_path, BmpImage* data_ptr)
{
	FILE* file_ptr = fopen(save_path, "wb");
	int32_t offset_address = 0;
	uint8_t* offset_data;
	int32_t dst_height = data_ptr->info_header->bi_height;
	int32_t dst_width = data_ptr->info_header->bi_width;
	int32_t symmetry_width = (dst_width * 8 + 31) >> 5 << 2;
	int64_t equal_width, equal_offset_address;

	int32_t i;
	if (!file_ptr)
	{
		printf("Can't create the bmp file!\n");
		exit(-12);
	}
	else if (8 == data_ptr->info_header->bi_bit_count)
	{
		/* 4-byte symmetry
		 * 32bits -> 4Bytes
		 * int32_t(bi_width[Byte numbers]*8 +31[32-1])/32*4
		 * ((bi_width*8+31)>>5)*4
		 * (bi_width*8+31)>>5 <<2
		 */
		data_ptr->info_header->bi_size_image = symmetry_width * dst_height;
		data_ptr->file_header->bf_size = data_ptr->file_header->bf_off_bits + data_ptr->info_header->bi_size_image;
		/* write header*/
		fwrite(data_ptr->file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
		fwrite(data_ptr->info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
		/* write color palette */
		fwrite(&data_ptr->ColorPalette, sizeof(BITMAPColorPalette), 256, file_ptr);
		offset_address = 4 - (dst_width % 4); /* symmetry */
		offset_data = (uint8_t*)malloc(sizeof(uint8_t) * offset_address);
		for (i = offset_address - 1; i >= 0; i--)
			*(offset_data + i) = 0;
		for (i = dst_height - 1; i >= 0; i--)
		{

			fwrite((data_ptr->DATA + i * dst_width), sizeof(uint8_t), dst_width, file_ptr);
			if (offset_address != 4)
				fwrite(offset_data, sizeof(uint8_t), offset_address, file_ptr);
			else
			{
				;
			}
		}
		free(offset_data);
	}
	else if (24 == data_ptr->info_header->bi_bit_count)
	{
		data_ptr->info_header->bi_size_image = symmetry_width * dst_height * 3;
		data_ptr->file_header->bf_size = data_ptr->file_header->bf_off_bits + data_ptr->info_header->bi_size_image;
		/* write header*/
		fwrite(data_ptr->file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
		fwrite(data_ptr->info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
		offset_address = 4 - (dst_width % 4); /* symmetry */
		equal_width = dst_width * 3;
		equal_offset_address = offset_address * 3;
		offset_data = (uint8_t*)malloc(sizeof(uint8_t) * equal_offset_address);
		for (i = dst_height - 1; i >= 0; i--)
		{

			fwrite((data_ptr->DATA + i * equal_width), sizeof(uint8_t),
				equal_width, file_ptr);
			if (offset_address != 4)
				fwrite(offset_data, sizeof(uint8_t), equal_offset_address, file_ptr);
			else
			{
				;
			}
		}
		free(offset_data);
	}
	else
	{
		;
	}
	fclose(file_ptr);
}

/* show the information to screen
 * file_header_ptr(inout): file header pointer
 * info_header_ptr(inout): info header pointer
 */
const void showInfo(const PBITMAPFILEHEADER file_header, const PBITMAPINFOHEADER info_header)
{
	/* 显示BMP文件头信息 */
	printf("SHOWING INFOMATION OF BMP IMAGE\n");
	printf("#####################################################################\
\nBit Map File Header:\n");
	printf("\tBMP BM:%d\n", file_header->bf_type);
	printf("\tBMP bitmap file size(Kb)：%f\n", (file_header->bf_size / 1024.0));
	printf("\t0 reserved Byte(Must be 0)：%d\n", file_header->bf_reserved_0);
	printf("\t1 reserved Byte(Must be 0)：%d\n", file_header->bf_reserved_1);
	printf("\tThe actual offset Bytes of bitmap data: %d\n", file_header->bf_off_bits);
	/*
	显示BMP信息头信息
	*/
	printf("#####################################################################\
\nBit Map Info Header:\n");
	printf("\tThe size of info header:%d\n", info_header->bi_size);
	printf("\tBitmap's width:%d\n", info_header->bi_width);
	printf("\tBitmap's height:%d\n", info_header->bi_height);
	printf("\tThe number of planes of the image(default 1):%d\n", info_header->bi_planes);
	printf("\tNumber of bits per pixel:%d\n", info_header->bi_bit_count);
	printf("\tCompress mode:%d\n", info_header->bi_compression);
	printf("\tSize of picture:%d\n", info_header->bi_size_image);
	printf("\tHorizontal resolution:%d\n", info_header->bi_x_pels_per_meter);
	printf("\tVertical resolution:%d\n", info_header->bi_y_pels_per_meter);
	printf("\tNumber of colors used:%d\n", info_header->bi_clr_used);
	printf("\tNumber of Important colors used:%d\n", info_header->bi_clr_important);
}

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
			*(hsi_ptr->S_component + i * width + j) = 1 - 3.0 * min(min(R, B), G)/ (R + G + B + 1e-15);
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
	double_t length_d,min_idx;
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
	*(dimension+1) = dst_width;
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
	fft_out_ptr = fft2d(complex_data_ptr, dst_height , dst_width);
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
			*(fft_out_ptr->real+ i * out_ptr->info_header->bi_width + j) = log(sqrt(
				pow(*(complex_data_ptr->real + i * out_ptr->info_header->bi_width + j), 2) +
				pow(*(complex_data_ptr->imag + i * out_ptr->info_header->bi_width + j), 2)
			));
			length_d = length_d < *(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) ?
				*(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) : length_d;
			min_idx = min_idx> *(fft_out_ptr->real + i * out_ptr->info_header->bi_width + j) ?
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
	int32_t begin_idx,middle_idx;
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
			middle_idx = *(dimension_length) >>1;
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
				for (i = middle_idx ; i >= 1; i--)
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
				for (i = *(dimension_length)- 1; i >= 0; i--)
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
		for (i = dst_height-1; i >= 0; i--)
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
Complex* fft1d(const Complex* data_ptr, const int32_t ptr_length,const Complex* wds_ptr)
{
	int32_t ptr_power_idx, ptr_power_half;
	double_t length_d = ptr_length;
	Complex* out_ptr = NULL;
	int32_t i, j, k, l, idx0, idx1, idx_pow;
	double_t treal, timag,ureal,uimag;
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

/* DCT Discrete cosine transform. 
 * data_ptr(in): image information
 */
BmpImage* dct(const BmpImage* data_ptr)
{
	BmpImage* out_ptr = copyBmpImagePtr(data_ptr, 255);
	Complex* dct_ptr = NULL;
	Complex* complex_data_ptr = (Complex*)malloc(sizeof(Complex));
	int32_t idx_pow, src_height, src_width, dst_height, dst_width;
	double_t length_d,min_idx;
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
				*(tmp_ptr->real + i) = *(out_ptr->real + i * dst_width + j );
				*(tmp_ptr->imag + i) = *(out_ptr->imag + i * dst_width + j );
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
			for (i = ptr_power_idx - 1; i >= 0; i-=2)
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
	double_t mul_2n = 0.5/(ptr_power_idx);
	double_t sqrt_2n_inverse = sqrt(mul_2n);
	double_t power_real = is_real == 1 ? 2* sqrt_2n_inverse : sqrt_2n_inverse;
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

BmpImage* dwt(const BmpImage* data_ptr)
{
}

/* convolution when height or width==1 is 1D conv   stride = 1
 * data_ptr(in): data
 * conv_kernel(in): conv kernel
 * kernel_size(in): kernel_size
 * length(in): data length
 * dimension: 1d 2d 3d
 * padding_mode: 0 -> padding zero 1 -> padding the nearest
 */
Complex* Conv(const Complex* data_ptr, const Complex* conv_kernel, const uint8_t* kernel_size, const int32_t* length, const uint8_t dimension, const uint8_t* padding,const uint8_t padding_mode)
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
			if (*(length + 1) < 1 || *(kernel_size + i) < 1)
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
			printf("Dimension %d: data_length(%d) kernel_size(%d)\n",i+1, *(length + i), *(kernel_size + i));
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
			dst_width = pixel_num + *(length+1);
			pixel_num = (int64_t)dst_height * dst_width;
			// printf("\nPadding from (%d,%d)->(%d,%d)\n", *(length), *(length + 1), dst_height, dst_width);
			padding_ptr = (Complex*)malloc(sizeof(Complex));
			padding_ptr->real = (double_t*)malloc(sizeof(double_t) * pixel_num);
			padding_ptr->imag = (double_t*)malloc(sizeof(double_t) * pixel_num);
			/* padding */
			for (i = dst_height - 1; i >= 0; i--)
			{
				for (j = dst_width - 1; j >= 0; j--)
				{
					is_satisfied = i >= dst_height - *(padding) || i < *(padding) ||
						j >= dst_width - *(padding + 1) || j < *(padding + 1) ? 0 : 1;
					*(padding_ptr->real + i * dst_width + j) = is_satisfied == 1 ?
						*(data_ptr->real + (i - *(padding)) * *(length + 1) + j - *(padding + 1)) : 0;
					*(padding_ptr->imag + i * dst_width + j) = is_satisfied == 1 ?
						*(data_ptr->imag + (i - *(padding)) * *(length + 1) + j - *(padding + 1)) : 0;
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
				/*printf("\n\n\n");
				for (i =  0; i < dst_height; i++)
				{
					for (j = 0 ; j < dst_width; j++)
						printf("%f,%f\t", *(padding_ptr->real + i * dst_width + j), *(padding_ptr->imag + i * dst_width + j));
					printf("\n");
				}
				printf("\n\n\n");*/
				/* for each col */
				for (j = *(padding+1) - 1; j >= 0; j--)
				{
					for (i = dst_height - 1; i >= 0; i--)
					{
						*(padding_ptr->real + i * dst_width + j) = *(padding_ptr->real + i * dst_width + j+1);
						*(padding_ptr->imag + i * dst_width + j) = *(padding_ptr->imag + i * dst_width + j+1);
						*(padding_ptr->real + i * dst_width + dst_width - 1 - j) = *(padding_ptr->real + i * dst_width + dst_width - 2 - j);
						*(padding_ptr->imag + i * dst_width + dst_width - 1 - j) = *(padding_ptr->imag + i * dst_width + dst_width - 2 - j);
					}
				}
			}
			else
			{
				;
			}
			printf("\n\n\nPadding Data:\n");
			for (i = 0; i < dst_height; i++)
			{
				for (j = 0; j < dst_width; j++)
					printf("%f,%f\t", *(padding_ptr->real + i * dst_width + j), *(padding_ptr->imag + i * dst_width + j));
				printf("\n");
			}
			printf("\n\n\n");
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
			printf("Out (%d,%d)->(%d,%d)\n", *(length), *(length + 1), dst_height, dst_width);
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