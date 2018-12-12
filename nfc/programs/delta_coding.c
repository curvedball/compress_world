
/*
	delta_coding.c
*/


#include "delta_coding.h"


U64  ntohll(U64   host)
{   
	U64 ret = 0;   
	U32 high,low;
	low =  host & 0xFFFFFFFF;
	high = (host >> 32) & 0xFFFFFFFF;
	low = ntohl(low);   
	high = ntohl(high);   
	ret = low;
	ret <<= 32;   
	ret |= high;   
	return ret;   
}


U64  htonll(U64 host)   
{   
	U64 ret = 0;   
	U32 high,low;
	low = host & 0xFFFFFFFF;
	high = (host >> 32) & 0xFFFFFFFF;
	low = htonl(low);
	high = htonl(high);
	ret = low;
	ret <<= 32;
	ret |= high;
	return ret;
}





#if 0
int delta_coding(char* input_filename, int width)
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
	if (srcSize == 0)
	{
		printf("Error! FileSize is %d!\n", srcSize);	
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

	DbgPrint("delta_coding===input_filename: %s srcSize: %d\n", input_filename, srcSize);


	//============================================================
	char* out_buffer = malloc(srcSize);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }

	char* ptr_in_end = input_buffer + srcSize;
	char* ptr_in = input_buffer;	
	char* ptr_out = out_buffer;

	if (width == 1)
	{
		*ptr_out = *ptr_in;
		ptr_in++;
		ptr_out++;	

		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in - *(ptr_in - 1);
			ptr_in++;
			ptr_out++;
		}
	}
	else if (width == 2)
	{
		memcpy(ptr_out, ptr_in, 2);
		ptr_in += 2;
		ptr_out += 2;;	

		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in - *(ptr_in - 2);
			*(ptr_out + 1) = *(ptr_in + 1) - *(ptr_in - 1);

			ptr_in += 2;
			ptr_out += 2;
		}		
	}
	else if (width == 4)
	{
		memcpy(ptr_out, ptr_in, 4);
		ptr_in += 4;
		ptr_out += 4;;	

		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in - *(ptr_in - 4);
			*(ptr_out + 1) = *(ptr_in + 1) - *(ptr_in - 3);
			*(ptr_out + 2) = *(ptr_in + 2) - *(ptr_in - 2);
			*(ptr_out + 3) = *(ptr_in + 3) - *(ptr_in - 1);

			ptr_in += 4;
			ptr_out += 4;
		}
	}
	else if (width == 8)
	{
		memcpy(ptr_out, ptr_in, 8);
		ptr_in += 8;
		ptr_out += 8;;	

		while (ptr_in < ptr_in_end)
		{
			*ptr_out = *ptr_in - *(ptr_in - 8);
			*(ptr_out + 1) = *(ptr_in + 1) - *(ptr_in - 7);
			*(ptr_out + 2) = *(ptr_in + 2) - *(ptr_in - 6);
			*(ptr_out + 3) = *(ptr_in + 3) - *(ptr_in - 5);
			
			*(ptr_out + 4) = *(ptr_in + 4) - *(ptr_in - 4);
			*(ptr_out + 5) = *(ptr_in + 5) - *(ptr_in - 3);
			*(ptr_out + 6) = *(ptr_in + 6) - *(ptr_in - 2);
			*(ptr_out + 7) = *(ptr_in + 7) - *(ptr_in - 1);	

			ptr_in += 8;
			ptr_out += 8;
		}		
	}
	else
	{
		printf("Invalid width value! width: %d\n", width);
		return -1;
	}





	//============================================================
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	sprintf(o_filename, "%s_delta", input_filename);
	
	FILE* dstFile = FIO_openDstFile(o_filename);
	if (!dstFile)
	{
		printf("dstFile error!\n");
    	return -1;
	}
	
	if (UTIL_writeFile(dstFile, out_buffer, srcSize))
	{
		return -1;
	}
	fclose(dstFile);

	//
	fclose(srcFile);

	return 0;
}
#endif



