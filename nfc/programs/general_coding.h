
/*
	general_coding.h
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"
#include "parse_config.h"


int need_to_delete_original_file();

int general_coding_file(char* input_filename, char* generic_compress_program_name, int removeFlag);


//
int nfc_general_coding(FIELD_DESC* pfield_desc, int field_num);
int nfc_general_coding_field(FIELD_DESC* pfield_desc);
int nfc_general_coding_column(COLUMN_DESC* pcol_desc, int column_id);



int nfc_general_decoding(FIELD_DESC* pfield_desc, int field_num);
int nfc_general_decoding_field(FIELD_DESC* pfield_desc);
int nfc_general_decoding_column(int n_records, COLUMN_DESC* pcol_desc, int column_id);


