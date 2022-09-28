#include"../include/img_encode.h"

GrayNode* getHuffmanTree(const uint16_t* gray_ptr/* sorted */, const int32_t length)
{
	GrayNode* cur_ptr, * left_ptr, * right_ptr;
	cur_ptr = NULL;
	int32_t idx_i = length - 1;
	if (idx_i < 0)
	{
		printf("\nError length(%d) is smaller than 0!!!\n", idx_i);
	}
	else
	{
		if (idx_i == 0)
		{
			cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
			setNodeVal(cur_ptr, *(gray_ptr + idx_i--), NULL, NULL);
			left_ptr = NULL; right_ptr = NULL;
			printf("h(%d)\n", *(gray_ptr + idx_i+1));
		}
		else if (0 == idx_i % 2)
		{
			left_ptr = (GrayNode*)malloc(sizeof(GrayNode));
			setNodeVal(left_ptr, *(gray_ptr + idx_i--), NULL, NULL);
			right_ptr = (GrayNode*)malloc(sizeof(GrayNode));
			setNodeVal(right_ptr, *(gray_ptr + idx_i--), NULL, NULL);
			printf("l(%d)r(%d)\n", *(gray_ptr + idx_i + 2), *(gray_ptr + idx_i + 1));
		}
		else
		{
			right_ptr = (GrayNode*)malloc(sizeof(GrayNode));
			setNodeVal(right_ptr, *(gray_ptr + idx_i--), NULL, NULL);
			left_ptr = NULL;
			printf("r(%d)\n", *(gray_ptr + idx_i + 1));
		}

		for (; idx_i >= 0;)
		{
			if (0 == idx_i)
			{
				cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(cur_ptr, *(gray_ptr + idx_i--), left_ptr, right_ptr);
				printf("h(%d)\n", *(gray_ptr + idx_i + 1));
			}
			else
			{
				cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(cur_ptr, *(gray_ptr + idx_i--), left_ptr, right_ptr);
				left_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(left_ptr, *(gray_ptr + idx_i--), NULL, NULL);
				right_ptr = cur_ptr;
				printf("l(%d)r(%d)\n", *(gray_ptr + idx_i + 2), *(gray_ptr + idx_i + 1));
			}
		}
	}
	left_ptr = NULL; right_ptr = NULL;
	free(left_ptr); free(right_ptr);
	return cur_ptr;
}


/* set Tree Node value
 * Input set_ptr, data, left_node, right_node
 * Output None
 */
void setNodeVal(GrayNode* set_ptr, const uint16_t gray_data, const GrayNode* left_ptr, const GrayNode* right_ptr)
{
	set_ptr->grad_level = gray_data;
	set_ptr->left_node = left_ptr;
	set_ptr->right_node = right_ptr;
}