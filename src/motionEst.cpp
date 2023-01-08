#include "motionEst.h"

namespace moEst 
{

	int32_t SAD(uint8_t* pSourceBuffer, uint8_t* frames, const int32_t width, const int32_t height, const int32_t offset_x, const int32_t offset_y, const int32_t mode)
	{
		int32_t sad = 0;
		int32_t idx_i, idx_j;
		int32_t offset_p_x = 0, offset_p_y = 0;
		if (mode > 0)
		{
			offset_p_x = offset_x;
			offset_p_y = offset_y;
			for (idx_i = 15; idx_i >= 0; idx_i--)
			{
				for (idx_j = 15; idx_j >= 0; idx_j--)
				{
					if (offset_p_x<0 || offset_p_y<0 || (offset_p_x + 16)>height || (offset_p_y + 16)>width)
					{
						continue;
					}
					sad += absSub(*(pSourceBuffer + (offset_p_x + idx_i) * width + offset_p_y + idx_j),
						*(frames + (offset_x + idx_i) * width + offset_y + idx_j));
				}
			}
			
		}
		else
		{
			for (idx_i = 15; idx_i >= 0; idx_i--)
			{
				for (idx_j = 15; idx_j >= 0; idx_j--)
				{
					sad += absSub(*(pSourceBuffer + (offset_p_x + idx_i) * width + offset_p_y + idx_j),
						*(frames + (offset_x + idx_i) * width + offset_y + idx_j));
				}
			}
		}
		
		return sad;
	}

	int32_t inner_search(uint8_t* pSourceBuffer, uint8_t* past_frame, const int32_t* SAD_for_points,
		const int32_t width, const int32_t height, const int32_t offset_x, const int32_t offset_y, const int32_t cen_x, const int32_t cen_y)
	{
		/* get the searching zone, get the closet inner point */
		int32_t* search_area = (int32_t*)malloc(sizeof(int32_t) * 6);
		memset(search_area, 0, sizeof(int32_t) * 6);
		// searching point-pairs
		int32_t searchGroup[12] = { 0,2,2,5,5,6,6,4,4,1,1,0 };
		// HEX inner search (left->right top->bottom traversal the points)
		int32_t inner_points[18] = { -1,-1,  0,-1,  1,-1,  -1,0,  0,0,  1,0,  -1,1,  0,1,  1,1 };
		// scheme of quick inner search 
		int32_t quick_inner_search[18] = { 1,2,4, 2,5,8, 8,7,4, 7,6,4, 6,3,0, 0,1,4 };


		int32_t search_point = 0;
		/* initial SAD*/
		int32_t min_SAD = 1 << 21, cur_SAD = 0;
		/* minima group index */
		int32_t group_idx = 0, idx = 0, tmp;
		/* cur pos */
		int32_t cur_x = 0, cur_y = 0;
		/* opt pos */
		int32_t opt_x = 0, opt_y = 0;


		for (idx = 0; idx < 6; idx++)
		{
			tmp = idx << 1;
			cur_SAD = *(SAD_for_points + *(searchGroup + tmp)) +
				*(SAD_for_points + *(searchGroup + tmp + 1));
			if (cur_SAD < min_SAD)
			{
				min_SAD = cur_SAD;
				group_idx = idx;
			}
		}
		// get search zone (the inner point closer to group_idx)

		for (idx = 0; idx < 3; idx++)
		{
			tmp = idx << 1;
			*(search_area + tmp) = *(inner_points +
				(*(quick_inner_search + group_idx * 3 + idx) << 1));
			*(search_area + tmp + 1) = *(inner_points +
				(*(quick_inner_search + group_idx * 3 + idx) << 1) + 1);
		}


		/* reinit, get the opt point */
		min_SAD = 1 << 21; cur_SAD = 0;

		for (idx = 0; idx < 3; idx++)
		{
			tmp = idx << 1;
			if (*(search_area + tmp) == 0 && *(search_area + tmp + 1) == 0)
			{
				;
			}
			else
			{
				cur_x = cen_x + *(search_area + tmp);
				cur_y = cen_y + *(search_area + tmp + 1);
				// center
				if (cur_x<0 || cur_y<0 || (cur_x + 16)>height || (cur_y + 16)>width)
				{
					;
				}
				else
				{
					// calculate SAD
					cur_SAD = SAD(pSourceBuffer + offset_x * width + offset_y, past_frame, width, height, cur_x, cur_y, 0);
					search_point += 1;

					// get optimazed position
					if (cur_SAD < min_SAD)
					{
						min_SAD = cur_SAD;
						opt_x = cur_x;
						opt_y = cur_y;
					}
				}
			}
		}

		// store the match block into pSourceBuffer, and display
		for (int32_t p = 0; p < 16; p++)
		{
			for (int32_t q = 0; q < 16; q++)
			{
				*(pSourceBuffer + (offset_x + p) * width + q + offset_y) =
					*(past_frame + (opt_x + p) * width + opt_y + q);
			}
		}
		free(search_area);
		return search_point;
	}

