
/*
	merge_data.c
*/


#include "merge_data.h"




int merge_input_file(char* input_filename, FILE* dstFile)
{
	FILE* srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize == 0)
	{
		return 0;
	}

	char* input_buffer = malloc(srcSize);
	if (!input_buffer)
	{
		printf("Allocate input_buffer memory error!\n");
		return -1;
	}
		
	if (UTIL_readFile(srcFile, input_buffer, srcSize) < 0)
	{
		return -1;
	}

	if (UTIL_writeFile(dstFile, input_buffer, srcSize))
	{
		return -1;
	}

	fclose(srcFile);

	return 0;
}




/*
	Output to file using certain format.
------------------------------------------------------	
	NFC: 3 bytes
	head_len: 2 bytes [exclue the 5 bytes of NFC and itself]
	n_records: 4 bytes
	field_num: 1 byte
	field_desp0: 1 byte
	field_desp1: 1 byte
	...
	field_desp10: 1 byte
	reverse_num1: 1 byte
	reverse_num2: 1 byte
	reverse_num3: 1 byte
	reverse_num4: 1 byte
	n_cloumns: 2 bytes
	column0_len: 4 bytes
	column1_len: 4 bytes
	...
	column38_len: 4 bytes

*/
int nfc_merge_data(char* input_filename, FIELD_DESC* pfield_desc, int field_num)
{
/*
	for (int i = 0; i < field_num; i++)
	{
		DbgPrint("nfc_merge_data===FieldDesc%.2d [name: %30s \twidth: %d \treverse_coding: %d \treverse_number: %d \tdelta_coding: %d]\n", i, pfield_desc[i].name, pfield_desc[i].width, pfield_desc[i].reverse_coding, pfield_desc[i].reverse_number, pfield_desc[i].delta_coding);
	}
*/	
	char output_filename[MAX_PATHNAME_LEN];
	sprintf(output_filename, "%s.nfc", input_filename);
	
	FILE* dstFile = FIO_openDstFile(output_filename);
	if (!dstFile)
	{
		printf("Open dstFile error in nfc_merge_data_func!\n");
		return -1;
	}

	int head_buf_size = 10240;
	char* head_buf = malloc(head_buf_size);
	if (head_buf == NULL)
	{
		printf("Allocate head_buf error!\n");
		return -1;
	}
	int head_buf_len = 0;

	//=========================
	U32* ptr32;
	U16* ptr16;
	char* ptr = head_buf;
	ptr[0] = 'N';
	ptr[1] = 'F';	
	ptr[2] = 'C';
	ptr += 5;
	ptr32 = (U32*)ptr;
	*ptr32++ = pfield_desc->n_records;
	ptr = (char*)ptr32;
	*ptr++ = (char)field_num;

	//
	BYTE v;
	FIELD_DESC* pcur_field_desc;
	int nColumnCount = 0;
	int i, j;
	for (i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		v = 0;
		if (pcur_field_desc->reverse_coding)
		{
			v |= FLAG_REVERSE_CODING;
		}
		if (pcur_field_desc->delta_coding)
		{
			v |= FLAG_DELTA_CODING;
		}
		if (pcur_field_desc->deltaofdelta_coding)
		{
			v |= FLAG_DELTAOFDELTA_CODING;
		}
		if (pcur_field_desc->rle_coding)
		{
			v |= FLAG_RLE_CODING;
		}
		v |= ((char)pcur_field_desc->width) << 4;
		*ptr++ = v;
		nColumnCount += pcur_field_desc->col_desc.column_count;
	}

	//
	for (i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		v = pcur_field_desc->reverse_number;
		if (v > 0)
		{
			*ptr++ = v;
		}
	}

	//
	ptr16 = ptr;
	*ptr16++ = nColumnCount;
	ptr32 = (U32*)ptr16;
	//=============================
	for (i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		for (j = 0; j < pcur_field_desc->col_desc.column_count; j++)
		{
			*ptr32++ = (U32)(pcur_field_desc->col_desc.out_len[j]);
		}	
	}

	//
	head_buf_len = (int)((char*)ptr32 - head_buf);
	ptr16 = (U16*)(head_buf + 3);
	*ptr16 = head_buf_len - 5;
	if (UTIL_writeFile(dstFile, head_buf, head_buf_len))
	{
		printf("Write head_buf error! output_filename: %s\n", output_filename);
		return -1;
	}

	//=============================
	for (i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		for (j = 0; j < pcur_field_desc->col_desc.column_count; j++)
		{
			if (UTIL_writeFile(dstFile, pcur_field_desc->col_desc.out_ptr[j], pcur_field_desc->col_desc.out_len[j]))
			{
				printf("Write output_data error! output_filename: %s field_id: %d field_name: %s column_id: %d\n", output_filename, pcur_field_desc->id, pcur_field_desc->name, j);
				return -1;
			}
		}
	}	

	fclose(dstFile);
	return 0;
}




