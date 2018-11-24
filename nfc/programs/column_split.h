
/*
	column_split.h
*/


#include "debug.h"
#include "file_op.h"
#include "parse_config.h"

int extract_netflow_v5_field_column(char* input_filename, int width, char* base_filename, int column_id);
int extract_netflow_v9_field_column(char* input_filename, int width, char* base_filename, int column_id);


int nfc_column_split(FIELD_DESC* pfield_desc, int field_num);
int nfc_split_one_field(FIELD_DESC* pfield_desc, int* pcur_column_id);





