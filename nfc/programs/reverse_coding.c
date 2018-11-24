
/*
	reverse_coding.c
*/
#include "reverse_coding.h"


int reverse_coding_netflow_field_u32(char* in1, char* in2, int nRecords, char* control_buffer, int* pcontrol_size, char* out1, char* out2, int* pout2_size)
{
	//DbgPrint("reverse_coding_netflow_field_u16=====nRecords: %d nBytes: %d\n", nRecords, nRecords * 2);
	
	U32 v1a, v1b, v2a, v2b;
	U32* ptr1 = (U16*)in1;
	U32* ptr_end = ptr1 + nRecords;
	U32* ptr2 = (U16*)in2;
	U32* ptr_out2 = (U16*)out2;
	U32 nOutSize2 = 0;

	//
	BYTE* ptr_control = control_buffer;
	BYTE vControl = 0;
	U32  nShiftCount = 0;
	U32  nControlSize = 0;
	int  nOneBits = 0;
	int  nZeroBits = 0;

	//
	while (ptr1 < ptr_end)
	{
		v1a = *ptr1;
		v1b = *(ptr1 + 1);
		v2a = *ptr2;
		v2b = *(ptr2 + 1);

		//
		if (v1a == v2b && v2a == v1b)
		{
			vControl |= 1 << (8 - nShiftCount - 1);
			ptr1 += 2;
			ptr2 += 2;
			nOneBits++;
		}
		else
		{
			*ptr_out2 = *ptr2;
			nOutSize2 += 2; //zb: bytes len measure
			ptr1 += 1;
			ptr2 += 1;
			ptr_out2 += 1;
			nZeroBits++;
		}
		

		//======================
		nShiftCount++;
		if (nShiftCount > 7)
		{
			*ptr_control = vControl;
			nControlSize++;
			ptr_control++;
			vControl = 0;
			nShiftCount = 0;
		}
	}

	if (nShiftCount > 0)
	{
		*ptr_control = vControl;
		nControlSize++;
		ptr_control++;
	}

	//
	*pcontrol_size = nControlSize;
	*pout2_size = nOutSize2;
	DbgPrint("reverse_coding_netflow_field_u32=====nControlSize: %d nOneBits: %d nZeroBits: %d\n", nControlSize, nOneBits, nZeroBits);

	return 0;

}

int reverse_coding_netflow_field_u16(char* in1, char* in2, int nRecords, char* control_buffer, int* pcontrol_size, char* out1, char* out2, int* pout2_size)
{
	//DbgPrint("reverse_coding_netflow_field_u16=====nRecords: %d nBytes: %d\n", nRecords, nRecords * 2);
	
	U16 v1a, v1b, v2a, v2b;
	U16* ptr1 = (U16*)in1;
	U16* ptr_end = ptr1 + nRecords;
	U16* ptr2 = (U16*)in2;
	U16* ptr_out2 = (U16*)out2;
	U32 nOutSize2 = 0;

	//
	BYTE* ptr_control = control_buffer;
	BYTE vControl = 0;
	U32  nShiftCount = 0;
	U32  nControlSize = 0;
	int  nOneBits = 0;
	int  nZeroBits = 0;

	//
	while (ptr1 < ptr_end)
	{
		v1a = *ptr1;
		v1b = *(ptr1 + 1);
		v2a = *ptr2;
		v2b = *(ptr2 + 1);

		//
		if (v1a == v2b && v2a == v1b)
		{
			vControl |= 1 << (8 - nShiftCount - 1);
			ptr1 += 2;
			ptr2 += 2;
			nOneBits++;
		}
		else
		{
			*ptr_out2 = *ptr2;
			nOutSize2 += 2; //zb: bytes len measure
			ptr1 += 1;
			ptr2 += 1;
			ptr_out2 += 1;
			nZeroBits++;
		}
		

		//======================
		nShiftCount++;
		if (nShiftCount > 7)
		{
			*ptr_control = vControl;
			nControlSize++;
			ptr_control++;
			vControl = 0;
			nShiftCount = 0;
		}
	}

	if (nShiftCount > 0)
	{
		*ptr_control = vControl;
		nControlSize++;
		ptr_control++;
	}

	//
	*pcontrol_size = nControlSize;
	*pout2_size = nOutSize2;
	DbgPrint("reverse_coding_netflow_field_u16=====nControlSize: %d nOneBits: %d nZeroBits: %d\n", nControlSize, nOneBits, nZeroBits);

	return 0;
}






