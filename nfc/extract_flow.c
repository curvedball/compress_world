/*
	extract_flow.c
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"





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
	while (1)
	{
		head_ptr = (V9_head*)ptr;
		magic = ntohl(head_ptr->magic);
		if (magic != UDP_V9_MAGIC)
		{
			printf("Error! Invalid magic header! CurrentPos: %d magic: 0x%.8X\n", ptr - in, magic);
			return -1;
		}
		flow_count = ntohs(head_ptr->count);
		if (flow_count != 22)
		{
			printf("Current Pos: %d\n", ptr - in);
		}

		ptr += 888;
		
	}
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
	char* output_buffer = malloc(srcSize);

	//
	if (UTIL_readFile(srcFile, input_buffer, srcSize) < 0)
	{
		return -1;
	}

	//
	char* ptr = input_buffer;
	int outSize;
	if (parse_netflow_v9_data(ptr, srcSize, output_buffer, &outSize))
	{
		printf("parse_netflow_data v9 error!\n");		
		return -1;
	}


#if 0
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
#endif

	
	fclose(srcFile);
		
	return 0;
}


int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename netflow_proto_num\n");
		return -1;
	}

	int netflow_version = atoi(argv[2]);
	if (netflow_version < 0 || (netflow_version != 5 && netflow_version != 9))
	{
		printf("Invalid netflow_proto_num! Must be 5 or 9! netflow_version: %d\n", netflow_version);
		return -1;
	}

	char input_filename[MAX_PATHNAME_LEN];
	char output_filename[MAX_PATHNAME_LEN];	
	memset(input_filename, 0, MAX_PATHNAME_LEN);
	memset(output_filename, 0, MAX_PATHNAME_LEN);

	int len = strlen(argv[1]);
	memcpy(input_filename, argv[1], len);
	memcpy(output_filename, argv[1], len);
	memcpy(output_filename + len, "_Record", 7);

	//
	if (netflow_version == 5)
	{
		extract_netflow_v5_record(input_filename, output_filename);		
	}
	else if (netflow_version == 9)
	{
		extract_netflow_v9_record(input_filename, output_filename);				
	}
	else
	{
		
	}


	
	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}



