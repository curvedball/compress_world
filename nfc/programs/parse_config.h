
/*
  parse_config.h

*/

#ifndef _PARSE_CONFIG_INC
#define _PARSE_CONFIG_INC

#include "debug.h"


typedef struct _COLUMN_DESC
{
	int column_count;	
	char* in_ptr[9];
	int in_len[9];
	char* out_ptr[9];
	int out_len[9];	
}COLUMN_DESC, *PCOLUMN_DESC;



typedef struct _FIELD_DESC
{
	int id;
	char name[30];
	int width;

	//
	bool reverse_coding;
	bool reverse_master;
	char reverse_number;
	char* control_ptr; //If reverse_coding is enabled, then control_bits are saved in this buffer. Its width is always 1 byte.
	int control_len;

	//
	bool delta_coding; //expressed by user!
	bool delta_check; //checked by the system!
	char* delta_ptr;
	int delta_len;
	
	
	bool deltaofdelta_coding;
	bool rle_coding;

	//
	int n_records;

	//
	char* in_ptr;
	int in_len;
	char* out_ptr;
	int out_len;

	//
	COLUMN_DESC col_desc;
}FIELD_DESC, *PFIELD_DESC;


int ParseConfigFile(FIELD_DESC* pfield_desc, int* pfield_num);
int GetCellLength(FIELD_DESC* pfield_desc, int field_num);



#define FLAG_REVERSE_CODING 		1
#define FLAG_DELTA_CODING			2
#define FLAG_DELTAOFDELTA_CODING	4
#define FLAG_RLE_CODING				8


#endif



