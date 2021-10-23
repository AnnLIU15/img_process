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
	int64_t equal_width,equal_offset_address;
	uint8_t* offset_data;
	FILE* file_ptr = fopen(img_path, "rb");
	printf("Reading path: %s\n", img_path);
	fread(data_ptr->file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
	fread(data_ptr->info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
	if ((!file_ptr)|| data_ptr->file_header->bf_type != 19778)
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
				fread((uint8_t*)(data_ptr->DATA +  i * src_width), sizeof(uint8_t), src_width, file_ptr);
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
				fread((uint8_t*)(data_ptr->DATA +  i * equal_width),
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
	uint8_t* offset_data ;
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
	else if(8 == data_ptr->info_header->bi_bit_count)
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
		for (i = dst_height-1; i >=0 ; i--)
		{
			
			fwrite((data_ptr->DATA +  i * dst_width), sizeof(uint8_t), dst_width, file_ptr);
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
	BmpImage* dst_data_ptr = copyBmpImagePtr(data_ptr,255);
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
			
			for (i = dst_height-1; i>=0 ; i--)
				for (j = dst_width-1; j >=0 ; j--)
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
						c * v * (1 - u) + u * v * d+0.001);
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
		data_ptr->info_header->bi_width,dst_data_ptr->info_header->bi_height, dst_data_ptr->info_header->bi_width);

	return dst_data_ptr;
}

/* HW2 */

/* histogram equalization
 * data_ptr(in): origin image
 */
BmpImage* histogramEqualization(const BmpImage* data_ptr)
{
	BmpImage* dst_data_ptr = copyBmpImagePtr(data_ptr,0);
	int32_t width = dst_data_ptr->info_header->bi_width;
	int32_t height = dst_data_ptr->info_header->bi_height;
	int32_t pixel_number = width * height;
	double_t pixel_number_inverse = 1.0 / (double_t)pixel_number;
	double_t gray_prob_equal[256];
	double_t gray_gray_distribution[256];
	int32_t i, j;
	HSIInfo* hsi_ptr = NULL;
	uint8_t* tmp_data = (uint8_t*)malloc(sizeof(uint8_t) * width * height );
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
				*(tmp_data + i * width + j) = round(*(hsi_ptr->I_component + i * width + j) * 255.0 );
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
		gray_gray_distribution[j] = gray_gray_distribution[j - 1]+ gray_prob_equal[j]; 
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
	else if(24 == data_ptr->info_header->bi_bit_count) // 3 channel
	{
		for (i = height - 1; i >= 0; i--)
			for (j = width - 1; j >= 0; j--)
				*(hsi_ptr->I_component + i * width + j)
				= ((uint8_t)gray_prob_equal[*(tmp_data + i * width + j)])/255.0;
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
	for (i = length - 1; i > 0 ; i--)
	{
		processed = 0;
		largest_position = i;
		for (j = i - 1; j >=0; j--)
		{
			if (*(data_array + largest_position) < *(data_array + j))
			{
				largest_position = j;
				processed = 1;
			}
		}
		if (processed==1)
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
	BmpImage* dst_data_ptr = copyBmpImagePtr(data_ptr,0);
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
		
		for ( i = dst_height-1; i >=0 ; i--)
			for ( j = dst_width-1; j >=0 ; j--)
				*(padding_matrix + i * dst_width + j) = 0;
		for (i = src_height-1; i >= 0; i--)
			for (j = src_width-1; j >= 0; j--)
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
					*(dst_data_ptr->DATA + i * src_width + j) = (uint8_t)(sum_of_GaussianTemplate+0.01);
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
	double_t B, G, R,theta;
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
			B = (double_t)*(data_ptr->DATA + i * equal_width + j * 3) / 255.0;
			G = (double_t) *(data_ptr->DATA + i * equal_width + j * 3 + 1) / 255.0;
			R = (double_t) *(data_ptr->DATA + i * equal_width + j * 3 + 2) / 255.0;
			
			/* theta = ((R-G)+(R-B))/2
			 * theta = theta/sqrt((R-G)^2+(R-B)(G-B))
			 * theta = arccos(theta)
			 */
			theta = ((R - G) + (R - B)) * 0.5;
			theta = theta / (sqrt(pow((R - G), 2) + (R - B) * (G - B)) + 1e-15);
			theta = acos(theta);
			
			*(hsi_ptr->S_component + i * width + j) = 1 - 3.0 * (R <= B ? ((R <= G) ? R : G) : ((B <= G) ? B : G))
														/ (R + G + B + 1e-15);
			*(hsi_ptr->H_component + i * width + j) = abs(*(hsi_ptr->S_component + i * width + j)) < 1e-10 ? 0 : ((G >= B) 
														? theta : 2 * M_PI - theta);
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
void hsi2bgr(BmpImage* data_ptr,const HSIInfo* hsi_ptr)
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
			else if (H_component <= 2.0 * angle&& H_component > angle)
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

BmpImage* fft(const BmpImage* data_ptr) 
{
	int32_t i, j, m, n;
	BmpImage* freq_ptr = copyBmpImagePtr(data_ptr, 0);
	int32_t width = data_ptr->info_header->bi_width;
	int32_t height = data_ptr->info_header->bi_height;
	// Normalization constant
	int32_t idx = height * width;

	double_t norm_constant = 1.0 / idx;
	Complex* complex_data_ptr = (Complex*)malloc(sizeof(Complex));
	complex_data_ptr->real = (double_t*)malloc(sizeof(double_t) * idx);
	complex_data_ptr->imag = (double_t*)malloc(sizeof(double_t) * idx);
	Complex* complex_data_ptr_out = (Complex*)malloc(sizeof(Complex));
	complex_data_ptr_out->real = (double_t*)malloc(sizeof(double_t) * idx);
	complex_data_ptr_out->imag = (double_t*)malloc(sizeof(double_t) * idx);
	double_t* Re_p = (double_t*)malloc(sizeof(double_t) * height);
	double_t* Im_p = (double_t*)malloc(sizeof(double_t) * height);
	double_t* Re_f = (double_t*)malloc(sizeof(double_t) * height);
	double_t* Im_f = (double_t*)malloc(sizeof(double_t) * height);
	// Fast Fourier Transform 2D - Computation
	printf("height:%d,width:%d\n", height, width);
	for (i = height - 1; i >= 1; i--)
		fft_real(
			(data_ptr->DATA + i * width),
			(complex_data_ptr->real + i * width),
			(complex_data_ptr->imag + i * width),
			 width, 1);

	for ( j = width-1; j >=1; j--) 
	{
		for (i = height - 1; i >= 1; i--) 
		{
			*(Re_p+i) = *(complex_data_ptr->real + i * width + j);
			*(Im_p+i) = *(complex_data_ptr->imag + i * width + j);
		}

		fft_complex(Re_p, Im_p, Re_f, Im_f, height, 1);

		for (i = height - 1; i >= 1; i--) 
		{
			*(complex_data_ptr_out->real + i * width + j) = *(Re_p + i) * norm_constant;
			*(complex_data_ptr_out->imag + i * width + j) = *(Im_p + i) * norm_constant;
		}
	}
	for (i = height - 1; i >= 0; i--)
		for (j = width - 1; j >= 0; j--)
		{
			//printf("row:%d,col:%d\n", i, j);
			*(freq_ptr->DATA + i * width + j) = (uint8_t)sqrt(
				pow(*(complex_data_ptr_out->real + i * width + j), 2) +
				pow(*(complex_data_ptr_out->imag + i * width + j), 2));
		}
	free(Re_p); free(Im_p); free(Re_f); free(Im_f);
	free_complex(complex_data_ptr);
	free_complex(complex_data_ptr_out);
	return freq_ptr;
}

BmpImage* dct(const BmpImage* data_ptr)
{

}

BmpImage* dwt(const BmpImage* data_ptr)
{
}

void fft_real(double_t* Signal, double_t* Re_F, double_t* Im_F, int32_t N, int32_t t)
{
	int32_t half = N / 2;
	int32_t i;
	double_t r1, r2, i1, i2, tmp;
	if (1 == N)
	{
		*(Re_F) = *(Signal);
		*(Im_F) = 0.0;
	}
	else
	{
		fft_real(Signal, Re_F, Im_F, half, 2 * t);
		fft_real(Signal + t, Re_F + half, Im_F + half, half, 2 * t);
		for (i = half - 1; i >= 1; i--)
		{
			tmp = -2.0 * M_PI * i / (double)(N);
			r1 = cos(tmp);
			i1 = sin(tmp);
			r2 = *(Re_F + i + half) * r1 - *(Im_F + i + half) * i1;
			i2 = *(Im_F + i + half) * r1 + *(Re_F + i + half) * i1;
			/*
			double r1 = Re_F[k];
			double i1 = Im_F[k];
			double r2 = Re_F[k + half];
			double i2 = Im_F[k + half];
			double alpha = - 2.0 * M_PI * (double)(k) / (double)(N);
			double r3 = cos(alpha);
			double i3 = sin(alpha);
			double r4 = r2 * r3 - i2 * i3;
			double i4 = r3 * i2 + r2 * i3;
			Re_F[k + half] = r1 - r4;
			Im_F[k + half] = i1 - i4;
			Re_F[k] = r1 + r4;
			Im_F[k] = i1 + i4;
			*/
			*(Re_F + i + half) = *(Re_F + i) - r2;
			*(Im_F + i + half) = *(Im_F + i) - i2;
			*(Re_F + i) +=  r2;
			*(Im_F + i) +=  i2;
		}
	}
}

void fft_complex(double_t* Re_Signal, double_t* Im_Signal, double_t* Re_F, double_t* Im_F, int32_t N, int32_t t) {
	int32_t half = N / 2;
	int32_t i;
	double_t r1, r2, i1, i2, tmp;
	if (1 == N) {
		*(Re_F) = *(Re_Signal);
		*(Im_F) = *(Im_Signal); 
	}
	else
	{
		fft_complex(Re_Signal, Im_Signal, Re_F, Im_F, half, 2 * t);
		fft_complex(Re_Signal + t, Im_Signal + t, Re_F + half, Im_F + half, half, 2 * t);
		for (i = half-1; i>= 1; i--) 
		{
			tmp = -2.0 * M_PI * i / (double)(N);
			r1 = cos(tmp);
			i1 = sin(tmp);
			r2 = *(Re_F + i + half) * r1 - *(Im_F + i + half) * i1;
			i2 = *(Im_F + i + half) * r1 + *(Re_F + i + half) * i1;
			*(Re_F + i + half) = *(Re_F + i) - r2;
			*(Im_F + i + half) = *(Im_F + i) - i2;
			*(Re_F + i) += r2;
			*(Im_F + i) += i2;
		}
	}
	
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


