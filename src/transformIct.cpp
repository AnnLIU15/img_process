#include "transformIct.h"

namespace ict_dsp
{
    double_t* histogram(uint8_t* pSourceBuffer/* inout */,
        const int32_t width/* in */, const int32_t height/* in */)
    {
        int32_t pixel_number = width * height;
        double_t pixel_number_inverse = 1.0 / (double_t)pixel_number;
        double_t* gray_prob_equal = new double_t [256];
        int32_t i, j;
        /* initialize array */
        memset(gray_prob_equal, 0, sizeof(double_t) * 256);
        /* 统计概率pdf -> gray_prob p_r(r_k)=\frac{n_k}{MN}*/
        for (i = height - 1; i >= 0; i--)
            for (j = width - 1; j >= 0; j--)
                *(gray_prob_equal + *(pSourceBuffer + i * width + j)) += 1;
        for (i = 255; i >= 0; i--)
            gray_prob_equal[i] *= pixel_number_inverse;
        
        return gray_prob_equal;
    }

    double_t cal_relative_rho(double_t* ori, double_t* dst, const int32_t length)
    {
        double_t rho = 0;
        double_t mean_ori = 0, mean_dst = 0;    // each dim without
        int32_t i = length - 1;
        for (; i >= 0; i--)
        {
            mean_ori += *(ori + i); mean_dst += *(dst + i);
        }
        mean_ori /= length; mean_dst /= length;
        double_t R1 = 0, R2 = 0, R3 = 0;
        for (i = length - 1; i >= 0; i--)
        {
            R1 += (*(ori + i) - mean_ori) * (*(dst + i) - mean_dst);
            R2 += powf((*(ori + i) - mean_ori), 2);
            R3 += powf((*(dst + i) - mean_dst), 2);
        }
        rho = R1 / sqrt(R2 * R3);
        return rho;
    }


