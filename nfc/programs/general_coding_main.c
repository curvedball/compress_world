
/*
	general_coding_main.c
*/


#include "general_coding.h"


int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 4)
	{
		printf("Usage: ./program input_filename column_count generic_compress_program_name\n");
		return -1;
	}


	int column_count = atoi(argv[2]);
	if (column_count == 0)
	{
		printf("Convert column_count to int error! column_count: %s.\n", argv[2]);
		return -1;
	}

	char* generic_compress_program_name = argv[3];
	if (!UTIL_isRegularFile(generic_compress_program_name))
	{
		printf("%s does not exist!\n", generic_compress_program_name);
		return -1;
	}


	int removeFlag = need_to_delete_original_file();
	printf("Delete Flag: %d\n", removeFlag);

	//
	char input_filename[MAX_PATHNAME_LEN];
	int i;
	for (i = 0; i < column_count; i++)
	{
		sprintf(input_filename, "%s%.2d", argv[1], i);
		if (UTIL_isRegularFile(input_filename))
		{
			if (general_coding_file(input_filename, generic_compress_program_name, removeFlag))
			{
				printf("general_coding_file error!\n");
				return -1;
			}
		}
	}

	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}




