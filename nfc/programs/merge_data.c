
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

	int head_buf_size = HEAD_MAX_LEN;
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
	memcpy(ptr, "NFC", 3);
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
		nColumnCount += pcur_field_desc->col_desc.column_count;

		//
		v = 0;
		if (pcur_field_desc->reverse_coding)
		{
			v |= FLAG_REVERSE_CODING;
		}
		if (pcur_field_desc->delta_coding && pcur_field_desc->delta_check)
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

		//Append one byte for this field.
		if (pcur_field_desc->reverse_coding && pcur_field_desc->reverse_number > 0)
		{
			if (pcur_field_desc->reverse_master)
			{
				*ptr++ = (1 << 7) | pcur_field_desc->reverse_number;
			}
			else
			{
				*ptr++ = pcur_field_desc->reverse_number;
			}
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



int nfc_find_field_column(int column_number, FIELD_DESC* pfield_desc, int field_num, FIELD_DESC** ppfield_desc, int* pcolumn_id)
{
	int nColumn = 0;
	FIELD_DESC* pCurFieldDesc;
	for (int i = 0; i < field_num; i++)
	{
		pCurFieldDesc = pfield_desc + i;
		if (column_number < nColumn + pCurFieldDesc->col_desc.column_count)
		{
			*ppfield_desc = pCurFieldDesc;
			*pcolumn_id = column_number - nColumn;
			return 0;
		}
		nColumn += pCurFieldDesc->col_desc.column_count;
	}
	return -1;
}


int nfc_parse_compressed_file(char* input_filename, FIELD_DESC* pfield_desc, int* pfield_num)
{
	FILE* srcFile = FIO_openSrcFile(input_filename);
	if (!srcFile)
	{
		printf("Open srcFile error in nfc_parse_compressed_file!\n");
		return -1;
	}

	int head_buf_size = HEAD_MAX_LEN;
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
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < 5)
	{
		printf("Error! FileSize is %d!\n", srcSize);	
		return -1;
	}
	
	if (UTIL_readFile(srcFile, head_buf, 5) < 0)
	{
		return -1;
	}
	
	if (memcmp(ptr, "NFC", 3))
	{
		printf("Invalid file format! Magic string is not NFC!\n");
		return -1;
	}

	//
	ptr += 3;
	ptr16 = (U16*)ptr;
	int nHeadLen = *ptr16;
	if (nHeadLen + 5 >= HEAD_MAX_LEN)
	{
		printf("head len is too large! %d\n", nHeadLen);
		return -1;
	}
	
	ptr += 2;
	if (UTIL_readFile(srcFile, ptr, nHeadLen) < 0)
	{
		printf("Read file head data error!\n");
		return -1;
	}

	//
	ptr32 = (U32*)ptr;
	int nRecords = *ptr32++;
	ptr = (char*)ptr32;
	int field_num = (int)*ptr++;
	*pfield_num = field_num;

	//
	int i;
	char v, v2;
	FIELD_DESC* pcur_field_desc;
	for (i = 0; i < field_num; i++)
	{
		v = *ptr++;
		pcur_field_desc = pfield_desc + i;
		pcur_field_desc->id = i;
		pcur_field_desc->n_records = nRecords;
		pcur_field_desc->width = (v >> 4) & 0x0F;
		pcur_field_desc->col_desc.column_count = pcur_field_desc->width;
		if (v & FLAG_REVERSE_CODING)
		{
			pcur_field_desc->reverse_coding = TRUE;
		}

		if (v & FLAG_DELTA_CODING)
		{
			pcur_field_desc->delta_coding = TRUE;
			pcur_field_desc->delta_check = TRUE;
			
		}		
		if (v & FLAG_DELTAOFDELTA_CODING)
		{
			pcur_field_desc->deltaofdelta_coding = TRUE;
		}	
		if (v & FLAG_RLE_CODING)
		{
			pcur_field_desc->rle_coding= TRUE;
		}

		//
		if (v & FLAG_REVERSE_CODING)
		{
			v2 = *ptr++;
			if (v2 >> 7)
			{
				pcur_field_desc->reverse_master = TRUE;
				pcur_field_desc->reverse_number = v2 & 0x7F;
				pcur_field_desc->col_desc.column_count++; //zb: only for master of reverse_coding field!
			}
			else
			{
				pcur_field_desc->reverse_master = FALSE;
				pcur_field_desc->reverse_number = v2 & 0x7F;
			}		
		}
	}

	//
	ptr16 = (U16*)ptr;
	int nColumns = (int)*ptr16++;
	DbgPrint("nRecords: %d field_num: %d nColumns: %d\n", nRecords, field_num, nColumns);

	//
	int nCurColumnLen;
	int column_id;
	char* pcol;
	ptr32 = (U32*)ptr16;
	for (i = 0; i < nColumns; i++)
	{
		nCurColumnLen = *ptr32++;
		//DbgPrint("nCurColumnLen: %d\n", nCurColumnLen);

		//
		if (nfc_find_field_column(i, pfield_desc, field_num, &pcur_field_desc, &column_id))
		{
			printf("Cannot find the column %d in field!\n", i);
			return -1;
		}

		//
		pcur_field_desc->col_desc.in_len[column_id] = nCurColumnLen;
		pcol =  malloc(nCurColumnLen);
		if (pcol == NULL)
		{
			printf("Allocate memory for pcol error! nCurColumnLen: %d\n", nCurColumnLen);
			return -1;
		}

		if (UTIL_readFile(srcFile, pcol, nCurColumnLen) < 0)
		{
			return -1;
		}
		pcur_field_desc->col_desc.in_ptr[column_id] = pcol;
	}
	

	//
	for (i = 0; i < field_num; i++)
	{
		DbgPrint("nfc_parse_compressed_file===FieldDesc%.2d width: %d \treverse_coding: %d \treverse_number: %d \tdelta_coding: %d coumn_count: %d]\n", i, pfield_desc[i].width, pfield_desc[i].reverse_coding, pfield_desc[i].reverse_number, pfield_desc[i].delta_coding, pfield_desc[i].col_desc.column_count);
	}
	DbgPrint("\n");		
	return 0;
}