	int32_t origin_search(uint8_t* pSourceBuffer, uint8_t* past_frame,
		const int32_t width, const int32_t height, const int32_t offset_x, const int32_t offset_y)
	{
		int32_t search_point = 0;
		/* center (x,y) */
		int32_t cen_x = offset_x, cen_y = offset_y;
		/* cur (x,y) */
		int32_t cur_x = 0, cur_y = 0;
		/* opt (x,y) */
		int32_t opt_x = 0, opt_y = 0;
		/* initial SAD*/
		int32_t min_SAD = 1 << 21;
		int32_t idx;
		int32_t is_finish = 0;
		/* SAD of each search points */
		int32_t* SAD_for_points = (int32_t*)malloc(sizeof(int32_t) * 7);
		memset(SAD_for_points, 0, sizeof(int32_t) * 6);
		// large HEXBS pattern (left->right top->bottom traversal the points)
		int32_t LHEXSP[14] = { 0,-2,-2,-1,2,-1,0,0,-2,1,2,1,0,2 };

		while (is_finish == 0)
		{
			for (idx = 0; idx < 7; idx++)
			{
				cur_x = cen_x + *(LHEXSP + (idx << 1));
				cur_y = cen_y + *(LHEXSP + (idx << 1) + 1);

				// outer of the search zone, continue to pass the result
				if (cur_x<0 || cur_y<0 || (cur_x + 16)>height || (cur_y + 16)>width)
				{
					*(SAD_for_points + idx) = 1 << 21;
				}
				else
				{
					// calculate SAD
					*(SAD_for_points + idx) = SAD(pSourceBuffer + offset_x * width + offset_y, past_frame, width, height, cur_x, cur_y, 0);
					search_point += 1;
					// get the optimized position
					if (*(SAD_for_points + idx) < min_SAD)
					{
						min_SAD = *(SAD_for_points + idx);
						opt_x = cur_x; opt_y = cur_y;
					}
				}
			}
			// travere done, if the optimization location in center
			if (opt_x == cen_x && opt_y == cen_y)
			{
				search_point += inner_search(pSourceBuffer, past_frame, SAD_for_points, width, height, offset_x, offset_y, opt_x, opt_y);
				is_finish = 1;
			}
			else // not the best, wait4optimize
			{
				cen_x = opt_x; cen_y = opt_y;
				min_SAD = 1 << 21;
			}
		}
		free(SAD_for_points);
		return search_point;
	}

	int32_t* enhanced_HEXBS(uint8_t* pSourceBuffer, uint8_t* cur_frame, uint8_t* past_frame,
		const int32_t width, const int32_t height, const int32_t frame_num, FILE* fp)
	{
		int32_t* data = (int32_t*)malloc(sizeof(int32_t) * 3);
		memset(data, 0, sizeof(int32_t) * 3);
		if (frame_num)  // process
		{
			int32_t points_frame = 0;	// searched points
			int32_t SAD_frame = 0;		// SAD in this frame
			int32_t blocks_frame = 0;	// blocks in this frame
			int32_t idx_row, idx_col;
			/* store cur frame */
			memcpy(cur_frame, pSourceBuffer, sizeof(uint8_t) * width * height);
			// enhanced_HEXBS ALGO 
			for (idx_row = 0; idx_row < height; idx_row += 16)
			{
				for (idx_col = 0; idx_col < width; idx_col += 16)
				{
					blocks_frame += 1;
					points_frame += origin_search(pSourceBuffer, past_frame, width, height, idx_row, idx_col);
					SAD_frame += SAD(pSourceBuffer, cur_frame, width, height, idx_row, idx_col, 1);;
				}
			}
			/*fprintf(fp, "%d,%.4f,%.4f\n", frame_num + 1,
				float(points_frame) / blocks_frame,
				float(SAD_frame) / (width * height));*/
			/* block/ SAD/ points*/
			*(data) = blocks_frame;
			*(data + 1) = SAD_frame;
			*(data + 2) = points_frame;
			/* store previous frame */
			memcpy(past_frame, cur_frame, sizeof(uint8_t) * width * height);

		}
		else // just store first frame
		{
			memcpy(past_frame, pSourceBuffer, sizeof(uint8_t) * width * height);
		}
		return data;
	}

}