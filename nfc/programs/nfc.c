
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


int nfc_compress_print_info(char* input_filename)
{
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize <= 0)
	{
		printf("nfc_compress_print_info_func error! filename: %s srcSize: %d\n", input_filename, srcSize);
		return -1;
	}
	char output_filename[MAX_PATHNAME_LEN];
	sprintf(output_filename, "%s.nfc", input_filename);
	int dstSize = UTIL_getFileSize(output_filename);
	if (dstSize <= 0)
	{
		printf("nfc_compress_print_info_func error! filename: %s dstSize: %d\n", output_filename, dstSize);
		return -1;
	}

	DbgPrint("Compress [%d --> %d], ratio: %.2f\n\n", srcSize, dstSize, srcSize*1.0/dstSize);
	return 0;
}

int nfc_compress(char* input_filename)
{
	int field_num = 0;	
	FIELD_DESC field_desc[100];


	//
	clock_t start = clock();
		
	if (ParseConfigFile(field_desc, &field_num))
	{
		printf("Parse configuration file error!\n");
		return -1;	
	}

	U32 ret = nfc_extract_field(input_filename, field_desc, field_num);
	if (ret < 0)
	{
		printf("nfc_extract_field error!\n");
		return -1;
	}

	if (nfc_reverse_coding(field_desc, field_num))
	{
		printf("nfc_reverse_coding error!\n");	
		return -1;
	}

	if (nfc_delta_coding(field_desc, field_num))
	{
		printf("nfc_delta_coding error!\n");
		return -1;		
	}

	if (nfc_column_split(field_desc, field_num))
	{
		printf("nfc_column_split error!\n");	
		return -1;		
	}

	if (nfc_rle_coding(field_desc, field_num))
	{
		printf("nfc_rle_coding error!\n");	
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
	
    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
	double totalSize = ret * 8.0;
	double speed = totalSize / seconds / (1 << 30);
    printf("Filesize: %u Completed in %.2f sec! Compress Speed: %.2f Gbps\n", ret, seconds, speed);

	if (nfc_compress_print_info(input_filename))
	{
		return -1;
	}
	
	return 0;
}



int nfc_decompress_print_info(char* input_filename)
{
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize <= 0)
	{
		printf("nfc_decompress_print_info_func error! filename: %s srcSize: %d\n", input_filename, srcSize);
		return -1;
	}
	char output_filename[MAX_PATHNAME_LEN];
	sprintf(output_filename, "%s.d", input_filename);
	int dstSize = UTIL_getFileSize(output_filename);
	if (dstSize <= 0)
	{
		printf("nfc_decompress_print_info_func error! filename: %s dstSize: %d\n", output_filename, dstSize);
		return -1;
	}

	DbgPrint("Decompress [%d --> %d]\n\n", srcSize, dstSize);
	return 0;

}


int nfc_decompress(char* input_filename)
{
	int field_num = 0;	
	FIELD_DESC field_desc[100];

	//
	clock_t start = clock();

	if (nfc_parse_compressed_file(input_filename, field_desc, &field_num))
	{
		printf("nfc_parse_compressed_file error!\n");
		return -1;	
	}

	if (nfc_general_decoding(field_desc, field_num))
	{
		printf("nfc_decompress_column error!\n");	
		return -1;		
	}

	if (nfc_rle_decoding(field_desc, field_num))
	{
		printf("nfc_rle_decoding error!\n");	
		return -1;				
	}

	if (nfc_restore_field(field_desc, field_num))
	{
		printf("nfc_restore_field error!\n");
		return -1;
	}

	if (nfc_delta_decoding(field_desc, field_num))
	{
		printf("nfc_delta_decoding_func error!\n");	
		return -1;
	}

	if (nfc_deltaofdelta_decoding(field_desc, field_num))
	{
		printf("nfc_deltaofdelta_decoding_func error!\n");	
		return -1;
	}

	if (nfc_reverse_decoding(field_desc, field_num))
	{
		printf("nfc_reverse_decoding error!\n");	
		return -1;
	}

	U32 ret = nfc_restore_compressed_file(input_filename, field_desc, field_num);
	if (ret < 0)
	{
		printf("nfc_restore_compressed_file_func error!\n");	
		return -1;		
	}

    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
	double totalSize = ret * 8.0;
	double speed = totalSize / seconds / (1 << 30);
    printf("Filesize: %u Completed in %.2f sec! Decompress Speed: %.2f Gbps\n", ret, seconds, speed);

	if (nfc_decompress_print_info(input_filename))
	{
		return -1;
	}

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




