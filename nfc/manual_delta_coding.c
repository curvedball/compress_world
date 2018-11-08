
/*
	manual_delta_coding.c
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"



int delta_coding_column(char* input_filename)
{
    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
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


	//============================================================	
	FILE* srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	if (UTIL_readFile(srcFile, input_buffer, srcSize) < 0)
	{
		return -1;
	}

	//
	fclose(srcFile);


	DbgPrint("delta_coding_column===input_filename: %s srcSize: %d\n", input_filename, srcSize);


	//============================================================
	char* out_buffer = malloc(srcSize);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }

	char* ptr_in_end = input_buffer + srcSize;
	char* ptr_in = input_buffer;
	char* ptr_out = out_buffer;

	*ptr_out = *ptr_in;
	ptr_in++;
	ptr_out++;	

	while (ptr_in < ptr_in_end)
	{
		*ptr_out = *ptr_in - *(ptr_in - 1);
		ptr_in++;
		ptr_out++;
	}


	//============================================================
	char* o_filename = input_filename;
	FILE* dstFile = FIO_openDstFile(o_filename);
	if (!dstFile)
	{
		printf("dstFile error!\n");
    	return -1;
	}
	
	if (UTIL_writeFile(dstFile, out_buffer, srcSize))
	{
		return -1;
	}
	fclose(dstFile);

	return 0;
}




int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 2)
	{
		printf("Usage: ./program input_filename\n");
		return -1;
	}

	if (delta_coding_column(argv[1]))
	{
		printf("delta_coding_column error!\n");
		return -1;
	}

	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}




