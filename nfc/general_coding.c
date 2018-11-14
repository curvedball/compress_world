
/*
	general_coding.c
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"


int need_to_delete_original_file()
{
	printf("Do you want to remove the original column_files? (y/N)");
	int ch = getchar();
	if ((ch != 'Y') && (ch != 'y'))
    {
        return 0;
    }

    /* flush rest of input line */
    while ((ch != EOF) && (ch != '\n'))
    {
        ch = getchar();
    }

	return 1;
}


int general_coding_file(char* input_filename, char* generic_compress_program_name, int removeFlag)
{
    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

	char cmd_buffer[MAX_PATHNAME_LEN];
	sprintf(cmd_buffer, "%s %s", generic_compress_program_name, input_filename);
	pid_t status = system(cmd_buffer);
	if (status == -1)
	{
		printf("Execute system cmd error!\n");
        return -1;
	}

	//
	if (removeFlag && strstr(input_filename, ".flows_Record"))
	{
		sprintf(cmd_buffer, "rm -f %s", input_filename);
		system(cmd_buffer);	
	}

	return 0;
}




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




