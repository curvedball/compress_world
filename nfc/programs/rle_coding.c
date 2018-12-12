
/*
	rle_coding.c
*/


#include "rle_coding.h"



void reset_data(char* buf, int len)
{
	memset(buf, 0, len);
}




int  SetBBlockBit(char* bblock_head, int cur_pos)
{
	if (cur_pos < 0 || cur_pos > 31)
	{
		printf("SetBBlockBit error! invalid cur_pos: %d\n", cur_pos);
		return -1;
	}

	if (cur_pos >=0 && cur_pos <= 7)
	{
		bblock_head[0] = bblock_head[0] | (1 << cur_pos);
	}
	else if (cur_pos >=8 && cur_pos <= 15)
	{
		bblock_head[1] = bblock_head[1] | (1 << (cur_pos - 8));
	}
	else if (cur_pos >=16 && cur_pos <= 23)
	{
		bblock_head[2] = bblock_head[2] | (1 << (cur_pos - 16));
	}
	else
	{
		bblock_head[3] = bblock_head[3] | (1 << (cur_pos - 24));
	}
	
	return 0;
}



int rle_coding_data(char* input_buffer, int input_size, char* output_buffer, int* poutput_size)
{
	char* ptr_in_end = input_buffer + input_size;
	char* ptr_in = input_buffer;	
	char* ptr_out_begin = output_buffer;	
	char* ptr_out = output_buffer;

	//
	BYTE bblock_head[4]; //zb: 4
	BYTE bblock_data[64]; //zb: 32+32
	BYTE cur_symbol;
	int  cur_pos = 0;
	int  repeat_count = 0;
	int  count_pos = 0; //
	char* ptr_stub = ptr_in;
	int  tail_len;
	int orig_len;

	//
	reset_data(bblock_head, 4);
	reset_data(bblock_data, 64);

	//
	while (ptr_in < ptr_in_end)
	{
		cur_symbol = *ptr_in;
		bblock_data[cur_pos] = cur_symbol;
		do
		{
			ptr_in++;
			repeat_count++;
			if (repeat_count == 255)
			{
				break;
			}	
		}while (cur_symbol == *ptr_in);

		//
		if (repeat_count > 1)
		{
			if (SetBBlockBit(bblock_head, cur_pos))
			{
				return -1;
			}
			bblock_data [cur_pos] = cur_symbol;
			cur_pos++;
			bblock_data [32 + count_pos] = repeat_count;
			count_pos++;
			repeat_count = 0;
			//DbgPrint("SetBit: %d count_pos: %d\n", cur_pos, count_pos);
		}
		else
		{
			bblock_data [cur_pos] = cur_symbol;
			cur_pos++;
			repeat_count = 0;
		}

		//==============Block is full============
		if (cur_pos == 32)
		{
			orig_len = ptr_in - ptr_stub;
			if (orig_len < 37 + count_pos && orig_len < 128)
			{
				//DbgPrint("---Convert it to vBlock and Put it into output buffer---\n");
				*ptr_out = orig_len << 1;
				memcpy(ptr_out + 1, ptr_stub, orig_len);
				ptr_out += (1 + orig_len);
				ptr_stub = ptr_in;
			}
			else
			{
				//DbgPrint("---BBlock and Put it into output buffer---count_pos: %d\n", count_pos);
				*ptr_out = 0x41; //01000001, the highest 7 bits records the valid bit numbers.
				memcpy(ptr_out + 1, bblock_head, 4);
				memcpy(ptr_out + 5, bblock_data, 32);
				memcpy(ptr_out + 37, bblock_data + 32, count_pos);
				ptr_out += 37 + count_pos;
				ptr_stub = ptr_in;
			}

			//
			reset_data(bblock_head, 4);
			reset_data(bblock_data, 64);
			cur_pos = 0;
			count_pos = 0;
		}
	}

	//=============Block is Not Full=============
	if (cur_pos != 0)
	{
		//DbgPrint("Block is not full! cur_pos: %d ptr_out_pos: %d\n", cur_pos, ptr_out - ptr_out_begin);
		//The last block may be not full
		int valid_data_len = 1 + 4 + cur_pos + count_pos;
		tail_len = ptr_in_end - ptr_stub;

		//
		if (tail_len > valid_data_len) //zb: still can be compressed!
		{
			*ptr_out = ((cur_pos << 1) | 1) & 0xFF; //The higher 7 bits records the valid bit numbers!
			memcpy(ptr_out + 1, bblock_head, 4);
			memcpy(ptr_out + 5, bblock_data, cur_pos);
			memcpy(ptr_out + 5 + cur_pos, bblock_data + 32, count_pos);
			ptr_out += valid_data_len;	
		}
		else
		{
			*ptr_out = tail_len << 1;
			memcpy(ptr_out + 1, ptr_stub, tail_len);
			ptr_out += tail_len + 1;
		}

	}

	int output_size = ptr_out - ptr_out_begin;
	*poutput_size = output_size;
	
	//DbgPrint("output_size: %d\n", output_size);
	return 0;
}



