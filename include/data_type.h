#ifndef DATA_TYPE_H_
#define DATA_TYPE_H_

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

#endif
