#ifndef BMP_FUNC_H_
#define BMP_FUNC_H_
#include"bmpInfo.h"

BmpImage* read(const char* img_path,const uint8_t is_show);

void free_ptr(BmpImage* data_ptr);

BmpImage* copyBmpImagePtr(const BmpImage* data_ptr, const uint8_t copy_data);

void copyBmpImageInfo(const BmpImage* data_ptr, BmpImage* new_ptr);

void save(const char* save_path, BmpImage* data_ptr);

const void showInfo(const PBITMAPFILEHEADER file_header, const PBITMAPINFOHEADER info_header);

#endif