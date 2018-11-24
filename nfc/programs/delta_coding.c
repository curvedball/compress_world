
/*
	delta_coding.c
*/


#include "delta_coding.h"


int delta_coding(char* input_filename, int width)
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
		printf("Error! FileSize is %d!\n", srcSize);	
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

	DbgPrint("delta_coding===input_filename: %s srcSize: %d\n", input_filename, srcSize);


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

	if (width == 1)
	{
		*ptr_out = *ptr_in;
		ptr_in++;
		ptr_out++;	

		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in - *(ptr_in - 1);
			ptr_in++;
			ptr_out++;
		}
	}
	else if (width == 2)
	{
		memcpy(ptr_out, ptr_in, 2);
		ptr_in += 2;
		ptr_out += 2;;	

		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in - *(ptr_in - 2);
			*(ptr_out + 1) = *(ptr_in + 1) - *(ptr_in - 1);

			ptr_in += 2;
			ptr_out += 2;
		}		
	}
	else if (width == 4)
	{
		memcpy(ptr_out, ptr_in, 4);
		ptr_in += 4;
		ptr_out += 4;;	

		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in - *(ptr_in - 4);
			*(ptr_out + 1) = *(ptr_in + 1) - *(ptr_in - 3);
			*(ptr_out + 2) = *(ptr_in + 2) - *(ptr_in - 2);
			*(ptr_out + 3) = *(ptr_in + 3) - *(ptr_in - 1);

			ptr_in += 4;
			ptr_out += 4;
		}
	}
	else if (width == 8)
	{
		memcpy(ptr_out, ptr_in, 8);
		ptr_in += 8;
		ptr_out += 8;;	

		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in - *(ptr_in - 8);
			*(ptr_out + 1) = *(ptr_in + 1) - *(ptr_in - 7);
			*(ptr_out + 2) = *(ptr_in + 2) - *(ptr_in - 6);
			*(ptr_out + 3) = *(ptr_in + 3) - *(ptr_in - 5);
			
			*(ptr_out + 4) = *(ptr_in + 4) - *(ptr_in - 4);
			*(ptr_out + 5) = *(ptr_in + 5) - *(ptr_in - 3);
			*(ptr_out + 6) = *(ptr_in + 6) - *(ptr_in - 2);
			*(ptr_out + 7) = *(ptr_in + 7) - *(ptr_in - 1);	

			ptr_in += 8;
			ptr_out += 8;
		}		
	}
	else
	{
		printf("Invalid width value! width: %d\n", width);
		return -1;
	}





	//============================================================
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	sprintf(o_filename, "%s_delta", input_filename);
	
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

	//
	fclose(srcFile);

	return 0;
}





int nfc_delta_decoding(FIELD_DESC* pfield_desc, int field_num)
{
	return 0;
}





