#ifndef __TRANSFORMICT_H__
#define __TRANSFORMICT_H__
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
namespace ict_dsp {

    #define clip4int8(val, min_val, max_val) ((val > max_val) ? max_val : ((min_val > val) ? min_val : val))

    /* Api */
    uint8_t transformIct(uint8_t* pSourceBuffer, const int32_t width, const int32_t height);
    uint8_t transformIct_8(uint8_t* pSourceBuffer, const int32_t width, const int32_t height);

    /* DATA = H * DATA *H' */
    void mat_mpy4x4(int32_t* data, const int32_t* H);
    void mat_mpy8x8(int32_t* data, const int32_t* H);
    /* H matrix */
    int32_t* get_H_kernel(bool is_t = 0);

    /* alpha matrix */
    float_t* get_alpha_kernel();

    /* output compressed data */
    void compressIct(int32_t* data, const int32_t* mat_H, const int32_t* mat_H_T, const float_t* mat_alpha);
    void compressIct8x8(int32_t* data, const int32_t* mat_H, const int32_t* mat_H_T, const float_t* mat_alpha);

}
#endif // __CTESTFILTER_H__