    uint8_t transformIct_8(uint8_t* pSourceBuffer/* inout */,
        const int32_t width/* in */, const int32_t height/* in */)
    {
        /* H. Qi, W. Gao, S. Ma and D. Zhao, "Adaptive Block-Size 
        Transform Based on Extended Integer 8×8/4×4 Transforms for
        H.264/AVC," 2006 International Conference on Image Processing
        , 2006, pp. 1341-1344, doi: 10.1109/ICIP.2006.312582.*/
        int32_t mat_H[64] = { 1,  1,  1,  1,  1,  1,  1,  1,
                              1,  1,  1,  0, -0, -1, -1, -1,
                              1,  1, -1, -1, -1, -1,  1,  1,
                              1, -0, -1, -1,  1,  1,  0, -1,
                              1, -1, -1,  1,  1, -1, -1,  1,
                              1, -1,  0,  1, -1, -0,  1, -1,
                              1, -1,  1, -1, -1,  1, -1,  1,
                              0, -1,  1, -1,  1, -1,  1, -0 };

        int32_t mat_H_T[64] = { 1,  1,  1,  1,  1,  1,  1,  0,
                                1,  1,  1,  0, -1, -1, -1, -1,
                                1,  1, -1, -1, -1,  0,  1,  1,
                                1,  0, -1, -1,  1,  1, -1, -1,
                                1,  0, -1,  1,  1, -1, -1,  1,
                                1, -1, -1,  1, -1,  0,  1, -1,
                                1, -1,  1,  0, -1,  1, -1,  1,
                                1, -1,  1, -1,  1, -1,  1,  0 };
        // S(x,y) = 1/sqrt((\sum_j H(x,j)^2)*(\sum_i H'(i,y)^2))
        // 1/8 1/4/sqrt(3) 1/6
        float_t a = 0.125, c = 1/6, b =0.25/sqrt(3);
        float_t mat_alpha[64] = { a, b, a, b, a, b, a, b,
                                  b, c, b, c, b, c, b, c,
                                  a, b, a, b, a, b, a, b,
                                  b, c, b, c, b, c, b, c,
                                  a, b, a, b, a, b, a, b,
                                  b, c, b, c, b, c, b, c,
                                  a, b, a, b, a, b, a, b,
                                  b, c, b, c, b, c, b, c };
        //int32_t mat_H[64] = { 64,  64,  64,  64,  64,  64,  64,  64,
        //                      89,  75,  50,  18, -18, -50, -75, -89,
        //                      83,  36, -36, -83, -83, -36,  36,  83,
        //                      75, -18, -89, -50,  50,  89,  18, -75,
        //                      64, -64, -64,  64,  64, -64, -64,  64,
        //                      50, -89,  18,  75, -75, -18,  89, -50,
        //                      36, -83,  83, -36, -36,  83, -83,  36,
        //                      18, -50,  75, -89,  89, -75,  50, -18 };

        //int32_t mat_H_T[64] = { 64,  89,  83,  75,  64,  50,  36,  18,
        //                        64,  75,  36, -18, -64, -89, -83, -50,
        //                        64,  50, -36, -89, -64,  18,  83,  75,
        //                        64,  18, -83, -50,  64,  75, -36, -89,
        //                        64, -18, -83,  50,  64, -75, -36,  89,
        //                        64, -50, -36,  89, -64, -18,  83, -75,
        //                        64, -75,  36,  18, -64,  89, -83,  50,
        //                        64, -89,  83, -75,  64, -50,  36, -18 };
        //// S(x,y) = 1/sqrt((\sum_j H(x,j)^2)*(\sum_i H'(i,y)^2))
        //// 1/8 1/4/sqrt(3) 1/6
        //float_t a = 3.05175781e-05, b = 3.05306250e-05, c = 3.05436775e-05;
        //float_t mat_alpha[64] = { a, b, b, b, a, b, b, b,
        //                          b, c, c, c, b, c, c, c,
        //                          b, c, c, c, b, c, c, c,
        //                          b, c, c, c, b, c, c, c,
        //                          a, b, b, b, a, b, b, b,
        //                          b, c, c, c, b, c, c, c,
        //                          b, c, c, c, b, c, c, c,
        //                          b, c, c, c, b, c, c, c };
        int32_t base_pos, offset_row;				// cur_pos_idx
        int32_t* tmp_data = new int32_t[64];		// 8 x 8 block from ICT and conv
        const int32_t idx_height = (height >> 3);	// every 8
        const int32_t idx_width = (width >> 3);		// every 8
        int32_t row, col, row_idx, col_idx;
        for (row = idx_height - 1; row >= 0; row--)
        {
            for (col = idx_width - 1; col >= 0; col--)
            {
                base_pos = (col << 3) + (row << 3) * width;
                /* get data */
                for (row_idx = 7; row_idx >= 0; row_idx--)
                {
                    offset_row = row_idx * width + base_pos;
                    for (col_idx = 7; col_idx >= 0; col_idx--)
                        *(tmp_data + (col_idx << 3) + row_idx) = (int32_t) * (pSourceBuffer + offset_row + col_idx);
                }
                compressIct8x8(tmp_data, mat_H, mat_H_T, mat_alpha);
                /* back to matrix */
                for (row_idx = 7; row_idx >= 0; row_idx--)
                {
                    offset_row = row_idx * width + base_pos;
                    for (col_idx = 7; col_idx >= 0; col_idx--)
                        *(pSourceBuffer + offset_row + col_idx) = (uint8_t) * (tmp_data + (col_idx << 3) + row_idx);
                }
            }
        }
        delete tmp_data; 
        return 0;
    }






