/*
 * bmpInfo.h
 *
 *  Created on: 2021-10-13
 *      Author: Ann
 */

#ifndef BMPINFO_H_
#define BMPINFO_H_

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
// unsigned long == unsigned int
typedef unsigned long long uint64_t;
typedef float              float_t;
typedef double             double_t;
/*
    位图信息头
    bi_size: 信息头大小 14-17Byte
    bi_width: 图宽 18-21Byte
    bi_height: 图高 22-25Byte
    bi_planes: bmp位平面数，只有一个平面(1) 26-27Byte
    bi_bit_count: 每个像素占的bits(1,4,8,24,32,) 28-29Byte
    bi_compression: 像素数据压缩类型,0==不压缩 30-33Byte
    bi_size_image: 像素数据所占大小, 等于bf_off_bits 34-37Byte
    bi_x_pels_per_meter: 水平分辨率，像素/米 一般为0 38-41Byte
    bi_y_pels_per_meter: 垂直分辨率，像素/米 一般为0 42-45Byte
    bi_clr_used: 使用的彩色表中的颜色索引数 0==使用所有调色板项 46-49Byte
    bi_clr_important 重要影响的颜色索引的数目 0==都重要 50-53Byte
*/
typedef struct tagBITMAPINFOHEADER
{
    uint32_t bi_size;
    int32_t  bi_width;
    int32_t  bi_height;
    uint16_t bi_planes;
    uint16_t bi_bit_count;
    uint32_t  bi_compression;
    uint32_t  bi_size_image;
    int32_t  bi_x_pels_per_meter;
    int32_t  bi_y_pels_per_meter;
    uint32_t  bi_clr_used;
    uint32_t  bi_clr_important;
} BITMAPINFOHEADER, * PBITMAPINFOHEADER;
/*
    位图文件头
    bf_type:        文件类型 必须是BM字符串 -> 0x4d42(16) 19778(10)
    bf_size:        文件大小 以字节为单位 2-5Byte -> uint32_t
    bf_reserved_0:  保留字0 必须设置为0 6-7Byte -> uint16_t
    bf_reserved_1:  保留字1 必须设置为0 8-9Byte -> uint16_t
    bf_off_bits:    文件头到像素数据的偏移字节数目 10-13Byte -> uint32_t
*/
#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER
{
    uint16_t bf_type;
    uint32_t bf_size;
    uint16_t bf_reserved_0;
    uint16_t bf_reserved_1;
    uint32_t bf_off_bits;
} BITMAPFILEHEADER, * PBITMAPFILEHEADER;
#pragma pack(pop)

typedef struct tagColorPalette
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
}BITMAPColorPalette;

typedef struct tagBmpImage
{
    uint8_t* DATA;	                         // 像素数组
    BITMAPColorPalette ColorPalette[256];    // 调色板
    PBITMAPFILEHEADER file_header;           // 文件头
    PBITMAPINFOHEADER info_header;           // 信息头
}BmpImage;

// RGBTRIPLE DATA*HEIGHT*WIDTH*3

typedef struct tagHSIInfo
{
    double_t* H_component;                          // H_component
    double_t* S_component;                          // S_component
    double_t* I_component;	                      // I_component
}HSIInfo;

typedef struct tagComplex
{
    double_t* real;
    double_t* imag;
}Complex;


#endif /* BMPINFO_H_ */