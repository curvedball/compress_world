/*
	extract_flow.c
*/


#include "extract_flow.h"


int parse_netflow_v5_data(char* in, int in_len, char* out, int* pout_len)
{
	char* start = in;
	char* ptr = in;
	V5_head* head_ptr;
	int count;
	int block_len;
	int out_pos = 0;
	int magic;
	DbgPrint("parse_netflow_v5_data==in_len: %d\n", in_len);
	while (1)
	{
		if (ptr - start >= in_len || ptr - start + NETFLOW_V5_HEAD_LEN > in_len)
		{
			break;
		}

		//
		head_ptr = (V5_head*)ptr;
		magic = ntohl(head_ptr->magic);
		count = ntohs(head_ptr->count);
		if (magic != UDP_V5_MAGIC)
		{
			printf("parse_netflow_v5_data===error! magic: %.8x UDP_V5_MAGIC: %.8x\n", magic, UDP_V5_MAGIC);
			return -1;
		}

		DbgPrint("parse_netflow_v5_data===count: %d\n", count);
		if (count > 0)
		{
			ptr += NETFLOW_V5_HEAD_LEN;
			block_len = NETFLOW_V5_CELL_LEN * count;

			//
			if (ptr - start +  block_len > in_len)
			{
				printf("Error! Not enough netflow records in this block! count: %d\n", count);
				return -1;
			}

			//
			memcpy(out + out_pos, ptr, block_len);
			out_pos += block_len;

			//
			ptr += block_len;
			DbgPrint("parse_netflow_v5_data===block_len: %d\n", block_len);
		}
		else
		{
			break;
		}
	}

	*pout_len = out_pos;

	return 0;
}




int extract_netflow_v5_record(char* input_filename, char* output_filename)
{
	FILE* srcFile;
    int result;

    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored \n", input_filename);
        return -1;
    }

    srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	//
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < 28)
	{
		printf("No netflows in this file! %s\n", input_filename);
		return -1;
	}


	char* input_buffer = malloc(srcSize);
	char* output_buffer = malloc(srcSize);

	//
	if (UTIL_readFile(srcFile, input_buffer, srcSize) < 0)
	{
		return -1;
	}

	//
	char* ptr = input_buffer;
	int outSize;
	if (parse_netflow_v5_data(ptr, srcSize, output_buffer, &outSize))
	{
		printf("parse_netflow_data v5 error!\n");		
		return -1;
	}

	//
	FILE* dstFile = FIO_openDstFile(output_filename);
    if (dstFile == NULL)
    {
        return -1;
    }

	if (UTIL_writeFile(dstFile, output_buffer, outSize))
	{
		return -1;
	}

	fclose(dstFile);
	fclose(srcFile);
		
	return 0;
}



int parse_netflow_v9_data(char* in, int in_len, char* out, int* pout_len)
{
	char* ptr = in;
	char* ptr_end = ptr + in_len;
	V9_head* head_ptr;
	int flow_count;
	U32 magic;
	U32 data_len;
	U32 block_len;
	U32 tail_len;
	U32 padding_len;

	//
	char* output_ptr = out;
	U32 output_pos = 0;


	//
	while (ptr < ptr_end)
	{
		head_ptr = (V9_head*)ptr;
		magic = ntohl(head_ptr->magic);
		if (magic != UDP_V9_MAGIC && magic != UDP_V9_MAGIC2 && magic != UDP_V9_MAGIC3)
		{
			printf("Error! Invalid magic header! CurrentPos: %ld magic: 0x%.8X\n", ptr - in, magic);
			return -1;
		}

		if (magic == UDP_V9_MAGIC2)
		{
			ptr += UDP_V9_MAGIC2_BLOCK_LEN;
			continue;
		}

		if (magic == UDP_V9_MAGIC3)
		{
			ptr += UDP_V9_MAGIC3_BLOCK_LEN;
			continue;
		}		

		//
		flow_count = ntohs(head_ptr->count);
		if (flow_count != 22)
		{
			printf("Current Pos: %ld\n", ptr - in);
		}

		//
		data_len = NETFLOW_V9_CELL_LEN * flow_count;
		block_len = NETFLOW_V9_HEAD_LEN + data_len;
		tail_len = block_len % 4;
		padding_len =  tail_len ?  4 - tail_len : 0;
		block_len += padding_len;

		//
		memcpy(output_ptr, ptr + NETFLOW_V9_HEAD_LEN, data_len);

		output_pos += data_len;
		output_ptr += data_len;

		//
		//printf("flow_count: %d block_len: %d data_len: %d NETFLOW_V9_HEAD_LEN: %d NETFLOW_V9_CELL_LEN: %d\n", flow_count, block_len, data_len, NETFLOW_V9_HEAD_LEN, NETFLOW_V9_CELL_LEN);
		ptr += block_len;
	}

	*pout_len = output_pos;
	return 0;
}



int extract_netflow_v9_record(char* input_filename, char* output_filename)
{
	FILE* srcFile;
    int result;

    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored \n", input_filename);
        return -1;
    }

    srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	//
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < 28)
	{
		printf("No netflows in this file! %s\n", input_filename);
		return -1;
	}


	char* input_buffer = malloc(srcSize);
	if (!input_buffer)
	{
		printf("Allocate input_buffer error!\n");		
		return -1;		
	}

	char* output_buffer = malloc(srcSize);
	if (!output_buffer)
	{
		printf("Allocate output_buffer error! srcSize: %d\n", srcSize);		
		return -1;		
	}

	//
	if (UTIL_readFile(srcFile, input_buffer, srcSize) < 0)
	{
		return -1;
	}

	memcpy(output_buffer, input_buffer, srcSize);

	
	//
	int outSize;
	if (parse_netflow_v9_data(input_buffer, srcSize, output_buffer, &outSize))
	{
		printf("parse_netflow_data v9 error!\n");
		return -1;
	}

	//
	FILE* dstFile = FIO_openDstFile(output_filename);
    if (dstFile == NULL)
    {
        return -1;
    }

	if (UTIL_writeFile(dstFile, output_buffer, outSize))
	{
		return -1;
	}

	fclose(dstFile);


	fclose(srcFile);
		
	return 0;
}




