
/*
	deltaofdelta_coding_main.c
*/


#include "deltaofdelta_coding.h"



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
			if (deltaofdelta_coding(input_filename, argv[1], i))
			{
				printf("delta_coding_column error!\n");
				return -1;
			}
		}
	}

	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}




