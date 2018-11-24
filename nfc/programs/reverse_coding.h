
/*
	reverse_coding.h
*/

#ifndef _REVERSE_CODING_INC
#define _REVERSE_CODING_INC


#include "debug.h"
#include "file_op.h"
#include "parse_config.h"


int reverse_coding_netflow_field_u32(char* in1, char* in2, int nRecords, char* control_buffer, int* pcontrol_size, char* out1, char* out2, int* pout2_size);
int reverse_coding_netflow_field_u16(char* in1, char* in2, int nRecords, char* control_buffer, int* pcontrol_size, char* out1, char* out2, int* pout2_size);

int reverse_coding_netflow_field_file(char* input_filename1, char* input_filename2, char* output_filename1, char* output_filename2, int width, char* output_filename_control);




//
int nfc_reverse_coding(FIELD_DESC* pfield_desc, int field_num);
int nfc_reverse_coding_field(FIELD_DESC* pfield_desc1, FIELD_DESC* pfield_desc2);



//
int nfc_reverse_decoding(FIELD_DESC* pfield_desc, int field_num);
int nfc_reverse_decoding_field(FIELD_DESC* pfield_desc1, FIELD_DESC* pfield_desc2);
int nfc_reverse_decoding_netflow_field_u32(U32* in1, U32* in2, int nRecords, char* control_buffer, int control_size, U32* out2);
int nfc_reverse_decoding_netflow_field_u16(U16* in1, U16* in2, int nRecords, char* control_buffer, int control_size, U16* out2);




#endif