int  GetBBlockBit(char* bblock_head, int cur_pos)
{
	if (cur_pos >=0 && cur_pos <= 7)
	{
		return bblock_head[0] & (1 << cur_pos);
	}
	else if (cur_pos >=8 && cur_pos <= 15)
	{
		return bblock_head[1] & (1 << (cur_pos - 8));
	}
	else if (cur_pos >=16 && cur_pos <= 23)
	{
		return bblock_head[2] & (1 << (cur_pos - 16));
	}
	else
	{
		return bblock_head[3] & (1 << (cur_pos - 24));
	}
	return 0;
}




int rle_decoding_data(char* input_buffer, int input_size, char* output_buffer, int* poutput_size)
{
	char* ptr_in_end = input_buffer + input_size;
	char* ptr_in = input_buffer;	
	char* ptr_out_begin = output_buffer;	
	char* ptr_out = output_buffer;
	//char bblock_head[4]; //zb: 4
	char bblock_data[64]; //zb: 32+32
	BYTE cur_type;
	BYTE cur_len;
	BYTE cur_symbol;
	int  cur_pos = 0;
	int  repeat_count = 0;
	int  count_pos = 0; //
	char* ptr_stub = ptr_in;
	BYTE val;
	int offset_len;
	//int  tail_len;
	//int valid_len;
	int i;


	//
	//DbgPrint("zb===input_data_len: %d\n", input_size);
	while (ptr_in < ptr_in_end)
	{
		cur_pos = 0;
		repeat_count = 0;
		count_pos = 0;

		val = (*ptr_in);
		cur_type = (*ptr_in) & 0x01;
		cur_len = (*ptr_in >> 1) & 0xFF;
		ptr_in++;
		//DbgPrint("\n===RLE_Block begin!===value: 0x%.2X cur_type: %d cur_len: %d\n", val&0xFF, cur_type, cur_len);

		//
		if (cur_type == 0)
		{
			//DbgPrint("do_type0\n");
			//tail_len = ptr_in_end - ptr_in;
			//valid_len = tail_len > cur_len ? cur_len : tail_len;
			memcpy(ptr_out, ptr_in, cur_len);
			ptr_out += cur_len;
			ptr_in += cur_len;
		}
		else if (cur_type == 1)
		{
			//DbgPrint("do_type1\n");
			ptr_stub = ptr_in + 4;
			for (i = 0; i < cur_len; i++)
			{
				cur_symbol = *(ptr_stub + cur_pos);
				//DbgPrint("cur_symbol: 0x%.2x i: %d\n", cur_symbol, i);
				if (GetBBlockBit(ptr_in, i))
				{
					//DbgPrint("Bit1====cur_len: %d count_pos: %d\n", cur_len, count_pos);
					repeat_count = (unsigned char)*(ptr_stub + cur_len + count_pos); //zb?
					//DbgPrint("repeat_count: %d\n", repeat_count);
					memset(ptr_out, cur_symbol, repeat_count);
					cur_pos += 1;
					count_pos += 1;
					ptr_out += repeat_count;
					//DbgPrint("repeat! repeat_count: %d output_len: %d\n", repeat_count, ptr_out - ptr_out_begin);
				}
				else
				{
					//DbgPrint("Bit0\n");
					*ptr_out++ = cur_symbol;
					cur_pos += 1;
					//DbgPrint("NO repeat! output_len: %d\n", ptr_out - ptr_out_begin);
				}
			}
			offset_len = 4 + cur_len + count_pos;
			//DbgPrint("count_pos: %d\n", count_pos);
			ptr_in += offset_len;
		}
		else
		{
			printf("Error! unknown type value: %d processed_len: %d\n", cur_type, ptr_in - input_buffer);
			return -1;
		}

		//DbgPrint("========Decode One Block OK!============\n\n");
	}
	int output_size = ptr_out - ptr_out_begin;
	*poutput_size = output_size;
	//DbgPrint("rle_decoding_data==output_size: %d\n", output_size);

	return 0;
}




