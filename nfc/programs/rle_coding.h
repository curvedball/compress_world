
/*
	rle_coding.h
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"
#include "parse_config.h"



void reset_data(char* buf, int len);
int  SetBBlockBit(char* bblock_head, int cur_pos);
int rle_coding_data(char* input_buffer, int input_size, char* output_buffer, int* poutput_size);
int rle_coding_file(char* input_filename, char* base_name, int column_id);





