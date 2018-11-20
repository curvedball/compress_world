/*
	extract_flow_main.c
*/


#include "extract_flow.h"



int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program input_filename netflow_proto_num\n");
		return -1;
	}

	int netflow_version = atoi(argv[2]);
	if (netflow_version < 0 || (netflow_version != 5 && netflow_version != 9))
	{
		printf("Invalid netflow_proto_num! Must be 5 or 9! netflow_version: %d\n", netflow_version);
		return -1;
	}

	char input_filename[MAX_PATHNAME_LEN];
	char output_filename[MAX_PATHNAME_LEN];	
	memset(input_filename, 0, MAX_PATHNAME_LEN);
	memset(output_filename, 0, MAX_PATHNAME_LEN);

	int len = strlen(argv[1]);
	memcpy(input_filename, argv[1], len);
	memcpy(output_filename, argv[1], len);
	memcpy(output_filename + len, "_Record", 7);

	//
	if (netflow_version == 5)
	{
		extract_netflow_v5_record(input_filename, output_filename);		
	}
	else if (netflow_version == 9)
	{
		extract_netflow_v9_record(input_filename, output_filename);			
	}
	else
	{
		
	}


	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}




