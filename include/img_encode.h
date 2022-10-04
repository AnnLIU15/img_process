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

typedef struct tagGrayNodeListWithChar
{
	GrayNode* tmp_cmb_node;					// left node
	struct tagGrayNodeListWithChar* next;	 // next tmp
	char* encode_char;
}GrayNodeListWithChar;


typedef struct tagGray
{
	int32_t		gray_level;     // pixel gray intensity
	float_t		gray_prob;
	char*		encode;
}Gray;

GrayNode* getHuffmanTree(const float_t* gray_ptr,const uint8_t* gray_level, const int32_t length);

void setNodeVal(GrayNode* set_ptr, const int32_t gray_data, const float_t gray_prob, const GrayNode* left_ptr, const GrayNode* right_ptr);

void freeGrayNode(GrayNode* root);

Gray* getHuffmanEncodeChar(const GrayNode* root);

Gray* initTable(const int32_t length);

void setEncode(Gray* table_ptr, const char* origin);

void freeTable(Gray* table, const int32_t length);

void sortEncodeWithProb(Gray* prob_arr, const uint64_t length);

Gray* getGolombEncodeChar(float_t* prob_arr, uint8_t* gray_index, const int32_t length);

#endif /* IMG_ENCODE_H_ */