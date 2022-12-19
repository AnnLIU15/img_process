#ifndef __MOTIONEST_H__
#define __MOTIONEST_H__

#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>


namespace moEst 
{
	/* Absolute Differences */
	#define absSub(val1, val2) val1 > val2 ? val1 - val2: val2 -val1
	/* calculate the Sum of Absolute Differences*/
	int32_t SAD(uint8_t* pSourceBuffer, uint8_t* frames, const int32_t width, const int32_t offset_x, const int32_t offset_y, const uint8_t mode);
	/* inner search, get exact zone*/
	int32_t inner_search(uint8_t* pSourceBuffer, uint8_t* past_frame, const int32_t* SAD_for_points,
		const int32_t width, const int32_t height, const int32_t offset_x, const int32_t offset_y, const int32_t cen_x, const int32_t cen_y);
	/* search, get rough zone */
	int32_t origin_search(uint8_t* pSourceBuffer, uint8_t* past_frame,
		const int32_t width, const int32_t height, const int32_t offset_x, const int32_t offset_y);
	/* Api */
	int32_t* enhanced_HEXBS(uint8_t* pSourceBuffer, uint8_t* cur_frame, uint8_t* past_frame,
		const int32_t width, const int32_t height, const int32_t frame_num, FILE* fp);
}
#endif
