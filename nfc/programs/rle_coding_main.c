
/*
	rle_coding_main.c
*/


#include "rle_coding.h"




#if 0
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
#endif




int rle_coding_my_file(char* input_filename)
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
	fclose(srcFile);
	

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
	sprintf(o_filename, "%s.rle", input_filename);
	
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

	DbgPrint("rle_coding===input_filename: %s srcSize: %d dstSize: %d\n", input_filename, srcSize, dstSize);
	if (UTIL_writeFile(dstFile, out_buffer, dstSize))
	{
		return -1;
	}
	fclose(dstFile);

	return 0;
}


int rle_decoding_my_file(char* input_filename)
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
	fclose(srcFile);
	

	//============================================================
	int dstSize = srcSize * 20;
	DbgPrint("out_buffer dstSize: %d\n", dstSize);
	char* out_buffer = malloc(dstSize);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }


	if (rle_decoding_data(input_buffer, srcSize, out_buffer, &dstSize))
	{
		printf("rle_decoding_data error!\n");
        return -1;		
	}

	//============================================================
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	sprintf(o_filename, "%s.d", input_filename);
	
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

	DbgPrint("rle_decoding===input_filename: %s srcSize: %d dstSize: %d\n", input_filename, srcSize, dstSize);
	if (UTIL_writeFile(dstFile, out_buffer, dstSize))
	{
		return -1;
	}
	fclose(dstFile);
	return 0;
}



int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program (c or d) input_filename\n");
		return -1;
	}

	if (memcmp(argv[1], "c", 1) == 0)
	{
		if (rle_coding_my_file(argv[2]))
		{
			printf("rle_coding_column error!\n");
			return -1;
		}
	}
	else if (memcmp(argv[1], "d", 1) == 0)
	{
		if (rle_decoding_my_file(argv[2]))
		{
			printf("rle_coding_column error!\n");
			return -1;
		}
	}
	else
	{
		printf("Usage: ./program (c or d) input_filename\n");
		return -1;		
	}

	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}



