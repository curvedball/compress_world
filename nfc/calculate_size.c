
/*
	calculate_size.c
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"


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


	char input_filename[MAX_PATHNAME_LEN];
	int i;
	U64 total_file_size = 0;
	U64 size;
	for (i = 0; i < column_count; i++)
	{
		sprintf(input_filename, "%s%.2d.%s", argv[1], i, argv[3]);
		if (UTIL_isRegularFile(input_filename))
		{
			size = UTIL_getFileSize(input_filename);
			if (size < 0)
			{
				printf("size error!\n");
				return -1;
			}

			total_file_size += size;
		}
	}
	DbgPrint("total_file_size: %ld\n", total_file_size);

	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}




