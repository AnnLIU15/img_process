#include"../include/img_encode.h"

GrayNode* getHuffmanTree(const float_t* gray_ptr/* sorted */, const uint8_t* gray_level, const int32_t length)
{
	GrayNode* cur_ptr, * left_ptr, * right_ptr;
	cur_ptr = NULL;
	int32_t idx_i = 0;
	uint8_t tmp_idx_next = 0;
	GrayNodeList* cur_list_ptr, * head_list_ptr, * tmp_list_ptr;
	uint8_t type = 0;
	
	idx_i = 0;
	if (length <= 0)
	{
		printf("\nError length(%d) is smaller than 0!!!\n", idx_i);
	}
	else if (length == 1)
	{
		cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
		setNodeVal(cur_ptr, *(gray_level + idx_i), *(gray_ptr + idx_i), NULL, NULL);
		left_ptr = NULL; right_ptr = NULL;
	}
	else 
	{
		left_ptr = (GrayNode*)malloc(sizeof(GrayNode));
		setNodeVal(left_ptr, *(gray_level + idx_i), *(gray_ptr + idx_i), NULL, NULL);
		right_ptr = (GrayNode*)malloc(sizeof(GrayNode));
		setNodeVal(right_ptr, *(gray_level + idx_i+1), *(gray_ptr + idx_i+1), NULL, NULL);
		idx_i += 2;
		cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
		setNodeVal(cur_ptr, -1, left_ptr->gray_prob+right_ptr->gray_prob, left_ptr, right_ptr);
		head_list_ptr = (GrayNodeList*)malloc(sizeof(GrayNodeList));
		head_list_ptr->next = NULL;
		head_list_ptr->tmp_cmb_node = cur_ptr;
		cur_list_ptr = head_list_ptr;
		//int16_t debug_idx = 0;
		for (; idx_i < length;)
		{
			//debug_idx++;
			if (idx_i <= length - 2)
			{
				if (head_list_ptr->next == NULL)
				{
					type = (head_list_ptr->tmp_cmb_node)->gray_prob < *(gray_ptr + idx_i + 1) ? 2 : 0;
				}
				else 
				{
					tmp_list_ptr = head_list_ptr->next;
					// printf("%d ", idx_i);
					// printf("%f ", head_list_ptr->tmp_cmb_node->gray_prob);
					// printf("%f ", *(gray_ptr + idx_i));
					// printf("%f ", *(gray_ptr + idx_i + 1));
					// printf("%f ", tmp_list_ptr->tmp_cmb_node->gray_prob);
					type = head_list_ptr->tmp_cmb_node->gray_prob < *(gray_ptr + idx_i + 1) ? 1 : 
							(tmp_list_ptr->tmp_cmb_node->gray_prob < *(gray_ptr + idx_i) ? 2 :0 );
					// printf("%d\n", type);
				}
			}
			else
			{
				if (head_list_ptr->next == NULL)
				{
					type = 2;
				}
				else
				{
					tmp_list_ptr = head_list_ptr->next;
					type = tmp_list_ptr->tmp_cmb_node->gray_prob < *(gray_ptr + idx_i) ? 1 : 2;
				}
			}

			if (0 == type)
			{
				left_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(left_ptr, *(gray_level + idx_i), *(gray_ptr + idx_i), NULL, NULL);
				right_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(right_ptr, *(gray_level + idx_i + 1), *(gray_ptr + idx_i + 1), NULL, NULL);
				idx_i += 2;
				cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(cur_ptr, -1, left_ptr->gray_prob + right_ptr->gray_prob, left_ptr, right_ptr);
				tmp_list_ptr = (GrayNodeList*)malloc(sizeof(GrayNodeList));
				tmp_list_ptr->next = NULL;
				tmp_list_ptr->tmp_cmb_node = cur_ptr;
				cur_list_ptr->next = tmp_list_ptr;
				cur_list_ptr = cur_list_ptr->next;
			}
			else if(1 == type)
			{
				left_ptr = head_list_ptr->tmp_cmb_node;
				//printf("%d,%f\t", idx_i, head_list_ptr->tmp_cmb_node->gray_prob);
				tmp_list_ptr = head_list_ptr->next;
				free(head_list_ptr);
				head_list_ptr = tmp_list_ptr;
				right_ptr = head_list_ptr->tmp_cmb_node;
				
				//printf("%f\n", head_list_ptr->tmp_cmb_node->gray_prob);
				
				//printf("%d ", debug_idx);
				cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(cur_ptr, -1, left_ptr->gray_prob + right_ptr->gray_prob, left_ptr, right_ptr);
				tmp_list_ptr = (GrayNodeList*)malloc(sizeof(GrayNodeList));
				tmp_list_ptr->next = NULL;
				tmp_list_ptr->tmp_cmb_node = cur_ptr;
				cur_list_ptr->next = tmp_list_ptr;
				cur_list_ptr = cur_list_ptr->next;
				// freeGrayNodeList(head_list_ptr, 0);
				tmp_list_ptr = head_list_ptr->next;
				free(head_list_ptr);
				head_list_ptr = tmp_list_ptr;
			}
			else
			{
				left_ptr = head_list_ptr->tmp_cmb_node;
				right_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(right_ptr, *(gray_level + idx_i), *(gray_ptr + idx_i), NULL, NULL);
				idx_i += 1;
				cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
				setNodeVal(cur_ptr, -1, left_ptr->gray_prob + right_ptr->gray_prob, left_ptr, right_ptr);
				tmp_list_ptr = (GrayNodeList*)malloc(sizeof(GrayNodeList));
				tmp_list_ptr->next = NULL;
				tmp_list_ptr->tmp_cmb_node = cur_ptr;
				cur_list_ptr->next = tmp_list_ptr;
				cur_list_ptr = cur_list_ptr->next;
				tmp_list_ptr = head_list_ptr->next;
				free(head_list_ptr);
				head_list_ptr = tmp_list_ptr;
			}
		}
	
		while (head_list_ptr->next!=NULL)
		{
			left_ptr = head_list_ptr->tmp_cmb_node;
			tmp_list_ptr = head_list_ptr->next;
			free(head_list_ptr);
			head_list_ptr = tmp_list_ptr;
			right_ptr = head_list_ptr->tmp_cmb_node;

			cur_ptr = (GrayNode*)malloc(sizeof(GrayNode));
			
			setNodeVal(cur_ptr, -1, left_ptr->gray_prob + right_ptr->gray_prob, left_ptr, right_ptr);
			//printf("\ncurrent prob%f, gray%d\n", cur_ptr->gray_prob, cur_ptr->gray_level);

			tmp_list_ptr = (GrayNodeList*)malloc(sizeof(GrayNodeList));
			tmp_list_ptr->next = NULL;
			tmp_list_ptr->tmp_cmb_node = cur_ptr;
			cur_list_ptr->next = tmp_list_ptr;
			cur_list_ptr = cur_list_ptr->next;
			tmp_list_ptr = head_list_ptr->next;
			free(head_list_ptr);
			head_list_ptr = tmp_list_ptr;
		}
		cur_ptr = head_list_ptr->tmp_cmb_node;
		printf("\ntotal prob%f, gray%d\n", cur_ptr->gray_prob,cur_ptr->gray_level);
		left_ptr = cur_ptr->left_node;
		right_ptr = cur_ptr->right_node;
		printf("\nleft prob%f, gray%d\n", left_ptr->gray_prob, left_ptr->gray_level);
		tmp_list_ptr = NULL; cur_list_ptr = NULL;
		head_list_ptr = NULL;
		free(head_list_ptr);
		free(tmp_list_ptr); free(cur_list_ptr);
	}
	
	left_ptr = NULL; right_ptr = NULL;
	free(left_ptr); free(right_ptr);
	return cur_ptr;
}


/* set Tree Node value
 * Input set_ptr, data, left_node, right_node
 * Output None
 */
void setNodeVal(GrayNode* set_ptr, const int32_t gray_data, const float_t gray_prob, const GrayNode* left_ptr, const GrayNode* right_ptr)
{
	set_ptr->gray_level = gray_data;
	set_ptr->gray_prob = gray_prob;
	set_ptr->left_node = left_ptr;
	set_ptr->right_node = right_ptr;
}

