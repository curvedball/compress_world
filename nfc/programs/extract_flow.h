
/*
	extract_flow.h
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"




int parse_netflow_v5_data(char* in, int in_len, char* out, int* pout_len);
int extract_netflow_v5_record(char* input_filename, char* output_filename);


int parse_netflow_v9_data(char* in, int in_len, char* out, int* pout_len);
int extract_netflow_v9_record(char* input_filename, char* output_filename);



