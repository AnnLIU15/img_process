// bmpv3.c : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <stdio.h>
#include"func.h"
int main()
{
    /* HW1 */
    /*{
    const char* hw1_in = "./data/lena512.bmp";
    const char* hw1_out_nearest = "./output/lena512_nearest.bmp";
    const char* hw1_out_bilinear = "./output/lena512_bilinear.bmp";
    const char* hw1_out_bicubic = "./output/lena512_bicubic.bmp";
    BmpImage* reader = read(hw1_in);
    BmpImage* nearest_interpolation = interpolation(reader, 0, 1.5);
    BmpImage* bilinear_interpolation = interpolation(reader, 1, 1.5);
    BmpImage* bicubic_interpolation = interpolation(reader, 2, 1.5);    

    save(hw1_out_nearest,nearest_interpolation);
    save(hw1_out_bilinear,bilinear_interpolation);
    save(hw1_out_bicubic,bicubic_interpolation );
    free_ptr(nearest_interpolation);
    free_ptr(bilinear_interpolation);
    free_ptr(bicubic_interpolation);
    free_ptr(reader);
    }*/

    /* HW2 */
    /*{
    const char* hw2_in = "./data/data2.bmp";
    const char* hw2_out = "./output/data2_processed.bmp";
    BmpImage* reader = read(hw2_in);
    BmpImage* histogram_interpolation = histogramEqualization(reader);
    save(hw2_out, histogram_interpolation);
    free_ptr(histogram_interpolation);
    free_ptr(reader);
    }*/

    /* HW3 */
    /*{
    const char* hw3_in = "./data/girl256-pepper-salt.bmp";
    const char* hw3_out_1_3 = "./output/girl256-pepper-salt_processed(median-kernel=3).bmp";
    const char* hw3_out_1_5 = "./output/girl256-pepper-salt_processed(median-kernel=5).bmp";
    const char* hw3_out_1_7 = "./output/girl256-pepper-salt_processed(median-kernel=7).bmp";
    const char* hw3_out_2_1 = "./output/girl256-pepper-salt_processed(Gaussian-kernel=3-sigma=0.5).bmp";
    const char* hw3_out_2_2 = "./output/girl256-pepper-salt_processed(Gaussian-kernel=3-sigma=0.8).bmp";
    const char* hw3_out_2_3 = "./output/girl256-pepper-salt_processed(Gaussian-kernel=5-sigma=0.8).bmp";
    const char* hw3_out_2_4 = "./output/girl256-pepper-salt_processed(Gaussian-kernel=5-sigma=1.3).bmp";
    const char* hw3_out_2_5 = "./output/girl256-pepper-salt_processed(Gaussian-kernel=10-sigma=1.3).bmp";
    const char* hw3_out_2_6 = "./output/girl256-pepper-salt_processed(Gaussian-kernel=10-sigma=2).bmp";
    BmpImage* reader = read(hw3_in);
    
    BmpImage* median = medianFilter(reader, 3);
    save(hw3_out_1_3, median);
    free_ptr(median);
    median = medianFilter(reader, 5);
    save(hw3_out_1_5, median);
    free_ptr(median);
    median = medianFilter(reader, 7);
    save(hw3_out_1_7, median);
    free_ptr(median);
    
    BmpImage* Gaussian = GaussianLowPassFilter(reader, 3, 0.5);
    save(hw3_out_2_1, Gaussian);
    free_ptr(Gaussian);
    Gaussian = GaussianLowPassFilter(reader,3,0.8);
    save(hw3_out_2_2, Gaussian);
    free_ptr(Gaussian);
    Gaussian = GaussianLowPassFilter(reader, 5, 0.8);
    save(hw3_out_2_3, Gaussian);
    free_ptr(Gaussian);
    Gaussian = GaussianLowPassFilter(reader, 5, 1.3);
    save(hw3_out_2_4, Gaussian);
    free_ptr(Gaussian);
    Gaussian = GaussianLowPassFilter(reader, 10, 1.3);
    save(hw3_out_2_5, Gaussian);
    free_ptr(Gaussian);
    Gaussian = GaussianLowPassFilter(reader, 10, 2);
    save(hw3_out_2_6, Gaussian);
    free_ptr(Gaussian);
    free_ptr(reader);
    }*/

    /* HW4 */
    /*{
    const char* hw4_in = "./data/data4.bmp";
    const char* hw4_out = "./output/data4_processed.bmp";
    BmpImage* reader = read(hw4_in);
    BmpImage* histogram_interpolation = histogramEqualization(reader);
    save(hw4_out, histogram_interpolation);
    free_ptr(histogram_interpolation);
    free_ptr(reader); 
    }*/

    /* HW5 */
    {
    /*const char* hw5_in = "./data/data2.bmp";
     const char* hw5_out_fft = "./output/data2_fft.bmp";
     const char* hw5_out_dct = "./output/data2_dct.bmp";
     const char* hw5_out_dwt = "./output/data2_dwt.bmp";
     */

    const char* hw5_in = "./data/lena512.bmp";
    const char* hw5_out_fft = "./output/lena512_fft.bmp";
    const char* hw5_out_dct = "./output/lena512_dct.bmp";
    const char* hw5_out_dwt = "./output/lena512_dwt.bmp";
    BmpImage* reader = read(hw5_in);
    BmpImage* fft_pointer = fft(reader);
    BmpImage* dct_pointer = dct(reader);
    save(hw5_out_fft, fft_pointer);
    save(hw5_out_dct, dct_pointer);
    free_ptr(fft_pointer);
    free_ptr(dct_pointer);
    free_ptr(reader);
    }
   // int32_t width = 4;
   // int32_t height = 6;
   // int32_t length = height* width;
   // int32_t is_real = 1;
   // int32_t idx_pow, ptr_power_idx;
   // int32_t fft_length;
   // double_t length_d = length;
   // idx_pow = 0;
   // for (; length_d > 1; )
   // {
   //     length_d *= 0.5;
   //     idx_pow++;
   // }
   // ptr_power_idx = pow(2, idx_pow);
   // Complex* data_ptr = (Complex*)malloc(sizeof(Complex));
   // data_ptr->real = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
   // data_ptr->imag = (double_t*)malloc(sizeof(double_t) * ptr_power_idx);
   // printf("Origin Data:\n");
   // for (int i = 0; i < ptr_power_idx; i++)
   // {
   //     if (i % width == 0 && i != 0)
   //         printf("\n");
   //     *(data_ptr->real + i) = i >= length?0:i;
   //     *(data_ptr->imag + i) = (is_real == 1|| i >= length) ? 0 : -i;
   //     printf("%f,%f\t", *(data_ptr->real + i), *(data_ptr->imag + i));
   //     
   // }
   // printf("\n\n");
   // Complex* dct_ptr = dct2d(data_ptr, 8, 4, is_real);
   // Complex* fft_ptr = fft2d(data_ptr, 8, 4);
   ///* printf("DCT kernel:\n");
   // for (size_t i = 0; i < ptr_power_idx; i++)
   // {
   //     
   //     printf("%f,%f\n", *(dct_wds_ptr->real + i), *(dct_wds_ptr->imag + i));
   // }
   // printf("FFT kernel:\n");
   // for (size_t i = 0; i < fft_length>>1; i++)
   // {
   //     printf("%f,%f\n", *(fft_wds_ptr->real + i), *(fft_wds_ptr->imag + i));
   // }*/
   // printf("\n\nOut:\n");
   // for (int i = 0; i < ptr_power_idx; i++)
   // {
   //     if (i % width == 0 && i != 0)
   //         printf(";\n");
   //     printf("%f+1j*(%f),", *(dct_ptr->real + i), *(dct_ptr->imag + i));
   //     
   // }
   // printf("\n\nFFT Out:\n");
   // for (int i = 0; i < ptr_power_idx; i++)
   // {
   //     if (i % width == 0 && i != 0)
   //         printf(";\n");
   //     printf("%f+1j*(%f),", *(fft_ptr->real + i), *(fft_ptr->imag + i));

   // }
}

