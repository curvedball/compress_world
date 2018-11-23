
/*
	merge_data.h
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"
#include "parse_config.h"



int merge_input_file(char* input_filename, FILE* dstFile);



int nfc_merge_data(char* input_filename, FIELD_DESC* pfield_desc, int field_num);