    /* using the 4x4 ICT with JFCD kernel to cpmpress the data flow
     */
    uint8_t transformIct(uint8_t* pSourceBuffer/* inout */,
        const int32_t width/* in */, const int32_t height/* in */)
    {
        int32_t base_pos, offset_row;				// cur_pos_idx
        int32_t* tmp_data = new int32_t [16];		// 4 x 4 block from ICT and conv
        const int32_t idx_height = (height >> 2);	// every 4
        const int32_t idx_width = (width >> 2);		// every 4
        int32_t row, col, row_idx, col_idx;
        int32_t* mat_H = get_H_kernel(0);
        int32_t* mat_H_T = get_H_kernel(1);
        float_t* mat_alpha = get_alpha_kernel();
        for (row = idx_height - 1; row >= 0; row--)
        {
            for (col = idx_width - 1; col >= 0; col--)
            {
                base_pos = (col<<2) + (row<<2) * width;
                /* get data */
                for (row_idx = 3; row_idx >= 0; row_idx--)
                {
                    offset_row = row_idx * width + base_pos;
                    for (col_idx = 3; col_idx >=0; col_idx--)
                        *(tmp_data + (col_idx<<2) + row_idx) = (int32_t) *(pSourceBuffer + offset_row + col_idx);
                }
                compressIct(tmp_data, mat_H, mat_H_T, mat_alpha);
                /* back to matrix */
                for (row_idx = 3; row_idx >= 0; row_idx--)
                {
                    offset_row = row_idx * width + base_pos;
                    for (col_idx = 3; col_idx >= 0; col_idx--)
                        *(pSourceBuffer + offset_row + col_idx) = (uint8_t) * (tmp_data + (col_idx << 2) + row_idx);
                }
            }
        }
        delete tmp_data; delete mat_H; delete mat_H_T; delete mat_alpha;
        return 0;
    }


    /* algorithm of ICT */
    void compressIct(int32_t* data/* inout */, const int32_t* mat_H/* in */,
        const int32_t* mat_H_T/* in */, const float_t* mat_alpha/* in */)
    {
        int32_t row, col, cur_pos;
        mat_mpy4x4(data, mat_H);											// 编码
        for (row = 3; row >= 0; row--)
        {
            for (col = 3; col >= 0; col--)
            {
                cur_pos = (row << 2) + col;
                if (row < 2 && col < 2)										// 低频分量在矩阵的左上角
                    *(data + cur_pos) = (int32_t)((double)(*(data + cur_pos)// 图像卷积，后续要恢复，故俩次
                        * powf(*(mat_alpha + cur_pos), 2)));
                else
                    *(data + cur_pos) = 0;
            }
        }
        mat_mpy4x4(data, mat_H_T);											// 解码
    }
    void compressIct8x8(int32_t* data/* inout */, const int32_t* mat_H/* in */,
        const int32_t* mat_H_T/* in */, const float_t* mat_alpha/* in */)
    {
        int32_t row, col, cur_pos;
        mat_mpy8x8(data, mat_H);											// 编码
        for (row = 7; row >= 0; row--)
        {
            for (col = 7; col >= 0; col--)
            {
                cur_pos = (row << 3) + col;
                if (row < 4 && col < 4)										// 低频分量在矩阵的左上角
                    *(data + cur_pos) = (int32_t)((double)(*(data + cur_pos) * powf(*(mat_alpha + cur_pos), 2)));// 图像卷积，后续要恢复，故俩次
                else
                    *(data + cur_pos) = 0;
            }
        }
        mat_mpy8x8(data, mat_H_T);											// 解码
        
    }
    /* Y = H * X * H'
     * data(inout): X matrix -> Y matrix
     * H(in): H matrix
     */
    void mat_mpy4x4(int32_t* data/* inout */, const int32_t* ict_mat/* in */)
    {
        int32_t i, j, m, idx;
        int32_t* tmp = new int32_t [16];
        for (i = 15; i >= 0; i--)
        {
            j = i % 4;
            idx = (i >> 2 << 2);
            *(tmp + i) = 0;
            for (m = 3; m >= 0; m--)
            {
                *(tmp + i) += *(ict_mat + idx + m) * *(data + (m << 2) + j);
                // 可以用移位代替乘法，我认为编译器会帮我优化，并且这不是dsp不优化了
            }
        }
        for (i = 15; i >= 0; i--)
        {
            j = i % 4;
            idx = (i >> 2 << 2);
            *(data + i) = 0;
            for (m = 3; m >= 0; m--)
            {
                *(data + i) += *(tmp + idx + m) * *(ict_mat + (j << 2) + m);
            }
        }
        delete tmp;
    }

