
/*
  nfc.c

*/

#include "debug.h"
#include "parse_config.h"



int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 2)
	{
		printf("Usage: ./program input_filename\n");
		return -1;
	}

	int field_num = 0;	
	FIELD_DESC field_desc[100];
	if (ParseConfigFile(field_desc, &field_num))
	{
		printf("Parse configuration file error!\n");
		return -1;	
	}

	


	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}