int delta_coding_u16(char* input_filename, int width)
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
	if (srcSize == 0)
	{
		printf("Error! FileSize is %d!\n", srcSize);	
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

	DbgPrint("delta_coding===input_filename: %s srcSize: %d\n", input_filename, srcSize);


	//============================================================
	char* out_buffer = malloc(srcSize);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }

	U16* ptr_in_end = (U16*)(input_buffer + srcSize);
	U16* ptr_in = input_buffer;
	U16* ptr_out = out_buffer;

	*ptr_out++ = *ptr_in++;
	while (ptr_in < ptr_in_end)
	{
		*ptr_out = htons(ntohs(*ptr_in) - ntohs(*(ptr_in - 1)));
		ptr_in += 1;
		ptr_out += 1;
	}		

	//============================================================
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	sprintf(o_filename, "%s_delta", input_filename);
	
	FILE* dstFile = FIO_openDstFile(o_filename);
	if (!dstFile)
	{
		printf("dstFile error!\n");
    	return -1;
	}
	
	if (UTIL_writeFile(dstFile, out_buffer, srcSize))
	{
		return -1;
	}
	fclose(dstFile);

	//
	fclose(srcFile);

	return 0;
}


int delta_coding_advanced(char* input_filename, int width)
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
	if (srcSize == 0)
	{
		printf("Error! FileSize is %d!\n", srcSize);	
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
	//
	fclose(srcFile);
	DbgPrint("delta_coding===input_filename: %s srcSize: %d\n", input_filename, srcSize);


	//============================================================
	BYTE* temp_buffer = malloc(srcSize);
	if (temp_buffer == NULL)
    {
		printf("Allocate temp_buffer error!\n");
        return -1;
    }
	memcpy(temp_buffer, input_buffer, srcSize);


	
	BYTE* index_buffer = malloc(srcSize);
	if (index_buffer == NULL)
    {
		printf("Allocate index_buffer error!\n");
        return -1;
    }

	char* out_buffer = malloc(srcSize);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }

	int nOutSize;
	U16 delta = 1; //zb?
	if (width == 1)
	{
		
	}
	else if (width == 2)
	{
		int count  = 0;
		bool init_value_saved = FALSE; 
		U16 v1, v2;
		U16* ptr_in_end = (U16*)(input_buffer + srcSize);
		U16* ptr_in = (U16*)input_buffer;
		U16* ptr_stub = ptr_in;
		BYTE* ptr_index = index_buffer;
		U16* ptr_out = (U16*)out_buffer;

		//
		U16* ptr_temp_end = (U16*)(temp_buffer + srcSize);
		U16* ptr_temp = (U16*)temp_buffer;
		

		//*ptr_in++ = *ptr_temp;
		*ptr_in = ntohs(*ptr_temp);
		ptr_in++;
		ptr_temp++;
		while (ptr_temp < ptr_temp_end)
		{
			/*
			count++;
			if (count > 32)
			{
				break;
			}
			DbgPrint("DeltaValue: 0x%.4X\n", ntohs(*ptr_temp));
			*/
			//*ptr_in = *(ptr_temp) - *(ptr_temp - 1);
			*ptr_in = ntohs(*ptr_temp) - ntohs(*(ptr_temp - 1));
			ptr_in++;
			ptr_temp++;
			//DbgPrint("CurrentValue: 0x%.4X\n", (*ptr_in));
		}
		

		//
		ptr_in = (U16*)input_buffer;
		while (ptr_in < ptr_in_end)
		{
			//DbgPrint("NewCurrentValue: 0x%.4X\n", (*ptr_in));
			/*
			count++;
			if (count > 32)
			{
				break;
			}
			DbgPrint("DeltaValue: 0x%.4X\n", (*ptr_in));
			*/


			if ((*ptr_in) == delta)
			{
				DbgPrint("EqualValue: 0x%.4X\n", (*ptr_in));
				*ptr_index++ = 0;
			}
			else
			{
				DbgPrint("DiffValue: 0x%.4X\n", (*ptr_in));
				*ptr_index++ = 1;
				*ptr_out++ = *ptr_in;
			}
			ptr_in++;
		}
		DbgPrint("srcSize: %d outSize: %d indexSize: %d\n", srcSize, ((BYTE*)ptr_out - (BYTE*)out_buffer), (ptr_index - index_buffer)/8);
		nOutSize = ((BYTE*)ptr_out - (BYTE*)out_buffer);
	}
	else if (width == 4)
	{

	}
	else if (width == 8)
	{
	
	}
	else
	{
		printf("Invalid width value! width: %d\n", width);
		return -1;
	}


	//============================================================
	char* o_filename = malloc(MAX_PATHNAME_LEN);
	if (o_filename == NULL)
    {
		printf("Allocate o_filename error!\n");
        return -1;
    }
	sprintf(o_filename, "%s_delta", input_filename);
	
	FILE* dstFile = FIO_openDstFile(o_filename);
	if (!dstFile)
	{
		printf("dstFile error!\n");
    	return -1;
	}
	
	if (UTIL_writeFile(dstFile, out_buffer, nOutSize))
	{
		return -1;
	}
	fclose(dstFile);


	return 0;
}


