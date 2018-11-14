
/*
	reverse_coding.c
*/


#include "debug.h"
#include "file_op.h"
#include "netflow_v5.h"



int reverse_coding_netflow_field_u32(char* in1, char* in2, int nRecords, char* control_buffer, int* pcontrol_size, char* out1, char* out2, int* pout2_size)
{
	DbgPrint("reverse_coding_netflow_v5_field_u32=====nRecords: %d nBytes: %d\n", nRecords, nRecords * 4);
	
	U32 v1a, v1b, v2a, v2b;
	U32* ptr1 = (U32*)in1;
	U32* ptr_end = ptr1 + nRecords;
	U32* ptr2 = (U32*)in2;
	U32* ptr_out2 = (U32*)out2;
	U32 nOutSize2 = 0;

	//
	BYTE* ptr_control = control_buffer;
	BYTE vControl = 0;
	U32  nShiftCount = 0;
	U32  nControlSize = 0;

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
			vControl |= 1;
			ptr1 += 2;
			ptr2 += 2;
		}
		else
		{
			*ptr_out2 = *ptr2;
			nOutSize2 += 4; //zb: bytes len measure
			ptr1 += 1;
			ptr2 += 1;
			ptr_out2 += 1;
		}

		//======================
		if (nShiftCount < 7)
		{
			vControl <<= 1;
			nShiftCount++;			
		}
		else
		{
			//
			*ptr_control = vControl;
			nControlSize++;
			ptr_control++;
			vControl = 0;
			nShiftCount = 0;
		}
	}

	//
	*pcontrol_size = nControlSize;
	*pout2_size = nOutSize2;
	DbgPrint("reverse_coding_netflow_v5_field_u32=====nControlSize: %d nOutSize1: %d nOutSize2: %d\n", nControlSize, nRecords * 4, nOutSize2);

	return 0;
}

int reverse_coding_netflow_field_u16(char* in1, char* in2, int nRecords, char* control_buffer, int* pcontrol_size, char* out1, char* out2, int* pout2_size)
{
	DbgPrint("reverse_coding_netflow_v5_field_u16=====nRecords: %d nBytes: %d\n", nRecords, nRecords * 2);
	
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
			vControl |= 1;
			ptr1 += 2;
			ptr2 += 2;
		}
		else
		{
			*ptr_out2 = *ptr2;
			nOutSize2 += 2; //zb: bytes len measure
			ptr1 += 1;
			ptr2 += 1;
			ptr_out2 += 1;
		}

		//======================
		if (nShiftCount < 7)
		{
			vControl <<= 1;
			nShiftCount++;		
		}
		else
		{
			//
			*ptr_control = vControl;
			nControlSize++;
			ptr_control++;
			vControl = 0;
			nShiftCount = 0;
		}
	}

	//
	*pcontrol_size = nControlSize;
	*pout2_size = nOutSize2;
	DbgPrint("reverse_coding_netflow_v5_field_u16=====nControlSize: %d nOutSize1: %d nOutSize2: %d\n", nControlSize, nRecords * 2, nOutSize2);

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



int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 4)
	{
		printf("Usage: ./program input_filename_a input_filename_b width(bytes)\n");
		return -1;
	}

	int width = atoi(argv[3]);
	if (width == 0)
	{
		printf("Convert width to int error! width: %s.\n", argv[3]);
		return -1;
	}

	char* input_filename1 = argv[1];
	char* input_filename2 = argv[2];	



	char output_filename1[MAX_PATHNAME_LEN];
	char output_filename2[MAX_PATHNAME_LEN];	
	char output_filename_control[MAX_PATHNAME_LEN];	
	memset(output_filename1, 0, MAX_PATHNAME_LEN);
	memset(output_filename2, 0, MAX_PATHNAME_LEN);
	memset(output_filename_control, 0, MAX_PATHNAME_LEN);
	sprintf(output_filename1, "%s_r", input_filename1);
	sprintf(output_filename2, "%s_r", input_filename2);

	/*
	if (width == 4)
	{
		sprintf(output_filename1, "%s_r4", input_filename1);
		sprintf(output_filename2, "%s_r4", input_filename2);	
	}
	else if (width == 2)
	{
		sprintf(output_filename1, "%s_r2", input_filename1);
		sprintf(output_filename2, "%s_r2", input_filename2);			
	}
	else
	{
		printf("Invalid width: %d. Width must be 4 or 2 bytes!\n", width);
		return -1;		
	}
	*/

	sprintf(output_filename_control, "%s_control", input_filename1);


	//
	if (reverse_coding_netflow_field_file(input_filename1, input_filename2, output_filename1, output_filename2, width, output_filename_control))
	{
		return -1;
	}

	
	DbgPrint("%s main end.\n", argv[0]);
	return 0;
}



