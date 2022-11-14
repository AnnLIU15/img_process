/**
*   Copyright (C) 2015-2021, Liang Fan
*   All rights reverved.
*/

/** \file		MediaConvert.c
*   \brief		Implementation of media convert functions, from YUV to RGB32.
*/

#include <stdint.h>
#include <string.h>
#include "MediaConvert.h"

//
#define Clip3(a, b, c) ((c < a) ? a : ((c > b) ? b : c))
#define Clip1(x)       Clip3(0, 255, x)


//
int32_t yuv420_to_rgb32(uint8_t *out, uint8_t *in, int32_t width, int32_t height)
{
    int32_t  r0, r1, r2, r3,
             g0, g1, g2, g3,
             b0, b1, b2, b3;
    int32_t  y0, y1, y2, y3,
             u0, u1, u2, u3,
             v0, v1, v2, v3;
    uint8_t *ipy, *ipu, *ipv, *op;

    /** initialize buffer pointers
    *   note: top line first.
    */
    op  = out;
    ipy = in;
    ipu = in + (width * height);
    ipv = in + (width * height * 5 / 4);

    //
    for(int32_t j=0; j<height; j+=2)
    {
        for(int32_t i=0; i<width; i+=2)
        {
            // get YUV data
            y0 = (int32_t)*ipy               - 16;
            y1 = (int32_t)*(ipy + 1)         - 16;
            y2 = (int32_t)*(ipy + width)     - 16;
            y3 = (int32_t)*(ipy + width + 1) - 16;

            u0 = (int32_t)*ipu               - 128;
            u1 = (int32_t)*ipu               - 128;
            u2 = (int32_t)*ipu               - 128;
            u3 = (int32_t)*ipu               - 128;

            v0 = (int32_t)*ipv               - 128;
            v1 = (int32_t)*ipv               - 128;
            v2 = (int32_t)*ipv               - 128;
            v3 = (int32_t)*ipv               - 128;
            
            /** convert YUV to RGB
            *   ITU-R BT.601
            *     R = Y           + 1.403*V
            *     G = Y - 0.344*U - 0.714*V
            *     B = Y + 1.770*U
            *   32bit calculation:
            *     R = (65536*Y            + 91947*V + 32768) >> 16
            *     G = (65536*Y -  22544*U - 46793*V + 32768) >> 16
            *     B = (65536*Y + 115999*U           + 32768) >> 16
            */
            r0 = (65536*y0            + 91947*v0 + 32768) >> 16;
            r1 = (65536*y1            + 91947*v1 + 32768) >> 16;
            r2 = (65536*y2            + 91947*v2 + 32768) >> 16;
            r3 = (65536*y3            + 91947*v3 + 32768) >> 16;

            g0 = (65536*y0 -  22544*u0 - 46793*v0 + 32768) >> 16;
            g1 = (65536*y1 -  22544*u1 - 46793*v1 + 32768) >> 16;
            g2 = (65536*y2 -  22544*u2 - 46793*v2 + 32768) >> 16;
            g3 = (65536*y3 -  22544*u3 - 46793*v3 + 32768) >> 16;

            b0 = (65536*y0 + 115999*u0           + 32768) >> 16;
            b1 = (65536*y1 + 115999*u1           + 32768) >> 16;
            b2 = (65536*y2 + 115999*u2           + 32768) >> 16;
            b3 = (65536*y3 + 115999*u3           + 32768) >> 16;
            
            /** RGB32
            *   unsigned char: B0, G0, R0, A0, B1, G1, R1, A1, ...
            *   note: top line first.
            */
            // first pixel line
            *op       = (uint8_t)Clip1(b0);
            *(op + 1) = (uint8_t)Clip1(g0);
            *(op + 2) = (uint8_t)Clip1(r0);
            *(op + 3) = (uint8_t)0;  // Alpha, don't care
            *(op + 4) = (uint8_t)Clip1(b1);
            *(op + 5) = (uint8_t)Clip1(g1);
            *(op + 6) = (uint8_t)Clip1(r1);
            *(op + 7) = (uint8_t)0;  // Alpha, don't care

            // second pixel line
            *(op + (width * 4))     = (uint8_t)Clip1(b2);
            *(op + (width * 4 + 1)) = (uint8_t)Clip1(g2);
            *(op + (width * 4 + 2)) = (uint8_t)Clip1(r2);
            *(op + (width * 4 + 3)) = (uint8_t)0; // Alpha, don't care
            *(op + (width * 4 + 4)) = (uint8_t)Clip1(b3);
            *(op + (width * 4 + 5)) = (uint8_t)Clip1(g3);
            *(op + (width * 4 + 6)) = (uint8_t)Clip1(r3);
            *(op + (width * 4 + 7)) = (uint8_t)0; // Alpha, don't care

            //
            op  += 8;
            ipy += 2;
            ipu++;
            ipv++;
        } // for i
        op  += (width * 4);
        ipy += width;
    } // for j

    return 0;
}