//=======================================================
int nfc_delta_coding(FIELD_DESC* pfield_desc, int field_num)
{
	FIELD_DESC* pcur_field_desc;
	for (int i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		//DbgPrint("nfc_delta_coding Processing field_id: %d field_name: %s\n", pcur_field_desc->id, pcur_field_desc->name);
		if (pcur_field_desc->delta_coding == TRUE)
		{
			//DbgPrint("RUN nfc_delta_coding Processing field_id: %d field_name: %s\n", pcur_field_desc->id, pcur_field_desc->name);
			pcur_field_desc->delta_check = FALSE;

			//
			nfc_delta_coding_field(pcur_field_desc);
		}
	}
	DbgPrint("\n");
	return 0;
}



int nfc_delta_coding_field(FIELD_DESC* pfield_desc)
{
	//int nRecords = pfield_desc->n_records; //zb?? Not right for some fields, e.g. dst_port!
	int width = pfield_desc->width;
	int len = pfield_desc->in_len;
	if (len % width != 0)
	{
		printf("nfc_delta_coding_field_func error! check bytes failed!");
		return -1;		
	}
	int nRecords = len / width;

	//
	char* buf = malloc(len);
	if (buf == NULL)
	{
		printf("Allocate memory in nfc_delta_coding_field_func error!");
		return -1;		
	}

	if (len < 1)
	{
		printf("Field data len is too short! len: %d!", len);
		return -1;		
	}

	char* buf_ptr = buf;
	char* data_ptr = pfield_desc->in_ptr;

	//
	int i, j;
	for (i = 0; i < width; i++)
	{
		*buf_ptr++ = *data_ptr++;
	}
	for (i = 1; i < nRecords; i++)
	{
		nfc_set_delta_value(buf_ptr, data_ptr, width);
		buf_ptr += width;
		data_ptr += width;
	}
	
	//DbgPrint("FUNC nfc_delta_coding Processing field_id: %d field_name: %s\n", pfield_desc->id, pfield_desc->name);
	U64 v1, v2;
	if (nfc_calc_nonuniformity(pfield_desc->in_ptr, len, &v1))
	{
		printf("nfc_calc_entropy_func error!");
		return -1;	
	}
	if (nfc_calc_nonuniformity(buf, len, &v2))
	{
		printf("nfc_calc_entropy_func error!");
		return -1;	
	}

	//
	if (v2 > v1)
	{
		DbgPrint("data_coding! field_id: %d field_name: %s\n", pfield_desc->id, pfield_desc->name);
		pfield_desc->delta_check = TRUE;
		pfield_desc->in_ptr = buf;
	}
	else
	{
		printf("!!! Don't need to use delta coding! field_id: %d field_name: %s\n", pfield_desc->id, pfield_desc->name);
		return -1;
	}
	
	return 0;
}


