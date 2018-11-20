
/*
	column_split.c
*/


#include "column_split.h"


int extract_netflow_v5_field_column(char* input_filename, int width, char* base_filename, int column_id)
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

	//
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < NETFLOW_V5_CELL_LEN)
	{
		printf("Too small of this file! %s size: %d\n", input_filename, srcSize);
		return -1;
	}

	//
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

	DbgPrint("extract_netflow_v5_field_column...\n");


	//============================================================	
	int nRecords = srcSize / width;
	char* ptr_in_end = input_buffer + srcSize;

	//
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	
	char* out_buffer = malloc(nRecords);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }


	DbgPrint("extract_netflow_v5_field_column===nRecords: %d\n", nRecords);

	

	//============================================================
	FILE* dstFile;
	int i;
	char* ptr_in;
	char* ptr_out;
	for (i = 0; i < width; i++)
	{
		sprintf(o_filename, "%s_column%.2d", base_filename, column_id + i);
		ptr_in = input_buffer + i;
		ptr_out = out_buffer;
		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in;
			ptr_in += width;
			ptr_out++;
		}

		//DbgPrint("o_filename: %s\n", o_filename);
		dstFile = FIO_openDstFile(o_filename);
		if (!dstFile)
		{
			printf("dstFile error!\n");
        	return -1;
		}
		
		if (UTIL_writeFile(dstFile, out_buffer, nRecords))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);

	return 0;
}




int extract_netflow_v9_field_column(char* input_filename, int width, char* base_filename, int column_id)
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

	//
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < NETFLOW_V9_CELL_LEN)
	{
		printf("Too small of this file! %s size: %d\n", input_filename, srcSize);
		return -1;
	}

	//
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

	DbgPrint("extract_netflow_v9_field_column...\n");


	//============================================================	
	int nRecords = srcSize / width;
	char* ptr_in_end = input_buffer + srcSize;

	//
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	
	char* out_buffer = malloc(nRecords);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }


	DbgPrint("extract_netflow_v9_field_column===nRecords: %d\n", nRecords);

	

	//============================================================
	FILE* dstFile;
	int i;
	char* ptr_in;
	char* ptr_out;
	for (i = 0; i < width; i++)
	{
		sprintf(o_filename, "%s_column%.2d", base_filename, column_id + i);
		ptr_in = input_buffer + i;
		ptr_out = out_buffer;
		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in;
			ptr_in += width;
			ptr_out++;
		}

		//DbgPrint("o_filename: %s\n", o_filename);
		dstFile = FIO_openDstFile(o_filename);
		if (!dstFile)
		{
			printf("dstFile error!\n");
        	return -1;
		}
		
		if (UTIL_writeFile(dstFile, out_buffer, nRecords))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);

	return 0;
}





