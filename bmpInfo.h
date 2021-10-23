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
    λͼ��Ϣͷ
    bi_size: ��Ϣͷ��С 14-17Byte
    bi_width: ͼ�� 18-21Byte
    bi_height: ͼ�� 22-25Byte
    bi_planes: bmpλƽ������ֻ��һ��ƽ��(1) 26-27Byte
    bi_bit_count: ÿ������ռ��bits(1,4,8,24,32,) 28-29Byte
    bi_compression: ��������ѹ������,0==��ѹ�� 30-33Byte
    bi_size_image: ����������ռ��С, ����bf_off_bits 34-37Byte
    bi_x_pels_per_meter: ˮƽ�ֱ��ʣ�����/�� һ��Ϊ0 38-41Byte
    bi_y_pels_per_meter: ��ֱ�ֱ��ʣ�����/�� һ��Ϊ0 42-45Byte
    bi_clr_used: ʹ�õĲ�ɫ���е���ɫ������ 0==ʹ�����е�ɫ���� 46-49Byte
    bi_clr_important ��ҪӰ�����ɫ��������Ŀ 0==����Ҫ 50-53Byte
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
    λͼ�ļ�ͷ
    bf_type:        �ļ����� ������BM�ַ��� -> 0x4d42(16) 19778(10)
    bf_size:        �ļ���С ���ֽ�Ϊ��λ 2-5Byte -> uint32_t
    bf_reserved_0:  ������0 ��������Ϊ0 6-7Byte -> uint16_t
    bf_reserved_1:  ������1 ��������Ϊ0 8-9Byte -> uint16_t
    bf_off_bits:    �ļ�ͷ���������ݵ�ƫ���ֽ���Ŀ 10-13Byte -> uint32_t
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
    uint8_t* DATA;	                         // ��������
    BITMAPColorPalette ColorPalette[256];    // ��ɫ��
    PBITMAPFILEHEADER file_header;           // �ļ�ͷ
    PBITMAPINFOHEADER info_header;           // ��Ϣͷ
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