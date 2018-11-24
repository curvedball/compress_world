
/*
	merge_data.h
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"
#include "parse_config.h"



int merge_input_file(char* input_filename, FILE* dstFile);


#define HEAD_MAX_LEN 10240

int nfc_merge_data(char* input_filename, FIELD_DESC* pfield_desc, int field_num);



int nfc_find_field_column(int column_number, FIELD_DESC* pfield_desc, int field_num, FIELD_DESC** ppfield_desc, int* pcolumn_id);
int nfc_parse_compressed_file(char* input_filename, FIELD_DESC* pfield_desc, int* pfield_num);
int nfc_restore_compressed_file(char* input_filename, FIELD_DESC* pfield_desc, int field_num);





