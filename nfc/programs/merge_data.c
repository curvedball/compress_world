
/*
	merge_data.c
*/


#include "merge_data.h"




int merge_input_file(char* input_filename, FILE* dstFile)
{
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

	if (UTIL_writeFile(dstFile, input_buffer, srcSize))
	{
		return -1;
	}

	fclose(srcFile);

	return 0;
}





