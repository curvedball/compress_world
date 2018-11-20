
/*
	deltaofdelta_coding.c
*/


#include "deltaofdelta_coding.h"




int deltaofdelta_coding(char* input_filename, char* base_name, int column_id)
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
	if (srcSize == 0)
	{
		return 0;
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


	DbgPrint("delta_coding_column===input_filename: %s srcSize: %d\n", input_filename, srcSize);


	//============================Delta================================
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


	//===========================DeltaOfDelta=============================
	char* out_buffer2 = malloc(srcSize);
	if (out_buffer2 == NULL)
    {
		printf("Allocate out_buffer2 error!\n");
        return -1;
    }

	char* ptr1_end = out_buffer + srcSize;
	char* ptr1 = out_buffer;
	char* ptr2 = out_buffer2;

	*ptr2 = *ptr1;
	ptr1++;
	ptr2++;

	while (ptr1 < ptr1_end)
	{
		*ptr2 = *ptr1 - *(ptr1 - 1);
		ptr1++;
		ptr2++;
	}

	


	//============================================================
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	sprintf(o_filename, "%s_deltaofdelta%.2d", base_name, column_id);
	
	FILE* dstFile = FIO_openDstFile(o_filename);
	if (!dstFile)
	{
		printf("dstFile error!\n");
    	return -1;
	}
	
	if (UTIL_writeFile(dstFile, out_buffer2, srcSize))
	{
		return -1;
	}
	fclose(dstFile);

	//
	fclose(srcFile);

	return 0;
}




