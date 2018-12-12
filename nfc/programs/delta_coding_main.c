
/*
	delta_coding_main.c
*/


#include "delta_coding.h"



#if 1
int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename width(bytes)\n");
		return -1;
	}


	int width = atoi(argv[2]);
	if (width <= 0 || (width != 1 && width != 2 && width != 4 && width != 8))
	{
		printf("Invalid width! Width must be 1, 2, 4 or 8! width: %d.\n", width);
		return -1;
	}


	char input_filename[MAX_PATHNAME_LEN];
	sprintf(input_filename, "%s", argv[1]);
	if (UTIL_isRegularFile(input_filename))
	{
		if (width == 1)
		{
		}
		else if (width == 2)
		{
			if (delta_coding_u16(input_filename, width))
			{
				printf("delta_coding_column error!\n");
				return -1;
			}
		}
		else if (width == 4)
		{
			
		}
		else if (width == 8)
		{
			
		}
		else
		{
			
		}
		
		
	}


	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}
#endif


#if 0
int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename width(bytes)\n");
		return -1;
	}


	int width = atoi(argv[2]);
	if (width <= 0 || (width != 1 && width != 2 && width != 4 && width != 8))
	{
		printf("Invalid width! Width must be 1, 2, 4 or 8! width: %d.\n", width);
		return -1;
	}


	char input_filename[MAX_PATHNAME_LEN];
	sprintf(input_filename, "%s", argv[1]);
	if (UTIL_isRegularFile(input_filename))
	{
		if (delta_coding_advanced(input_filename, width))
		{
			printf("delta_coding_column error!\n");
			return -1;
		}
	}


	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}
#endif






