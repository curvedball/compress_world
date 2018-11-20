
/*
	reverse_coding_main.c
*/


#include "reverse_coding.h"


int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 4)
	{
		printf("Usage: ./program input_filename_a input_filename_b width(bytes)\n");
		return -1;
	}

	int width = atoi(argv[3]);
	if (width == 0)
	{
		printf("Convert width to int error! width: %s.\n", argv[3]);
		return -1;
	}

	char* input_filename1 = argv[1];
	char* input_filename2 = argv[2];	



	char output_filename1[MAX_PATHNAME_LEN];
	char output_filename2[MAX_PATHNAME_LEN];	
	char output_filename_control[MAX_PATHNAME_LEN];	
	memset(output_filename1, 0, MAX_PATHNAME_LEN);
	memset(output_filename2, 0, MAX_PATHNAME_LEN);
	memset(output_filename_control, 0, MAX_PATHNAME_LEN);
	sprintf(output_filename1, "%s_r", input_filename1);
	sprintf(output_filename2, "%s_r", input_filename2);

	/*
	if (width == 4)
	{
		sprintf(output_filename1, "%s_r4", input_filename1);
		sprintf(output_filename2, "%s_r4", input_filename2);	
	}
	else if (width == 2)
	{
		sprintf(output_filename1, "%s_r2", input_filename1);
		sprintf(output_filename2, "%s_r2", input_filename2);			
	}
	else
	{
		printf("Invalid width: %d. Width must be 4 or 2 bytes!\n", width);
		return -1;		
	}
	*/

	sprintf(output_filename_control, "%s_control", input_filename1);


	//
	if (reverse_coding_netflow_field_file(input_filename1, input_filename2, output_filename1, output_filename2, width, output_filename_control))
	{
		return -1;
	}

	
	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}