int nfc_restore_compressed_file(char* input_filename, FIELD_DESC* pfield_desc, int field_num)
{
	char output_filename[MAX_PATHNAME_LEN];
	sprintf(output_filename, "%s.d", input_filename);
	
	FILE* dstFile = FIO_openDstFile(output_filename);
	if (!dstFile)
	{
		printf("Open dstFile error in nfc_restore_compressed_file_func!\n");
		return -1;
	}

	//=============================
	int nRecords = pfield_desc[0].n_records;
	int i, j;
	FIELD_DESC* pcur_field_desc;
	int nCellWidth = 0;
	for (i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		nCellWidth += pcur_field_desc->width;
	}

	//
	int nTotalLen = nCellWidth * nRecords;
	//DbgPrint("field_num: %d nCellWidth: %d nRecords: %d nTotalLen: %d\n", field_num, nCellWidth, nRecords, nTotalLen);
	char* output_buf = malloc(nTotalLen);
	if (output_buf == NULL)
	{
		printf("Allocate memory for output_buf in nfc_restore_compressed_file_func error!\n");
		return -1;
	}

	char* output_ptr = output_buf;
	for (i = 0; i < nRecords; i++)
	{
		for (j = 0; j < field_num; j++)
		{
			//DbgPrint("Output bytes! i: %d j: %d CopyPos: %d\n", i, j, output_ptr - output_buf);
			pcur_field_desc = pfield_desc + j;
			memcpy(output_ptr, pcur_field_desc->out_ptr, pcur_field_desc->width);
			pcur_field_desc->out_ptr += pcur_field_desc->width;
			output_ptr += pcur_field_desc->width;
		}	
	}

	if (output_ptr - output_buf != nTotalLen)
	{
		printf("Output data Error in nfc_restore_compressed_file_func! nOutputLen: %d nTotalLen: %d\n", output_ptr - output_buf, nTotalLen);
		return -1;
	}

	if (UTIL_writeFile(dstFile, output_buf, nTotalLen))
	{
		printf("Write output_data error in nfc_restore_compressed_file_func! \n");
		return -1;
	}

	fclose(dstFile);

	int dstSize = UTIL_getFileSize(output_filename);
	return dstSize;
}



