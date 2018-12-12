
/*
	delta_coding.h
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"
#include "netflow_v9.h"
#include "parse_config.h"



int delta_coding(char* input_filename, int width);
int delta_coding_advanced(char* input_filename, int width);



//
int nfc_delta_coding(FIELD_DESC* pfield_desc, int field_num);
int nfc_delta_coding_field(FIELD_DESC* pfield_desc);
int nfc_set_delta_value(char* buf, char* data, int width);
int nfc_calc_nonuniformity(char* data, int len, U64* pvalue);




int nfc_delta_decoding(FIELD_DESC* pfield_desc, int field_num);
int nfc_delta_decoding_field(FIELD_DESC* pfield_desc);










