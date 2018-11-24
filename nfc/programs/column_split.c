
/*
	column_split.c
*/

#include "netflow_v5.h"
#include "netflow_v9.h"
#include "column_split.h"



int extract_netflow_v5_field_column(char* input_filename, int width, char* base_filename, int column_id)
{
    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    FILE* srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	//
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < NETFLOW_V5_CELL_LEN)
	{
		printf("Too small of this file! %s size: %d\n", input_filename, srcSize);
		return -1;
	}

	//
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

	DbgPrint("extract_netflow_v5_field_column...\n");


	//============================================================	
	int nRecords = srcSize / width;
	char* ptr_in_end = input_buffer + srcSize;

	//
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	
	char* out_buffer = malloc(nRecords);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }


	DbgPrint("extract_netflow_v5_field_column===nRecords: %d\n", nRecords);

	

	//============================================================
	FILE* dstFile;
	int i;
	char* ptr_in;
	char* ptr_out;
	for (i = 0; i < width; i++)
	{
		sprintf(o_filename, "%s_column%.2d", base_filename, column_id + i);
		ptr_in = input_buffer + i;
		ptr_out = out_buffer;
		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in;
			ptr_in += width;
			ptr_out++;
		}

		//DbgPrint("o_filename: %s\n", o_filename);
		dstFile = FIO_openDstFile(o_filename);
		if (!dstFile)
		{
			printf("dstFile error!\n");
        	return -1;
		}
		
		if (UTIL_writeFile(dstFile, out_buffer, nRecords))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);

	return 0;
}




int extract_netflow_v9_field_column(char* input_filename, int width, char* base_filename, int column_id)
{
    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    FILE* srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	//
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < NETFLOW_V9_CELL_LEN)
	{
		printf("Too small of this file! %s size: %d\n", input_filename, srcSize);
		return -1;
	}

	//
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

	DbgPrint("extract_netflow_v9_field_column...\n");


	//============================================================	
	int nRecords = srcSize / width;
	char* ptr_in_end = input_buffer + srcSize;

	//
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	
	char* out_buffer = malloc(nRecords);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }


	DbgPrint("extract_netflow_v9_field_column===nRecords: %d\n", nRecords);

	

	//============================================================
	FILE* dstFile;
	int i;
	char* ptr_in;
	char* ptr_out;
	for (i = 0; i < width; i++)
	{
		sprintf(o_filename, "%s_column%.2d", base_filename, column_id + i);
		ptr_in = input_buffer + i;
		ptr_out = out_buffer;
		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in;
			ptr_in += width;
			ptr_out++;
		}

		//DbgPrint("o_filename: %s\n", o_filename);
		dstFile = FIO_openDstFile(o_filename);
		if (!dstFile)
		{
			printf("dstFile error!\n");
        	return -1;
		}
		
		if (UTIL_writeFile(dstFile, out_buffer, nRecords))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);

	return 0;
}




int nfc_column_split(FIELD_DESC* pfield_desc, int field_num)
{
/*	
	for (int i = 0; i < field_num; i++)
	{
		DbgPrint("nfc_column_split===FieldDesc%.2d [name: %30s \twidth: %d \treverse_coding: %d \treverse_number: %d \tdelta_coding: %d]\n", i, pfield_desc[i].name, pfield_desc[i].width, pfield_desc[i].reverse_coding, pfield_desc[i].reverse_number, pfield_desc[i].delta_coding);
	}
*/	
	int cur_column_id = 0;
	for (int i = 0; i < field_num; i++)
	{
		//DbgPrint("zb===call nfc_split_one_field\n");
		if (nfc_split_one_field(pfield_desc + i, &cur_column_id))
		{
			return -1;
		}
	}
	return 0;
}



int nfc_split_one_field(FIELD_DESC* pfield_desc, int* pcur_column_id)
{
	int i;
	int cur_column_id = *pcur_column_id;
	int nColumnCount = (pfield_desc->reverse_coding && pfield_desc->reverse_master) ? pfield_desc->width + 1 : pfield_desc->width;
	int nRecords = pfield_desc->in_len / pfield_desc->width;
	pfield_desc->col_desc.column_count = nColumnCount;

/*
	if (pfield_desc->width == 1)
	{
		pfield_desc->in_len[0] = pfield_desc->in_len;
		pfield_desc->in_ptr[0] = pfield_desc->in_ptr;
		*pcur_column_id = cur_column_id + 1;
		return 0;
	}
*/

	//
	//printf("nRecords: %d\n", nRecords);
	for (i = 0; i < pfield_desc->width; i++)
	{
		pfield_desc->col_desc.in_len[i] = nRecords;
		pfield_desc->col_desc.in_ptr[i] = malloc(nRecords);
		if (pfield_desc->col_desc.in_ptr[i] == NULL)
		{
			printf("Allocate  memory error in nfc_split_one_field!\n");
			return -1;
		}
	}


	if (pfield_desc->reverse_coding)
	{
		pfield_desc->col_desc.in_len[i] = pfield_desc->control_len;
		pfield_desc->col_desc.in_ptr[i] = pfield_desc->control_ptr;
	}


	//
	char* ptr = pfield_desc->in_ptr;
	char* ptr_end = pfield_desc->in_ptr + pfield_desc->in_len;
	while (ptr < ptr_end)
	{
		for (i = 0; i < pfield_desc->width; i++)
		{
			*(pfield_desc->col_desc.in_ptr[i])= *ptr++;
			pfield_desc->col_desc.in_ptr[i]++;
		}
	}


	//zb: move the pointer to the beginning of each buffer
	for (i = 0; i < pfield_desc->width; i++)
	{
		pfield_desc->col_desc.in_ptr[i] -= nRecords;
	}

	//
	*pcur_column_id = cur_column_id + nColumnCount;

	return 0;
}






