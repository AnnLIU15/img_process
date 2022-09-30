#include"../include/img_encode.h"
#pragma warning(disable:4996)
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
		//printf("\ntotal prob%f, gray%d\n", cur_ptr->gray_prob,cur_ptr->gray_level);
		left_ptr = cur_ptr->left_node;
		right_ptr = cur_ptr->right_node;
		//printf("\nleft prob%f, gray%d\n", left_ptr->gray_prob, left_ptr->gray_level);
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

Gray* getHuffmanEncodeChar(const GrayNode* root)
{
	GrayNode* node = root;
	const char* one_str = "1";
	const char* zero_str = "0";
	char* cur_str = (char*)malloc(sizeof(char) * 256);
	*cur_str = '\0';
	GrayNodeListWithChar* stack_list,*tmp_stack;
	stack_list = NULL;
	int16_t idx_cnt = 0;
	float_t avg_code_len = 0;
	Gray* huffmanEncodeTable = initTable(256);
	while ((node!=NULL) ||(stack_list!=NULL))
	{
		if (node->gray_level != -1)
		{
			(huffmanEncodeTable + idx_cnt)->gray_level = node->gray_level;
			(huffmanEncodeTable + idx_cnt)->gray_prob = node->gray_prob;
			setEncode((huffmanEncodeTable+ idx_cnt), cur_str);
			/*printf("level:%d,prob:%f\n", node->gray_level, node->gray_prob);
			printf("encode:%s\n", cur_str);*/
			idx_cnt++;
		}
		/* append stack*/
		tmp_stack = (GrayNodeListWithChar*)malloc(sizeof(GrayNodeListWithChar));
		tmp_stack->next = stack_list;
		tmp_stack->tmp_cmb_node = node;
		tmp_stack->encode_char = (char*)malloc(sizeof(char) * strlen(cur_str) + 1);
		*tmp_stack->encode_char = '\0';
		strcpy(tmp_stack->encode_char, cur_str);
		stack_list = tmp_stack;
		/*if (node->right_node != NULL)
			strcat(cur_str, one_str);*/
		node = node->right_node;
		strcat(cur_str, one_str);
		
		while ((node==NULL)&& (stack_list != NULL))
		{
			/* pop */
			tmp_stack = stack_list->next;
			node = stack_list->tmp_cmb_node;
			stack_list->next = NULL;
			stack_list->tmp_cmb_node = NULL;
			strcpy(cur_str,stack_list->encode_char);
			free(stack_list->encode_char);
			stack_list = tmp_stack;
			/*strcpy(tmp_str, cur_str);
			str(cur_str, tmp_str, strlen(tmp_str) - 1);
			*(cur_str + strlen(tmp_str) - 1) = '\0';*/
			strcat(cur_str, zero_str);
			node = node->left_node;
			
		}
	}
	free(cur_str);
	free(stack_list);
	return huffmanEncodeTable;
}

void freeGrayNode(GrayNode* root)
{
	GrayNode* node = root;
	GrayNode* free_tmp;
	GrayNodeList* stack_list, * tmp_stack;
	stack_list = NULL;
	while ((node != NULL) || (stack_list != NULL))
	{
		/* append stack*/
		tmp_stack = (GrayNodeList*)malloc(sizeof(GrayNodeList));
		tmp_stack->next = stack_list;
		tmp_stack->tmp_cmb_node = node;
		stack_list = tmp_stack;
		
		node = node->right_node;

		while ((node == NULL) && (stack_list != NULL))
		{
			/* pop */
			tmp_stack = stack_list->next;
			node = stack_list->tmp_cmb_node;
			stack_list->next = NULL;
			stack_list->tmp_cmb_node = NULL;
			stack_list = tmp_stack;

			free_tmp = node;
			node = node->left_node;
			free_tmp->left_node = NULL;
			free_tmp->right_node = NULL;
			free(free_tmp);
		}
	}
}

Gray* initTable(const int32_t length)
{
	Gray* table = (Gray*)malloc(sizeof(Gray) * length);
	int32_t idx_i = length - 1;
	/*for (; idx_i >= 0; idx_i--)
	{
		*(table + idx_i)->encode = NULL;
	}*/
	return table;
}

void setEncode(Gray* table_ptr, const char* origin)
{
	const int32_t length = strlen(origin);
	table_ptr->encode = (char*)malloc(sizeof(char) * (length + 1));
	*(table_ptr->encode) = '\0';
	strcpy(table_ptr->encode, origin);
}

void freeTable(Gray* table, const int32_t length)
{
	int32_t idx_i = length-1;
	for (; idx_i >= 0;idx_i--)
	{
		free((table + idx_i)->encode);
	}
	free(table);
}

/* bubble sort(smallest->largest)
 * data_array(inout): array which should be sorted
 * length(in): array length
 */
void sortEncodeWithProb(Gray* prob_arr, const uint64_t length)
{
	uint8_t processed = 1;
	int32_t largest_position = 0;
	float_t tmp_float_data = 0;
	uint8_t tmp_uint8_data = 0;
	int32_t i, j;
	Gray tmp_arr;
	for (i = length - 1; i > 0; i--)
	{
		processed = 0;
		largest_position = i;
		for (j = i - 1; j >= 0; j--)
		{
			if ((prob_arr + largest_position)->gray_prob < (prob_arr + j)->gray_prob)
			{
				largest_position = j;
				processed = 1;
			}
		}
		if (processed == 1)
		{

			tmp_arr = *(prob_arr + largest_position);
			*(prob_arr + largest_position) = *(prob_arr + i);
			*(prob_arr + i) = tmp_arr;
		}
		else
		{
			;
		}
	}
}