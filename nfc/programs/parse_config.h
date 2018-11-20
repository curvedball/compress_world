
/*
  parse_config.h

*/

#ifndef _PARSE_CONFIG_INC
#define _PARSE_CONFIG_INC

#include "debug.h"


typedef struct _FIELD_DESC
{
	char name[30];
	int width;
	bool reverse_coding;
	bool delta_coding;
	bool deltaofdelta_coding;
	char* data_ptr;
	int data_len;
	char* control_ptr; //If reverse_coding is enabled, then control_bits are saved in this buffer.
}FIELD_DESC, *PFIELD_DESC;


int ParseConfigFile(FIELD_DESC* pfield_desc, int* pfield_num);
int GetCellLength(FIELD_DESC* pfield_desc, int field_num);


#endif



