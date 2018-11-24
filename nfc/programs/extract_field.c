
/*
	extract_field.c
*/
#include "netflow_v5.h"
#include "netflow_v9.h"
#include "extract_field.h"


int parse_netflow_v5_field_data(char* in, int in_len, char* out_buffer_table[], int* out_size_arr)
{
	DbgPrint("parse_netflow_v5_field_data==in_len: %d\n", in_len);
	
	char* ptr = in;
	char* ptr_end = in + in_len;
	int i;
	int j;

	char* temp_ptr[V5_FIELD_NUM];
	for (i = 0; i < V5_FIELD_NUM; i++)
	{
		temp_ptr[i] = out_buffer_table[i];
	}

	while (ptr < ptr_end)
	{
		for (i = 0; i < V5_FIELD_NUM; i++)
		{
			for (j = 0; j < v5_field_width[i]; j++)
			{
				*(temp_ptr[i] + j) = *ptr++;
			}	
			out_size_arr[i] += v5_field_width[i];
			temp_ptr[i] += v5_field_width[i];
		}
	}

	return 0;
}



int extract_netflow_v5_field(char* input_filename, char* output_filename_table[])
{
	FILE* srcFile;
    int result;

    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    srcFile = FIO_openSrcFile(input_filename);
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

	int nRecords = srcSize / NETFLOW_V5_CELL_LEN;
	char* output_buffer_table[V5_FIELD_NUM];
	int output_buffer_table_size[V5_FIELD_NUM];
	
	int i;
	char* ptr;
	for (i = 0; i < V5_FIELD_NUM; i++)
	{
		ptr = malloc(nRecords * v5_field_width[i]);
		if (!ptr)
		{
			printf("Allocate memory for ptr error!\n");
			return -1;
		}
		output_buffer_table[i] = ptr;
		output_buffer_table_size[i] = 0; //zb: init
	}


	if (parse_netflow_v5_field_data(input_buffer, srcSize, output_buffer_table, output_buffer_table_size))
	{
		printf("parse_netflow_v5_field_data error!\n");		
		return -1;
	}

	FILE* dstFile;
	for (i = 0; i < V5_FIELD_NUM; i++)
	{
		dstFile = FIO_openDstFile(output_filename_table[i]);
		if (UTIL_writeFile(dstFile, output_buffer_table[i], output_buffer_table_size[i]))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);
		
	return 0;
}



//-----------------------------------------------------------------------------
int parse_netflow_v9_field_data(char* in, int in_len, char* out_buffer_table[], int* out_size_arr)
{
	DbgPrint("parse_netflow_v5_field_data==in_len: %d\n", in_len);
	
	char* ptr = in;
	char* ptr_end = in + in_len;
	int i;
	int j;

	char* temp_ptr[V9_FIELD_NUM];
	for (i = 0; i < V9_FIELD_NUM; i++)
	{
		temp_ptr[i] = out_buffer_table[i];
	}

	while (ptr < ptr_end)
	{
		for (i = 0; i < V9_FIELD_NUM; i++)
		{
			for (j = 0; j < v9_field_width[i]; j++)
			{
				*(temp_ptr[i] + j) = *ptr++;
			}	
			out_size_arr[i] += v9_field_width[i];
			temp_ptr[i] += v9_field_width[i];
		}
	}

	return 0;
}



int extract_netflow_v9_field(char* input_filename, char* output_filename_table[])
{
	FILE* srcFile;
    int result;

    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    srcFile = FIO_openSrcFile(input_filename);
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

	int nRecords = srcSize / NETFLOW_V9_CELL_LEN;
	char* output_buffer_table[V9_FIELD_NUM];
	int output_buffer_table_size[V9_FIELD_NUM];
	
	int i;
	char* ptr;
	for (i = 0; i < V9_FIELD_NUM; i++)
	{
		ptr = malloc(nRecords * v9_field_width[i]);
		if (!ptr)
		{
			printf("Allocate memory for ptr error!\n");
			return -1;
		}
		output_buffer_table[i] = ptr;
		output_buffer_table_size[i] = 0; //zb: init
	}


	if (parse_netflow_v9_field_data(input_buffer, srcSize, output_buffer_table, output_buffer_table_size))
	{
		printf("parse_netflow_v5_field_data error!\n");		
		return -1;
	}

	FILE* dstFile;
	for (i = 0; i < V9_FIELD_NUM; i++)
	{
		dstFile = FIO_openDstFile(output_filename_table[i]);
		if (UTIL_writeFile(dstFile, output_buffer_table[i], output_buffer_table_size[i]))
		{
			return -1;
		}
		fclose(dstFile);
	}

	fclose(srcFile);
		
	return 0;
}