int reverse_coding_netflow_field_file(char* input_filename1, char* input_filename2, char* output_filename1, char* output_filename2, int width, char* output_filename_control)
{
    if (UTIL_isDirectory(input_filename1))
    {
        printf("%s is a directory--ignored!\n", input_filename1);
        return -1;
    }
	
    if (UTIL_isDirectory(input_filename2))
    {
        printf("%s is a directory--ignored!\n", input_filename2);
        return -1;
    }
	
	int srcSize1 = UTIL_getFileSize(input_filename1);
	int srcSize2 = UTIL_getFileSize(input_filename2);
	if (srcSize1 != srcSize2)
	{
		printf("File sizes are not equal! srcSize1: %d srcSize2: %d\n", srcSize1, srcSize2);
		return -1;
	}

	if (srcSize1 % width != 0 || srcSize2 % width != 0 )
	{
		printf("File sizes are n * %d records! srcSize1: %d srcSize2: %d\n", width, srcSize1, srcSize2);
		return -1;		
	}

	int nRecords = srcSize1 / width;
	
    FILE* srcFile1 = FIO_openSrcFile(input_filename1);
    if (srcFile1 == NULL)
    {
        return -1;
    }

    FILE* srcFile2 = FIO_openSrcFile(input_filename2);
    if (srcFile2 == NULL)
    {
        return -1;
    }

	char* input_buffer1 = malloc(srcSize1);
	if (!input_buffer1)
	{
		printf("Allocate input_buffer memory error!\n");
		return -1;
	}

	char* input_buffer2 = malloc(srcSize2);
	if (!input_buffer2)
	{
		printf("Allocate input_buffer memory error!\n");
		return -1;
	}

	//
	if (UTIL_readFile(srcFile1, input_buffer1, srcSize1) < 0)
	{
		return -1;
	}

	if (UTIL_readFile(srcFile2, input_buffer2, srcSize2) < 0)
	{
		return -1;
	}

	//
	char* output_buffer1 = input_buffer1;
	char* output_buffer2 = malloc(srcSize2);
	if (!output_buffer2)
	{
		printf("Allocate output_buffer2 memory error!\n");
		return -1;
	}

	int nControl = (nRecords - nRecords % 8) / 8 + 1; //zb: control bytes;
	//DbgPrint("nControl: %d\n", nControl);

	//
	char* control_buffer = malloc(nControl);
	if (!control_buffer)
	{
		printf("Allocate control_buffer memory error!\n");
		return -1;
	}
	memset(control_buffer, 0, nControl);


	//=================================================
	int nControlSize = nControl;
	int output_size1 = srcSize1;
	int output_size2;
	if (width == 4)
	{
		if (reverse_coding_netflow_field_u32(input_buffer1, input_buffer2, nRecords, control_buffer, &nControlSize, output_buffer1, output_buffer2, &output_size2))
		{
			printf("reverse_coding_netflow_v5_field_u32 error!\n");		
			return -1;
		}
	}
	else if (width == 2)
	{
		if (reverse_coding_netflow_field_u16(input_buffer1, input_buffer2, nRecords, control_buffer, &nControlSize, output_buffer1, output_buffer2, &output_size2))
		{
			printf("reverse_coding_netflow_v5_field_u16 error!\n");		
			return -1;
		}		
	}
	else
	{
			printf("reverse_coding_netflow_v5_field_invlalid error!\n");		
			return -1;		
	}


	//==================================================
	if (nControlSize > 0)
	{
		FILE* controlFile = FIO_openDstFile(output_filename_control);
		if (UTIL_writeFile(controlFile, control_buffer, nControlSize))
		{
				return -1;
		}
		fclose(controlFile);
	}

	FILE* dstFile1 = FIO_openDstFile(output_filename1);
	if (UTIL_writeFile(dstFile1, output_buffer1, srcSize1))
	{
			return -1;
	}
	fclose(dstFile1);


	FILE* dstFile2 = FIO_openDstFile(output_filename2);
	if (UTIL_writeFile(dstFile2, output_buffer2, output_size2))
	{
			return -1;
	}
	fclose(dstFile2);

	fclose(srcFile1);
	fclose(srcFile2);

	//
	DbgPrint("\nSummary===srcSize1: %d srcSize2: %d origSize: %d\n", srcSize1, srcSize2, srcSize1 + srcSize2);
	DbgPrint("========nControlSize: %d output_size1: %d output_size2: %d reversedCodingSize: %d\n\n", nControlSize, output_size1, output_size2, nControlSize + output_size1 + output_size2);

	return 0;
}



