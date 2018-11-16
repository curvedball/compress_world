
/*
  parse_config.h

*/

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
}FIELD_DESC, *PFIELD_DESC;


int ParseConfigFile(FIELD_DESC* pfield_desc, int* pfield_num);
int GetCellLength(FIELD_DESC* pfield_desc, int field_num);