    /* Y = H * X * H'
     * data(inout): X matrix -> Y matrix
     * H(in): H matrix
     */
    void mat_mpy8x8(int32_t* data/* inout */, const int32_t* ict_mat/* in */)
    {
        int32_t i, j, m, idx;
        int32_t* tmp = new int32_t[64];
        for (i = 63; i >= 0; i--)
        {
            j = i % 8;
            idx = (i >> 3 << 3);
            *(tmp + i) = 0;
            for (m = 7; m >= 0; m--)
            {
                *(tmp + i) += *(ict_mat + idx + m) * *(data + (m << 3) + j);
                // 可以用移位代替乘法，我认为编译器会帮我优化，并且这不是dsp不优化了
            }
        }
        for (i = 63; i >= 0; i--)
        {
            j = i % 8;
            idx = (i >> 3 << 3);
            *(data + i) = 0;
            for (m = 7; m >= 0; m--)
            {
                *(data + i) += *(tmp + idx + m) * *(ict_mat + (j << 3) + m);
            }
        }
        delete tmp;
    }

    int32_t* get_H_kernel(bool is_t/* in */)
    {
        /*     |--       --|         |--       --|
         *     | 1, 1, 1, 1|         | 1, 2, 1, 1|
         * H = | 2, 1,-1,-2|   H^T = | 1, 1,-1,-2|
         *     | 1,-1,-1, 1|         | 1,-1,-1, 2|
         *     | 1,-2, 2,-1|         | 1,-2, 1,-1|
         *     |--       --|         |--       --|
         */
        int32_t* mat_H = new int32_t [16];
        if (is_t)
        {
            *(mat_H)      = 1; *(mat_H + 1)  =  2; *(mat_H + 2)  =  1; *(mat_H + 3)  =  1;
            *(mat_H + 4)  = 1; *(mat_H + 5)  =  1; *(mat_H + 6)  = -1; *(mat_H + 7)  = -2;
            *(mat_H + 8)  = 1; *(mat_H + 9)  = -1; *(mat_H + 10) = -1; *(mat_H + 11) =  2;
            *(mat_H + 12) = 1; *(mat_H + 13) = -2; *(mat_H + 14) =  1; *(mat_H + 15) = -1;
        }
        else
        {
            *(mat_H)      = 1; *(mat_H + 1)  =  1; *(mat_H + 2)  =  1; *(mat_H + 3)  =  1;
            *(mat_H + 4)  = 2; *(mat_H + 5)  =  1; *(mat_H + 6)  = -1; *(mat_H + 7)  = -2;
            *(mat_H + 8)  = 1; *(mat_H + 9)  = -1; *(mat_H + 10) = -1; *(mat_H + 11) =  1;
            *(mat_H + 12) = 1; *(mat_H + 13) = -2; *(mat_H + 14) =  2; *(mat_H + 15) = -1;
        }
        return mat_H;
    }

    float_t* get_alpha_kernel()
    {
        /*        |--						 --|
         *        |  a^2,	ab/2,	a^2,  ab/2 |
         * alpha= | ab/2,  b^2/4,  ab/2, b^2/4 |
         *        |  a^2,	ab/2,	a^2,  ab/2 |
         *        | ab/2,  b^2/4,  ab/2, b^2/4 |
         *        |--					     --|
         */

         //float_t a = 0.5;
         //float_t b = 0.6324555320336759; // \sqrt{\frac{2}{5}}
        float_t a2 = 0.25; float_t ab_2 = 0.15811388300841897; // ab / 2 = 0.25 * \sqrt{\frac{2}{5}}
        float_t b2_4 = 0.1; // b^2/4 = \frac{2}{5}/4 = \frac{1}{10}
        float_t* mat_alpha = new float_t[16];
        *(mat_alpha) = a2;  *(mat_alpha + 1) = ab_2; *(mat_alpha + 2) = a2;  *(mat_alpha + 3) = ab_2;
        *(mat_alpha + 4) = ab_2; *(mat_alpha + 5) = b2_4; *(mat_alpha + 6) = ab_2; *(mat_alpha + 7) = b2_4;
        *(mat_alpha + 8) = a2;	 *(mat_alpha + 9) = ab_2; *(mat_alpha + 10) = a2;  *(mat_alpha + 11) = ab_2;
        *(mat_alpha + 12) = ab_2; *(mat_alpha + 13) = b2_4; *(mat_alpha + 14) = ab_2; *(mat_alpha + 15) = b2_4;
        return mat_alpha; // conv image like cv
    }
}