int nfc_reverse_coding(FIELD_DESC* pfield_desc, int field_num)
{
/*	
	for (int i = 0; i < field_num; i++)
	{
		DbgPrint("nfc_reverse_coding===FieldDesc%.2d [name: %30s \twidth: %d \treverse_coding: %d \treverse_number: %d \tdelta_coding: %d]\n", i, pfield_desc[i].name, pfield_desc[i].width, pfield_desc[i].reverse_coding, pfield_desc[i].reverse_number, pfield_desc[i].delta_coding);
	}
*/		
	//zb: find the appropriate fields
	int i, j;
	for (i = 0; i < field_num; i++)
	{
		if (pfield_desc[i].reverse_coding == TRUE)
		{
			pfield_desc[i].reverse_master = TRUE;
		}
	}

	for (i = 0; i < field_num; i++)
	{
		if (pfield_desc[i].reverse_coding == TRUE && pfield_desc[i].reverse_master == TRUE)
		{
			for (j = i + 1; j < field_num; j++)
			{
				if (pfield_desc[j].reverse_coding == TRUE && pfield_desc[j].reverse_number == pfield_desc[i].reverse_number)
				{
					if (pfield_desc[j].width != pfield_desc[i].width)
					{
						printf("Width does not match in reverse_coding! name: %s width: %d, name: %s width: %d\n", pfield_desc[i].name, pfield_desc[i].width, pfield_desc[j].name, pfield_desc[j].width);
						return -1;
					}
					//printf("Find Match! i: %d j: %d\n", i, j);
					//pfield_desc[j].reverse_coding = FALSE;
					pfield_desc[j].reverse_master = FALSE;
					if (nfc_reverse_coding_field(pfield_desc + i, pfield_desc + j))
					{
						return -1;
					}
					break;
				}
			}
			
		}
	}
	DbgPrint("\n");
	return 0;
}


int nfc_reverse_coding_field(FIELD_DESC* pfield_desc1, FIELD_DESC* pfield_desc2)
{
	int nRecords = pfield_desc1->in_len / pfield_desc1->width;
	char* control_buffer = malloc(nRecords);
	if (control_buffer == NULL)
	{
		printf("Allocate control_buffer error!\n");
		return -1;
	}

	int nControlSize;
	int nOutSize;
	if (pfield_desc1->width == 4)
	{
		reverse_coding_netflow_field_u32(pfield_desc1->in_ptr, pfield_desc2->in_ptr, nRecords, control_buffer, &nControlSize, pfield_desc1->in_ptr, pfield_desc2->in_ptr, &nOutSize);
		pfield_desc1->control_ptr = control_buffer;
		pfield_desc1->control_len = nControlSize;
		pfield_desc2->in_len = nOutSize;
	}
	else if (pfield_desc1->width == 2)
	{
		reverse_coding_netflow_field_u16(pfield_desc1->in_ptr, pfield_desc2->in_ptr, nRecords, control_buffer, &nControlSize, pfield_desc1->in_ptr, pfield_desc2->in_ptr, &nOutSize);
		pfield_desc1->control_ptr = control_buffer;
		pfield_desc1->control_len = nControlSize;
		pfield_desc2->in_len = nOutSize;
	}
	else
	{
		printf("Width %d is not supported in reverse_coding!\n", pfield_desc1->width);
		return -1;	
	}
	
	return 0;
}


int nfc_reverse_decoding(FIELD_DESC* pfield_desc, int field_num)
{
	int i, j;
	for (i = 0; i < field_num; i++)
	{
		if (pfield_desc[i].reverse_coding == TRUE && pfield_desc[i].reverse_master == TRUE)
		{
			for (j = i + 1; j < field_num; j++)
			{
				if (pfield_desc[j].reverse_coding == TRUE && pfield_desc[j].reverse_master == FALSE && pfield_desc[j].reverse_number == pfield_desc[i].reverse_number)
				{
					if (pfield_desc[j].width != pfield_desc[i].width)
					{
						printf("nfc_reverse_decoding===Width does not match in reverse_coding! name: %s width: %d, name: %s width: %d\n", pfield_desc[i].name, pfield_desc[i].width, pfield_desc[j].name, pfield_desc[j].width);
						return -1;
					}
					//DbgPrint("nfc_reverse_decoding===Find Match! i: %d j: %d\n", i, j);
					if (nfc_reverse_decoding_field(pfield_desc + i, pfield_desc + j))
					{
						return -1;
					}
					break;
				}
			}
			
		}
	}
	DbgPrint("\n");

	return 0;
}


