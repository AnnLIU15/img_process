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
    int32_t					gray_level;     // pixel gray intensity
	float_t					gray_prob;
	struct GrayNode*		left_node;      // left node
	struct GrayNode*		right_node;     // right node
}GrayNode;

typedef struct tagGrayNodeList
{
	GrayNode* tmp_cmb_node;      // left node
	struct GrayNodeList* next;			// next tmp
}GrayNodeList;


GrayNode* getHuffmanTree(const float_t* gray_ptr,const uint8_t* gray_level, const int32_t length);

void setNodeVal(GrayNode* set_ptr, const int32_t gray_data, const float_t gray_prob, const GrayNode* left_ptr, const GrayNode* right_ptr);

void freeGrayNode(GrayNode* root);


#endif /* IMG_ENCODE_H_ */