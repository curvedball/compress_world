
/*
  nfc.c

Performance improvement:
(1) extract_field needs to remove additional memcpy for unused fields!

*/

#include "debug.h"
#include "parse_config.h"
#include "extract_field.h"
#include "reverse_coding.h"
#include "column_split.h"


int nfc_compress(char* input_filename)
{
	int field_num = 0;	
	FIELD_DESC field_desc[100];
	if (ParseConfigFile(field_desc, &field_num))
	{
		printf("Parse configuration file error!\n");
		return -1;	
	}

	if (nfc_extract_field(input_filename, field_desc, field_num))
	{
		return -1;
	}

	if (nfc_reverse_coding(field_desc, field_num))
	{
		printf("nfc_reverse_coding error!\n");	
		return -1;
	}

	if (nfc_column_split(field_desc, field_num))
	{
		printf("nfc_column_split error!\n");	
		return -1;		
	}

	if (nfc_general_coding(field_desc, field_num))
	{
		printf("nfc_column_split error!\n");	
		return -1;		
	}

	if (nfc_merge_data(input_filename, field_desc, field_num))
	{
		printf("nfc_column_split error!\n");	
		return -1;		
	}
	
	return 0;
}


int nfc_decompress(char* input_filename)
{
	return 0;
}


int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3)
	{
		printf("Usage: ./program operation(c or d) input_filename\n");
		return -1;
	}

	if (memcmp(argv[1], "c", 1) == 0)
	{
		if (nfc_compress(argv[2]))
		{
			return -1;
		}
	}
	else if (memcmp(argv[1], "d", 1) == 0)
	{
		if (nfc_decompress(argv[2]))
		{
			return -1;
		}		
	}
	else
	{
		printf("Usage: ./program operation(c or d) input_filename\n");
		return -1;
	}

	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}