int nfc_reverse_decoding_field(FIELD_DESC* pfield_desc1, FIELD_DESC* pfield_desc2)
{
	//
	int width = pfield_desc2->width;
	int nRecords = pfield_desc1->n_records;
	int out_len = nRecords * width;
	char* ptr = malloc(out_len);
	if (ptr == NULL)
	{
		printf("Allocate memrory in nfc_reverse_decoding_field_func error!\n");
		return -1;
	}
	pfield_desc2->out_ptr = ptr;
	pfield_desc2->out_len = out_len;

	if (width == 4)
	{
		if (nfc_reverse_decoding_netflow_field_u32(pfield_desc1->in_ptr, pfield_desc2->in_ptr, nRecords, pfield_desc1->control_ptr, pfield_desc1->control_len, pfield_desc2->out_ptr))
		{
			return -1;
		}
	}
	else if (width == 2)
	{
		if (nfc_reverse_decoding_netflow_field_u16(pfield_desc1->in_ptr, pfield_desc2->in_ptr, nRecords, pfield_desc1->control_ptr, pfield_desc1->control_len, pfield_desc2->out_ptr))
		{
			return -1;
		}
	}
	else
	{
		
	}

	//
	pfield_desc1->out_ptr = pfield_desc1->in_ptr;
	pfield_desc1->out_len = pfield_desc1->in_len;
	pfield_desc2->out_ptr = ptr;
	pfield_desc2->out_len = nRecords * width;
	return 0;
}


int nfc_reverse_decoding_netflow_field_u32(U32* in1, U32* in2, int nRecords, char* control_buffer, int control_size, U32* out2)
{
	U32* ptr1 = in1;
	U32* ptr2 = in2;
	U32* ptr2_out = out2;
	//
	int nRecordsCount = 0;
	int i;
	BYTE v;
	BYTE* ptr_control = control_buffer;
	BYTE* ptr_end = ptr_control + control_size;
	int nOneBits = 0;
	int nZeroBits = 0;
	while (ptr_control < ptr_end)
	{
		v = *ptr_control;
		for (i = 0; i < 8; i++)
		{
			if (nRecordsCount >= nRecords) //zb: Avoid redundant bits
			{
				break;
			}
			//
			if (v & (1 << (8 - i - 1)))
			{
				*ptr2_out = *(ptr1 + 1);
				*ptr2_out++;
				*ptr2_out = *ptr1;
				*ptr2_out++;
				ptr1 += 2;
				nRecordsCount += 2;
				nOneBits++;
				
			}
			else
			{
				ptr1++;
				*ptr2_out = *ptr2;
				ptr2++;
				ptr2_out++;
				nRecordsCount += 1;
				nZeroBits++;
			}
		}
		ptr_control++;
		
	}

	int nDecoded = ptr2_out - out2;
	int nOffset = ptr1 - in1;
	DbgPrint("nfc_reverse_decoding_netflow_field_u32===nRecords: %d control_size: %d nOneBits: %d nZeroBits: %d\n", nRecords, control_size, nOneBits, nZeroBits);	

	if (nDecoded != nRecords)
	{
		printf("nfc_reverse_decoding_netflow_field_u32===decode error! nRecords: %d nDecoded: %d\n", nRecords, nDecoded);
		return -1;
	}
	return 0;

}


int nfc_reverse_decoding_netflow_field_u16(U16* in1, U16* in2, int nRecords, char* control_buffer, int control_size, U16* out2)
{
	U16* ptr1 = in1;
	U16* ptr2 = in2;
	U16* ptr2_out = out2;
	//
	int nRecordsCount = 0;
	int i;
	BYTE v;
	BYTE* ptr_control = control_buffer;
	BYTE* ptr_end = ptr_control + control_size;
	int nOneBits = 0;
	int nZeroBits = 0;
	while (ptr_control < ptr_end)
	{
		v = *ptr_control;
		for (i = 0; i < 8; i++)
		{
			if (nRecordsCount >= nRecords) //zb: Avoid redundant bits
			{
				break;
			}
			//
			if (v & (1 << (8 - i - 1)))
			{
				*ptr2_out = *(ptr1 + 1);
				*ptr2_out++;
				*ptr2_out = *ptr1;
				*ptr2_out++;
				ptr1 += 2;
				nRecordsCount += 2;
				nOneBits++;
				
			}
			else
			{
				ptr1++;
				*ptr2_out = *ptr2;
				ptr2++;
				ptr2_out++;
				nRecordsCount += 1;
				nZeroBits++;
			}
		}
		ptr_control++;
		
	}

	int nDecoded = ptr2_out - out2;
	int nOffset = ptr1 - in1;
	DbgPrint("nfc_reverse_decoding_netflow_field_u16===nRecords: %d control_size: %d nOneBits: %d nZeroBits: %d\n", nRecords, control_size, nOneBits, nZeroBits);	

	if (nDecoded != nRecords)
	{
		printf("nfc_reverse_decoding_netflow_field_u16===decode error! nRecords: %d nDecoded: %d\n", nRecords, nDecoded);
		return -1;
	}
	return 0;
}