int rle_coding_file(char* input_filename, char* base_name, int column_id)
{
    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    FILE* srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize == 0)
	{
		return 0;
	}

	char* input_buffer = malloc(srcSize);
	if (!input_buffer)
	{
		printf("Allocate input_buffer memory error!\n");
		return -1;
	}
		
	if (UTIL_readFile(srcFile, input_buffer, srcSize) < 0)
	{
		return -1;
	}



	//============================================================
	char* out_buffer = malloc(srcSize * 2);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }

	int dstSize;
	if (rle_coding_data(input_buffer, srcSize, out_buffer, &dstSize))
	{
		printf("rle_coding_data error!\n");
        return -1;		
	}



	//============================================================
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	sprintf(o_filename, "%s_rle%.2d", base_name, column_id);
	
	FILE* dstFile = FIO_openDstFile(o_filename);
	if (!dstFile)
	{
		printf("dstFile error!\n");
    	return -1;
	}

	if (dstSize == 0)
	{
		printf("dstSize is zero!\n");
		return -1;
	}

	DbgPrint("rle_coding_column===input_filename: %s srcSize: %d dstSize: %d\n", input_filename, srcSize, dstSize);


	if (UTIL_writeFile(dstFile, out_buffer, dstSize))
	{
		return -1;
	}
	fclose(dstFile);

	//
	fclose(srcFile);

	return 0;
}



int nfc_rle_coding(FIELD_DESC* pfield_desc, int field_num)
{
	FIELD_DESC* pcur_field_desc;
	for (int i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		if (pcur_field_desc->rle_coding == TRUE)
		{
			DbgPrint("nfc_rle_coding_column_func! field_id: %d field_name: %s\n", pcur_field_desc->id, pcur_field_desc->name);
			for (int j = 0; j < pcur_field_desc->width; j++)
			{
				if (nfc_rle_coding_column(&(pcur_field_desc->col_desc), j))
				{
					printf("nfc_rle_coding_column_func error! field_id: %d field_name: %s column_id: %d\n", pcur_field_desc->id, pcur_field_desc->name, j);
					return -1;
				}
			}
			DbgPrint("\n");
		}
	}
	return 0;
}


int nfc_rle_coding_column(COLUMN_DESC* pcol_desc, int column_id)
{
	char* input_buffer = pcol_desc->in_ptr[column_id];
	int srcSize = pcol_desc->in_len[column_id];
	char* output_buffer = malloc(srcSize * 3);
	if (output_buffer == NULL)
	{
		printf("Allocate memory for output_buffer error! nfc_rle_coding_column_func!\n");
		return -1;
	}

	//UTIL_writeTempFile(input_buffer, srcSize);

	//
	int dstSize;
	if (rle_coding_data(input_buffer, srcSize, output_buffer, &dstSize))
	{
		printf("rle_coding_data error!\n");
        return -1;		
	}

	//
	//UTIL_writeTempFile(output_buffer, dstSize);

	//
	//free(input_buffer);
	pcol_desc->in_ptr[column_id] = output_buffer;
	pcol_desc->in_len[column_id] = dstSize;
	DbgPrint("nfc_rle_coding_column===column_id: %d srcSize %d dstSize: %d\n", column_id, srcSize, dstSize);
	
	return 0;
}



int nfc_rle_decoding(FIELD_DESC* pfield_desc, int field_num)
{
	FIELD_DESC* pcur_field_desc;
	int total_len;
	for (int i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		if (pcur_field_desc->rle_coding == TRUE)
		{
			DbgPrint("nfc_rle_decoding_column_func! field_id: %d\n", pcur_field_desc->id);
			total_len = pcur_field_desc->n_records;
			for (int j = 0; j < pcur_field_desc->width; j++)
			{
				if (nfc_rle_decoding_column(&(pcur_field_desc->col_desc), j, total_len))
				{
					//printf("nfc_rle_decoding_column_func error! field_id: %d field_name: %s column_id: %d\n", pcur_field_desc->id, pcur_field_desc->name, j);
					return -1;
				}
			}
			DbgPrint("\n");
		}
	}
	DbgPrint("nfc_rle_decoding ok!\n");
	return 0;
}


int nfc_rle_decoding_column(COLUMN_DESC* pcol_desc, int column_id, int total_len)
{
	char* input_buffer = pcol_desc->out_ptr[column_id];
	int srcSize = pcol_desc->out_len[column_id];
	int dstSize = total_len * 2;
	DbgPrint("output_buffer==dstSize: %d\n", dstSize);
	char* output_buffer = malloc(dstSize);
	//char* output_buffer = malloc(srcSize * 5);
	if (output_buffer == NULL)
	{
		printf("Allocate memory for output_buffer error! nfc_rle_decoding_column_func!\n");
		return -1;
	}

	//

	if (rle_decoding_data(input_buffer, srcSize, output_buffer, &dstSize))
	{
		printf("rle_decoding_data_func error!\n");
        return -1;		
	}

	//
	//free(input_buffer);
	pcol_desc->out_ptr[column_id] = output_buffer;
	pcol_desc->out_len[column_id] = dstSize;
	DbgPrint("nfc_rle_decoding_column===column_id: %d srcSize %d dstSize: %d\n", column_id, srcSize, dstSize);
	return 0;
}




