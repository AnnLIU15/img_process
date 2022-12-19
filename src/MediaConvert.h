/**
* 	Copyright (C) 2015-2021, Liang Fan
    All rights reverved.
*/

/** \file		MediaConvert.h
*   \brief		Header file of media convert functions.
*/

#ifndef __MEDIACONVERT_H__
#define __MEDIACONVERT_H__

#include <stdint.h>
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t yuv420_to_rgb32(uint8_t *out, uint8_t *in, int32_t width, int32_t height);

#ifdef __cplusplus
}
#endif

#endif /* __MEDIACONVERT_H__ */