int nfc_extract_field(char* input_filename, FIELD_DESC* pfield_desc, int field_num)
{
	FILE* srcFile;
    int result;
	int i;

    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

    srcFile = FIO_openSrcFile(input_filename);
    if (srcFile == NULL)
    {
        return -1;
    }

	//
	int cell_length = GetCellLength(pfield_desc, field_num);
	int srcSize = UTIL_getFileSize(input_filename);
	if (srcSize < cell_length)
	{
		printf("Too small of this file! %s size: %d cell_length: %d\n", input_filename, srcSize, cell_length);
		return -1;
	}

	if (srcSize % cell_length != 0)
	{
		printf("Invalid netflow file! srcSize is not n*cell_length. srcSize: %d cell_length: %d\n", srcSize, cell_length);
		return -1;		
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
	fclose(srcFile);


	//=======================
	char* output_buffer = malloc(srcSize);
	if (!output_buffer)
	{
		printf("Allocate output_buffer memory error!\n");
		return -1;
	}

	//
	int nRecords = srcSize / cell_length;
	pfield_desc[0].in_len = nRecords * pfield_desc[0].width;
	pfield_desc[0].in_ptr = output_buffer;
	pfield_desc[0].n_records = nRecords;
	for (int i = 1; i < field_num; i++)
	{
		pfield_desc[i].in_len = nRecords * pfield_desc[i].width;
		pfield_desc[i].in_ptr = pfield_desc[i - 1].in_ptr + pfield_desc[i - 1].in_len;
		pfield_desc[i].n_records = nRecords;
	}



	char* ptr = input_buffer;
	char* ptr_end = input_buffer + srcSize;
	while (ptr < ptr_end)
	{
		for (i = 0; i < field_num; i++)
		{
			memcpy(pfield_desc[i].in_ptr, ptr, pfield_desc[i].width);
			pfield_desc[i].in_ptr += pfield_desc[i].width;
			ptr += pfield_desc[i].width;
		}
	}

	//Reset the position of pointers to the start of each segment!
	for (i = 0; i < field_num; i++)
	{
		pfield_desc[i].in_ptr -= pfield_desc[i].in_len;
	}	

	return 0;
}



int nfc_restore_field(FIELD_DESC* pfield_desc, int field_num)
{
	for (int i = 0; i < field_num; i++)
	{
		if (nfc_restore_one_field(pfield_desc + i))
		{
			return -1;
		}
	}	
	return 0;
}

int nfc_restore_one_field(FIELD_DESC* pfield_desc)
{
	int width = pfield_desc->width;
	int nRecords = pfield_desc->n_records;
	char* temp_buffer = malloc(nRecords * width);
	if (temp_buffer == NULL)
	{
		printf("Allocate memory innfc_restore_one_field_func error!\n");
		return -1;
	}

	char* ptr = temp_buffer;
	COLUMN_DESC* pCol = &(pfield_desc->col_desc);
	for (int i = 0; i < nRecords; i++)
	{
		for (int j = 0; j < width; j++)
		{
			*ptr++ = *pCol->out_ptr[j]++;
		}
	}
	pfield_desc->in_ptr = temp_buffer; //zb: 
	pfield_desc->in_len = nRecords;

	//
	pfield_desc->out_ptr = pfield_desc->in_ptr;
	pfield_desc->out_len = nRecords;
	DbgPrint("nfc_restore_one_field===field_id: %d width: %d out_len: %d\n", pfield_desc->id, width, pfield_desc->out_len);

	//
	if (pfield_desc->reverse_coding == TRUE && pfield_desc->reverse_master == TRUE)
	{
		pfield_desc->control_ptr = pCol->out_ptr[width];
		pfield_desc->control_len = pCol->out_len[width];
		DbgPrint("nfc_restore_one_field===field_id: %d width: %d control_len: %d\n", pfield_desc->id, width, pfield_desc->control_len);
	}
	return 0;
}



