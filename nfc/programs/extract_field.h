
/*
	extract_field.h
*/

#ifndef _EXTRACT_FIELD_INC
#define _EXTRACT_FIELD_INC

#include "debug.h"
#include "file_op.h"
#include "parse_config.h"


int parse_netflow_v5_field_data(char* in, int in_len, char* out_buffer_table[], int* out_size_arr);

int extract_netflow_v5_field(char* input_filename, char* output_filename_table[]);


int parse_netflow_v9_field_data(char* in, int in_len, char* out_buffer_table[], int* out_size_arr);

int extract_netflow_v9_field(char* input_filename, char* output_filename_table[]);



int nfc_extract_field(char* input_filename, FIELD_DESC* pfield_desc, int field_num);

int nfc_restore_field(FIELD_DESC* pfield_desc, int field_num);
int nfc_restore_one_field(FIELD_DESC* pfield_desc);


#endif



