
/*
	general_coding.c
*/


#include "general_coding.h"
#include "zstd_api.h"


int need_to_delete_original_file()
{
	printf("Do you want to remove the original column_files? (y/N)");
	int ch = getchar();
	if ((ch != 'Y') && (ch != 'y'))
    {
        return 0;
    }

    /* flush rest of input line */
    while ((ch != EOF) && (ch != '\n'))
    {
        ch = getchar();
    }

	return 1;
}


int general_coding_file(char* input_filename, char* generic_compress_program_name, int removeFlag)
{
    if (UTIL_isDirectory(input_filename))
    {
        printf("%s is a directory--ignored!\n", input_filename);
        return -1;
    }

	char cmd_buffer[MAX_PATHNAME_LEN];
	sprintf(cmd_buffer, "%s %s", generic_compress_program_name, input_filename);
	pid_t status = system(cmd_buffer);
	if (status == -1)
	{
		printf("Execute system cmd error! cmd_buffer: %s\n", cmd_buffer);
        return -1;
	}

	//
	if (removeFlag && strstr(input_filename, ".flows_Record"))
	{
		sprintf(cmd_buffer, "rm -f %s", input_filename);
		status = system(cmd_buffer);
		if (status == -1)
		{
			printf("Execute system cmd error! cmd_buffer: %s\n", cmd_buffer);
			return -1;
		}

	}

	return 0;
}



int nfc_general_coding(FIELD_DESC* pfield_desc, int field_num)
{
	for (int i = 0; i < field_num; i++)
	{
		DbgPrint("Compress===field_id: %2d field_name: %s\n", (pfield_desc + i)->id, (pfield_desc + i)->name);
		if (nfc_general_coding_field(pfield_desc + i))
		{
			return -1;
		}
	}
	DbgPrint("\n");
	
	return 0;
}


int nfc_general_coding_field(FIELD_DESC* pfield_desc)
{
	if (pfield_desc->col_desc.column_count > 9)
	{
		printf("Error in nfc_general_coding_field! column_count: %d\n", pfield_desc->col_desc.column_count);
		return -1;
	}

	for (int i = 0; i < pfield_desc->col_desc.column_count; i++)
	{
		if (nfc_general_coding_column(&(pfield_desc->col_desc), i))
		{
			printf("Error in nfc_general_coding_field! field_id: %d column_id: %d\n", pfield_desc->id, i);
			return -1;
		}
	}

	return 0;
}


int nfc_general_coding_column(COLUMN_DESC* pcol_desc, int column_id)
{
	int additional_len = 1024;
	pcol_desc->out_ptr[column_id] = malloc(pcol_desc->in_len[column_id] + additional_len); //Compressed data my be larger! 
	if (pcol_desc->out_ptr[column_id] == NULL)
	{
		printf("Allocate out_ptr memory error! column_id: %d\n", column_id);
		return -1;
	}
	pcol_desc->out_len[column_id] = pcol_desc->in_len[column_id] + additional_len;

	//
	ZSTD_compressionParameters compressionParams;
	memset(&compressionParams, 0, sizeof(compressionParams));
    cRess_t ress = FIO_createCResources(NULL, ZSTDCLI_CLEVEL_DEFAULT, pcol_desc->in_len[column_id], &compressionParams);
	int compressedSize = FIO_compressData(ress, pcol_desc->in_ptr[column_id], pcol_desc->in_len[column_id], pcol_desc->out_ptr[column_id], pcol_desc->out_len[column_id], ZSTDCLI_CLEVEL_DEFAULT);
	FIO_freeCResources(ress);
	pcol_desc->out_len[column_id] = compressedSize;

	return 0;
}



