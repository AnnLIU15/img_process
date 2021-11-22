#pragma warning(disable:4996)
#include "../include/bmp_func.h"
/* base function */

/* read BMP data
 * img_path(in): file path
 */
BmpImage* read(const char* img_path,const uint8_t is_show)
{
	BmpImage* data_ptr = (BmpImage*)malloc(sizeof(BmpImage));
	data_ptr->file_header = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
	data_ptr->info_header = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	int32_t i, j;
	int32_t src_height, src_width, offset_address;
	int64_t equal_width, equal_offset_address;
	uint8_t* offset_data;
	FILE* file_ptr = fopen(img_path, "rb");
	printf("Reading path: %s\n", img_path);
	fread(data_ptr->file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
	fread(data_ptr->info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
	if(0!=is_show)
	{
		showInfo(data_ptr->file_header, data_ptr->info_header);
		//printf("\n\n\ndata_ptr->info_header->bi_bit_count:%d", data_ptr->info_header->bi_bit_count);
	}
	else { ; }
	if ((!file_ptr) || data_ptr->file_header->bf_type != 19778)
	{
		printf("It is not BMP file or haven't open file! Please check your input!\n");
	}
	else
	{
		src_height = data_ptr->info_header->bi_height;
		src_width = data_ptr->info_header->bi_width;
		offset_address = 4 - (src_width % 4); /* symmetry */
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
			equal_width = src_width * 3;
			equal_offset_address = offset_address * 3;
			data_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * src_height * equal_width); // bgr排布
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
		else if (1 == data_ptr->info_header->bi_bit_count) // 2 color pic
		{
			fread(data_ptr->ColorPalette, sizeof(uint8_t), 8, file_ptr);
			equal_width = src_width >> 3;
			data_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * src_height * equal_width);
			offset_data = (uint8_t*)malloc(sizeof(uint8_t)* offset_address);
			for (i = src_height - 1; i >= 0; i--)
			{
				for (j = equal_width - 1; j >= 0; j--)
				{
					fread((data_ptr->DATA + i * equal_width + j), sizeof(uint8_t), 1, file_ptr);
					//printf("%x,%x\t", one_bit_tmp, *(data_ptr->DATA + i * equal_width + j));
				}
				
				if (offset_address != 4)
				{
					fread(offset_data, sizeof(uint8_t), 1, file_ptr);
				}
				else
				{
					;
				}
				//printf("\n");
			}
			/*for (i = 0; i < 100; i++)
			{
				fread(&one_bit_tmp, sizeof(uint8_t), 1, file_ptr);
				printf("%x\t", one_bit_tmp);
			}*/
			
			free(offset_data);
			
			
		}
		else
		{
			printf("current version is not supported %d-bit BMP!\n", data_ptr->info_header->bi_bit_count);
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
	int32_t equal_width;
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
			equal_width = new_ptr->info_header->bi_width * 3;
			new_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * new_ptr->info_header->bi_height
				* equal_width);
			for (i = new_ptr->info_header->bi_height - 1; i >= 0; i--)
			{
				for (j = equal_width - 1; j >= 0; j--)
				{
					*(new_ptr->DATA + i * equal_width + j) = copy_data == 0 ? 0 :
						*(data_ptr->DATA + i * equal_width + j);
				}
			}
		}
		else
		{
			;
		}
	}
	else if (1 == data_ptr->info_header->bi_bit_count)
	{
		if (copy_data != 255)
		{
			for (int32_t i = 1; i >= 0; i--)
				new_ptr->ColorPalette[i] = data_ptr->ColorPalette[i];
			equal_width = new_ptr->info_header->bi_height >> 3;
			new_ptr->DATA = (uint8_t*)malloc(sizeof(uint8_t) * new_ptr->info_header->bi_width
				* equal_width);
			for (i = new_ptr->info_header->bi_height - 1; i >= 0; i--)
			{
				for (j = equal_width - 1; j >= 0; j--)
				{
					*(new_ptr->DATA + i * equal_width + j) = copy_data == 0 ? 0 :
						*(data_ptr->DATA + i * equal_width + j);
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
	int32_t i,j ;
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
	else if (1 == data_ptr->info_header->bi_bit_count) // 2 color pic
	{
		data_ptr->info_header->bi_size_image = (symmetry_width * dst_height)>>3;
		data_ptr->file_header->bf_size = data_ptr->file_header->bf_off_bits + data_ptr->info_header->bi_size_image;
		/* write header*/
		fwrite(data_ptr->file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
		fwrite(data_ptr->info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
		/* write color palette */
		fwrite(&data_ptr->ColorPalette, sizeof(uint8_t), 8, file_ptr);
		offset_address = 4 - (dst_width % 4); /* symmetry */
		offset_data = (uint8_t*)malloc(sizeof(uint8_t) * offset_address);
		equal_width = dst_width >> 3;
		for (i = offset_address - 1; i >= 0; i--)
			*(offset_data + i) = 0;
		for (i = dst_height - 1; i >= 0; i--)
		{
			for (j = equal_width - 1; j >= 0; j--)
			{
				fwrite((data_ptr->DATA + i * equal_width + j), sizeof(uint8_t), 1, file_ptr);
			}
			if (offset_address != 4)
				fwrite(offset_data, sizeof(uint8_t), offset_address, file_ptr);
			else
			{
				;
			}
		}
		free(offset_data);
	}
	else
	{
		printf("current version is not supported %d-bit BMP!\n", data_ptr->info_header->bi_bit_count);
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
	printf("\tBMP bitmap file size(Kb):%f\n", (file_header->bf_size / 1024.0));
	printf("\t0 reserved Byte(Must be 0):%d\n", file_header->bf_reserved_0);
	printf("\t1 reserved Byte(Must be 0):%d\n", file_header->bf_reserved_1);
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
