
/*
	extract_field_main.c
*/


#include "extract_field.h"


int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename netflow_version\n");
		return -1;
	}

	int netflow_version = atoi(argv[2]);
	if (netflow_version < 0 || (netflow_version != 5 && netflow_version != 9))
	{
		printf("Invalid netflow_proto_num! Must be 5 or 9! netflow_version: %d\n", netflow_version);
		return -1;
	}


	if (netflow_version == 5)
	{
		//Validate file
		int srcSize = UTIL_getFileSize(argv[1]);
		if (srcSize % NETFLOW_V5_CELL_LEN != 0)
		{
			printf("File size %d is not 48 * n. Maybe not a regular netflow v5 file.\n", srcSize);
			return -1;
		}

		char input_filename[MAX_PATHNAME_LEN];
		char* output_filename_table[V5_FIELD_NUM];
		memset(input_filename, 0, MAX_PATHNAME_LEN);
		int len = strlen(argv[1]);
		memcpy(input_filename, argv[1], len);
		

		int i;
		char* ptr;
		for (i = 0; i < V5_FIELD_NUM; i++)
		{
			ptr = malloc(MAX_PATHNAME_LEN);
			if (!ptr)
			{
				printf("Allocate memory error!\n");
				return -1;
			}
			output_filename_table[i] = ptr;
			memset(ptr, 0, MAX_PATHNAME_LEN);
			sprintf(ptr, "%s_Field%.2d", input_filename, i);
		}

		//
		extract_netflow_v5_field(input_filename, output_filename_table);
	}
	else
	{
		//Validate file
		int srcSize = UTIL_getFileSize(argv[1]);
		if (srcSize % NETFLOW_V9_CELL_LEN != 0)
		{
			printf("File size %d is not NETFLOW_V9_CELL_LEN * n. Maybe not a regular netflow v9 file.\n", srcSize);
			return -1;
		}

		char input_filename[MAX_PATHNAME_LEN];
		char* output_filename_table[V9_FIELD_NUM];
		memset(input_filename, 0, MAX_PATHNAME_LEN);
		int len = strlen(argv[1]);
		memcpy(input_filename, argv[1], len);

		int i;
		char* ptr;
		for (i = 0; i < V9_FIELD_NUM; i++)
		{
			ptr = malloc(MAX_PATHNAME_LEN);
			if (!ptr)
			{
				printf("Allocate memory error!\n");
				return -1;
			}
			output_filename_table[i] = ptr;
			memset(ptr, 0, MAX_PATHNAME_LEN);
			sprintf(ptr, "%s_Field%.2d", input_filename, i);
		}

		//
		extract_netflow_v9_field(input_filename, output_filename_table);		
	}


	
	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}