int nfc_set_delta_value(char* buf, char* data, int width)
{
	U16 v16;
	U32 v32;
	U64 v64;
	switch (width)
	{
		case 1:
			buf[0] = (*data) - (*(data - width));
			break;
		case 2:
			v16 = htons(ntohs((*(U16*)data)) - ntohs((*(U16*)(data - width))));
			buf[0] = v16 & 0xFF;
			buf[1] = v16 >> 8;
			break;
		case 4:
			v32 = htonl(ntohl((*(U32*)data)) - ntohl((*(U32*)(data - width))));
			buf[0] = v32 & 0xFF;
			buf[1] = (v32 >> 8) & 0xFF;	
			buf[2] = (v32 >> 16) & 0xFF;	
			buf[3] = (v32 >> 24) & 0xFF;	
			break;
		case 8:
			v64 = htonll(ntohll((*(U64*)data)) - ntohll((*(U64*)(data - width))));
			buf[0] = v64 & 0xFF;
			buf[1] = (v64 >> 8) & 0xFF;	
			buf[2] = (v64 >> 16) & 0xFF;	
			buf[3] = (v64 >> 24) & 0xFF;	
			buf[4] = (v64 >> 32) & 0xFF;	
			buf[5] = (v64 >> 40) & 0xFF;	
			buf[6] = (v64 >> 48) & 0xFF;	
			buf[7] = (v64 >> 56) & 0xFF;	
			break;
		default:
			printf("Error! Not supported by current version!!!\n");
			return -1;
			break;
	}
	return 0;
}



int nfc_calc_nonuniformity(char* data, int len, U64* pvalue)
{
	U64 v = 0L;
	int i;
	int count[256];
	memset(count, 0, 256);
	for (i = 0; i < len; i++)
	{
		count[(BYTE)data[i]]++;
	}

	//
	int avg = len / 256;
	for (i = 0; i < 256; i++)
	{
		//v += (count[i] - avg) * (count[i] - avg);
		v += abs(count[i] - avg);
		
	}
	*pvalue = v;
	return 0;
}



//=======================================================
int nfc_delta_decoding(FIELD_DESC* pfield_desc, int field_num)
{
	FIELD_DESC* pcur_field_desc;
	for (int i = 0; i < field_num; i++)
	{
		pcur_field_desc = pfield_desc + i;
		if (pcur_field_desc->delta_coding == TRUE && pcur_field_desc->delta_check == TRUE)
		{
			nfc_delta_decoding_field(pcur_field_desc);
		}
	}
	return 0;
}


int nfc_delta_decoding_field(FIELD_DESC* pfield_desc)
{
	//int nRecords = pfield_desc->n_records; //zb?? Not right for some fields, e.g. dst_port!
	int width = pfield_desc->width;
	int len = pfield_desc->in_len;	
	if (len % width != 0)
	{
		printf("nfc_delta_decoding_field_func error! check bytes failed!");
		return -1;		
	}
	int nRecords = len / width;

	char* data_ptr = pfield_desc->in_ptr;
	data_ptr += width;
	for (int i = 1; i < nRecords; i++)
	{
		nfc_restore_delta_value(data_ptr, width);
		data_ptr += width;
	}
	return 0;
}


int nfc_restore_delta_value(char* data, int width)
{
	U16 v16;
	U32 v32;
	U64 v64;
	switch (width)
	{
		case 1:
			(*data) = (*data) + (*(data - width));
			break;
		case 2:
			v16 = htons(ntohs((*(U16*)data)) + ntohs((*(U16*)(data - width))));
			data[0] = v16 & 0xFF;
			data[1] = v16 >> 8;
			break;
		case 4:
			v32 = htonl(ntohl((*(U32*)data)) + ntohl((*(U32*)(data - width))));
			data[0] = v32 & 0xFF;
			data[1] = (v32 >> 8) & 0xFF;	
			data[2] = (v32 >> 16) & 0xFF;	
			data[3] = (v32 >> 24) & 0xFF;	
			break;
		case 8:
			v64 = htonll(ntohll(*(U64*)data) + ntohll(*(U64*)(data - width)));
			data[0] = v64 & 0xFF;
			data[1] = (v64 >> 8) & 0xFF;	
			data[2] = (v64 >> 16) & 0xFF;	
			data[3] = (v64 >> 24) & 0xFF;	
			data[4] = (v64 >> 32) & 0xFF;	
			data[5] = (v64 >> 40) & 0xFF;	
			data[6] = (v64 >> 48) & 0xFF;	
			data[7] = (v64 >> 56) & 0xFF;	
			break;
		default:
			printf("Error! Not supported by current version!!!\n");
			return -1;
			break;
	}
	return 0;
}



