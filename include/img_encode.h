/*
 * img_encode.h
 *
 *  Created on: 2022-09-28
 *      Author: Ann
 */
#ifndef IMG_ENCODE_H_
#define IMG_ENCODE_H_
#include"data_type.h"
 
typedef struct tagGrayNode
{
    int32_t         grad_level;     // pixel gray intensity
	struct GrayNode*       left_node;      // left node
	struct GrayNode*       right_node;     // right node
}GrayNode;


GrayNode* getHuffmanTree(const uint16_t* gray_ptr,const int32_t length);

void setNodeVal(GrayNode* set_ptr, const uint16_t gray_data, const GrayNode* left_ptr, const GrayNode* right_ptr);




#endif /* IMG_ENCODE_H_ */