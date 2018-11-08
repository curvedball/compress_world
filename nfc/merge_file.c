
/*
	merge_file.c
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"



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




int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 4)
	{
		printf("Usage: ./program input_filename column_count file_extension_name\n");
		return -1;
	}


	int column_count = atoi(argv[2]);
	if (column_count == 0)
	{
		printf("Convert column_count to int error! column_count: %s.\n", argv[2]);
		return -1;
	}


	char output_filename[MAX_PATHNAME_LEN];
	sprintf(output_filename, "%s_merge.zst", argv[1]);
	
	FILE* dstFile = FIO_openDstFile(output_filename);
	if (!dstFile)
	{
		printf("dstFile error!\n");
    	return -1;
	}


	char input_filename[MAX_PATHNAME_LEN];
	for (int i = 0; i < column_count; i++)
	{
		sprintf(input_filename, "%s%.2d.%s", argv[1], i, argv[3]);
		if (UTIL_isRegularFile(input_filename))
		{
			if (merge_input_file(input_filename, dstFile))
			{
				printf("merge_cell_file error!\n");
				return -1;
			}
		}
	}

	fclose(dstFile);


	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}




