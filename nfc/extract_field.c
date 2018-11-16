
/*
	extract_field.c
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"
#include "parse_config.h"


int parse_netflow_v5_field_data(char* in, int in_len, char* out_buffer_table[], int* out_size_arr)
{
	DbgPrint("parse_netflow_v5_field_data==in_len: %d\n", in_len);
	
	char* ptr = in;
	char* ptr_end = in + in_len;
	int i;
	int j;

	char* temp_ptr[NETFLOW_V5_CELL_LEN];
	for (i = 0; i < NETFLOW_V5_CELL_LEN; i++)
	{
		temp_ptr[i] = out_buffer_table[i];
	}

	while (ptr < ptr_end)
	{
		for (i = 0; i < NETFLOW_V5_CELL_LEN; i++)
		{
			for (j = 0; j < v5_field_width[i]; j++)
			{
				*(temp_ptr[i] + j) = *ptr++;
			}	
			out_size_arr[i] += v5_field_width[i];
			temp_ptr[i] += v5_field_width[i];
		}
	}

	return 0;
}



int extract_netflow_v5_field(char* input_filename, char* output_filename_table[])
{
	FILE* srcFile;
    int result;

    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	//
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < NETFLOW_V5_CELL_LEN)
	{
		printf("Too small of this file! %s size: %d\n", input_filename, srcSize);
		return -1;
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

	int nRecords = srcSize / NETFLOW_V5_CELL_LEN;
	char* output_buffer_table[V5_FIELD_NUM];
	int output_buffer_table_size[V5_FIELD_NUM];
	
	int i;
	char* ptr;
	for (i = 0; i < V5_FIELD_NUM; i++)
	{
		ptr = malloc(nRecords * v5_field_width[i]);
		if (!ptr)
		{
			printf("Allocate memory for ptr error!\n");
			return -1;
		}
		output_buffer_table[i] = ptr;
		output_buffer_table_size[i] = 0; //zb: init
	}


	if (parse_netflow_v5_field_data(input_buffer, srcSize, output_buffer_table, output_buffer_table_size))
	{
		printf("parse_netflow_v5_field_data error!\n");		
		return -1;
	}

	FILE* dstFile;
	for (i = 0; i < V5_FIELD_NUM; i++)
	{
		dstFile = FIO_openDstFile(output_filename_table[i]);
		if (UTIL_writeFile(dstFile, output_buffer_table[i], output_buffer_table_size[i]))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);
		
	return 0;
}



//-----------------------------------------------------------------------------


int parse_netflow_v9_field_data(char* in, int in_len, char* out_buffer_table[], int* out_size_arr)
{
	DbgPrint("parse_netflow_v5_field_data==in_len: %d\n", in_len);
	
	char* ptr = in;
	char* ptr_end = in + in_len;
	int i;
	int j;

	char* temp_ptr[NETFLOW_V9_CELL_LEN];
	for (i = 0; i < NETFLOW_V9_CELL_LEN; i++)
	{
		temp_ptr[i] = out_buffer_table[i];
	}

	while (ptr < ptr_end)
	{
		for (i = 0; i < NETFLOW_V9_CELL_LEN; i++)
		{
			for (j = 0; j < v9_field_width[i]; j++)
			{
				*(temp_ptr[i] + j) = *ptr++;
			}	
			out_size_arr[i] += v9_field_width[i];
			temp_ptr[i] += v9_field_width[i];
		}
	}

	return 0;
}



int extract_netflow_v9_field(char* input_filename, char* output_filename_table[])
{
	FILE* srcFile;
    int result;

    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	//
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < NETFLOW_V9_CELL_LEN)
	{
		printf("Too small of this file! %s size: %d\n", input_filename, srcSize);
		return -1;
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

	int nRecords = srcSize / NETFLOW_V9_CELL_LEN;
	char* output_buffer_table[V9_FIELD_NUM];
	int output_buffer_table_size[V9_FIELD_NUM];
	
	int i;
	char* ptr;
	for (i = 0; i < V9_FIELD_NUM; i++)
	{
		ptr = malloc(nRecords * v9_field_width[i]);
		if (!ptr)
		{
			printf("Allocate memory for ptr error!\n");
			return -1;
		}
		output_buffer_table[i] = ptr;
		output_buffer_table_size[i] = 0; //zb: init
	}


	if (parse_netflow_v9_field_data(input_buffer, srcSize, output_buffer_table, output_buffer_table_size))
	{
		printf("parse_netflow_v5_field_data error!\n");		
		return -1;
	}

	FILE* dstFile;
	for (i = 0; i < V9_FIELD_NUM; i++)
	{
		dstFile = FIO_openDstFile(output_filename_table[i]);
		if (UTIL_writeFile(dstFile, output_buffer_table[i], output_buffer_table_size[i]))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);
		
	return 0;
}



int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename netflow_version\n");
		return -1;
	}

	int netflow_version = atoi(argv[2]);
	if (netflow_version < 0 || (netflow_version != 5 && netflow_version != 9))
	{
		printf("Invalid netflow_proto_num! Must be 5 or 9! netflow_version: %d\n", netflow_version);
		return -1;
	}


	if (netflow_version == 5)
	{
		//Validate file
		int srcSize = UTIL_getFileSize(argv[1]);
		if (srcSize % NETFLOW_V5_CELL_LEN != 0)
		{
			printf("File size %d is not 48 * n. Maybe not a regular netflow v5 file.\n", srcSize);
			return -1;
		}

		char input_filename[MAX_PATHNAME_LEN];
		char* output_filename_table[V5_FIELD_NUM];
		memset(input_filename, 0, MAX_PATHNAME_LEN);
		int len = strlen(argv[1]);
		memcpy(input_filename, argv[1], len);
		

		int i;
		char* ptr;
		for (i = 0; i < V5_FIELD_NUM; i++)
		{
			ptr = malloc(MAX_PATHNAME_LEN);
			if (!ptr)
			{
				printf("Allocate memory error!\n");
				return -1;
			}
			output_filename_table[i] = ptr;
			memset(ptr, 0, MAX_PATHNAME_LEN);
			sprintf(ptr, "%s_Field%.2d", input_filename, i);
		}

		//
		extract_netflow_v5_field(input_filename, output_filename_table);
	}
	else
	{
		//Validate file
		int srcSize = UTIL_getFileSize(argv[1]);
		if (srcSize % NETFLOW_V9_CELL_LEN != 0)
		{
			printf("File size %d is not NETFLOW_V9_CELL_LEN * n. Maybe not a regular netflow v9 file.\n", srcSize);
			return -1;
		}

		char input_filename[MAX_PATHNAME_LEN];
		char* output_filename_table[V9_FIELD_NUM];
		memset(input_filename, 0, MAX_PATHNAME_LEN);
		int len = strlen(argv[1]);
		memcpy(input_filename, argv[1], len);

		int i;
		char* ptr;
		for (i = 0; i < V9_FIELD_NUM; i++)
		{
			ptr = malloc(MAX_PATHNAME_LEN);
			if (!ptr)
			{
				printf("Allocate memory error!\n");
				return -1;
			}
			output_filename_table[i] = ptr;
			memset(ptr, 0, MAX_PATHNAME_LEN);
			sprintf(ptr, "%s_Field%.2d", input_filename, i);
		}

		//
		extract_netflow_v9_field(input_filename, output_filename_table);		
	}


	
	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}


int nfc_extract_field(char* input_filename, FIELD_DESC* pfield_desc, int field_num)
{
	FILE* srcFile;
    int result;

    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	//
	int cell_length = GetCellLength(pfield_desc, field_num);
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < cell_length)
	{
		printf("Too small of this file! %s size: %d cell_length: %d\n", input_filename, srcSize, cell_length);
		return -1;
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

	//
	int nRecords = srcSize / cell_length;
	char* output_buffer_table[V9_FIELD_NUM];
	int output_buffer_table_size[V9_FIELD_NUM];
	
	int i;
	char* ptr;
	for (i = 0; i < V9_FIELD_NUM; i++)
	{
		ptr = malloc(nRecords * v9_field_width[i]);
		if (!ptr)
		{
			printf("Allocate memory for ptr error!\n");
			return -1;
		}
		output_buffer_table[i] = ptr;
		output_buffer_table_size[i] = 0; //zb: init
	}


	if (parse_netflow_v9_field_data(input_buffer, srcSize, output_buffer_table, output_buffer_table_size))
	{
		printf("parse_netflow_v5_field_data error!\n");		
		return -1;
	}

	FILE* dstFile;
	for (i = 0; i < V9_FIELD_NUM; i++)
	{
		dstFile = FIO_openDstFile(output_filename_table[i]);
		if (UTIL_writeFile(dstFile, output_buffer_table[i], output_buffer_table_size[i]))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);
		
	return 0;
}




