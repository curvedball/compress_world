
/*
	general_coding.c
*/


#include "general_coding.h"


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
		printf("Execute system cmd error! cmd_buffer: %s\n", cmd_buffer);
        return -1;
	}

	//
	if (removeFlag && strstr(input_filename, ".flows_Record"))
	{
		sprintf(cmd_buffer, "rm -f %s", input_filename);
		status = system(cmd_buffer);
		if (status == -1)
		{
			printf("Execute system cmd error! cmd_buffer: %s\n", cmd_buffer);
			return -1;
		}

	}

	return 0;
}





