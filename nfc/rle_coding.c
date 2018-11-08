
/*
	rle_coding.c
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"



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
		bblock_head[2] = bblock_head[2] | (1 << (cur_pos - 15));
	}
	else
	{
		bblock_head[3] = bblock_head[3] | (1 << (cur_pos - 23));
	}
	
	return 0;
}



int rle_coding_data(char* input_buffer, int input_size, char* output_buffer, int* poutput_size)
{
	char* ptr_in_end = input_buffer + input_size;
	char* ptr_in = input_buffer;	
	char* ptr_out_begin = output_buffer;	
	char* ptr_out = output_buffer;
	char bblock_head[4]; //zb: 4
	char bblock_data[64]; //zb: 32+32
	char cur_symbol;
	int  cur_pos = 0;
	int  repeat_count = 0;
	int  count_length = 0; //
	char* ptr_stub = ptr_in;
	int  tail_len;

	reset_data(bblock_head, 4);

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

		//DbgPrint("---cell---cur_symbol: %c\n", cur_symbol);

		//
		if (repeat_count > 1)
		{
			if (SetBBlockBit(bblock_head, cur_pos))
			{
				return -1;
			}
			bblock_data [32 + count_length] = repeat_count;
			count_length++;
		}

		//
		cur_pos++;
		repeat_count = 0;

		//
		if (cur_pos == 32)
		{
			if (ptr_in - ptr_stub < 37 + count_length)
			{
				DbgPrint("---Convert it to vBlock and Put it into output buffer---\n");
				*ptr_out = 0;
				memcpy(ptr_out + 1, ptr_stub, 32);
				ptr_out += 33;
				ptr_in = ptr_stub + 32;
				ptr_stub = ptr_in;
				
			}
			else
			{
				DbgPrint("---BBlock and Put it into output buffer---\n");
				*ptr_out = 1;
				memcpy(ptr_out + 1, bblock_head, 4);
				memcpy(ptr_out + 5, bblock_data, 32);
				memcpy(ptr_out + 37, bblock_data + 32, count_length);
				ptr_out += 37 + count_length;
				ptr_stub = ptr_in;
			}
			cur_pos = 0;
			count_length = 0;
		}
	}

	if (cur_pos != 0)
	{
		//The last block may be not full
		int valid_data_len = 1 + 4 + cur_pos + count_length;
		tail_len = ptr_in_end - ptr_stub;

		//
		if (valid_data_len < tail_len) //zb: still can be compressed!
		{
			*ptr_out = 1;
			memcpy(ptr_out + 1, bblock_head, 4);
			memcpy(ptr_out + 5, bblock_data, cur_pos);
			memcpy(ptr_out + 5 + cur_pos, bblock_data + 32, count_length);
			ptr_out += valid_data_len;		
		}
		else
		{
			*ptr_out = 0;
			memcpy(ptr_out + 1, ptr_stub, tail_len);
			ptr_out += tail_len + 1;
		}

	}

	int output_size = ptr_out - ptr_out_begin;
	*poutput_size = output_size;
	
	//DbgPrint("output_size: %d\n", output_size);
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




int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename column_count\n");
		return -1;
	}


	int column_count = atoi(argv[2]);
	if (column_count == 0)
	{
		printf("Convert column_count to int error! column_count: %s.\n", argv[2]);
		return -1;
	}


	char input_filename[MAX_PATHNAME_LEN];
	int i;
	for (i = 0; i < column_count; i++)
	{
		sprintf(input_filename, "%s%.2d", argv[1], i);
		if (UTIL_isRegularFile(input_filename))
		{
			if (rle_coding_file(input_filename, argv[1], i))
			{
				printf("rle_coding_column error!\n");
				return -1;
			}
		}
	}

	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}




