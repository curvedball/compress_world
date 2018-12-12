
/*
	approximation_coding.c
*/

#include "approximation_coding.h"


int approximation_coding_u16(char* input_filename, int width)
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
	DbgPrint("approximation_coding===input_filename: %s srcSize: %d\n", input_filename, srcSize);


	//-------------------------------------------------------------
	int nRecords = srcSize / width;
	U16* ptr_in_end = input_buffer + srcSize;
	U16* ptr_in = input_buffer;

	//
	U16* tmp_buffer = malloc(srcSize * 2);
	if (!tmp_buffer)
	{
		printf("Allocate tmp_buffer memory error!\n");
		return -1;
	}
	int tmp_len = 0;
	U16* ptr_tmp = tmp_buffer;

	//
	U16* out_buffer = malloc(srcSize * 2);
	if (!out_buffer)
	{
		printf("Allocate out_buffer memory error!\n");
		return -1;
	}
	int out_len = 0;
	U16* ptr_out = out_buffer;


	//============================================
	U16 va, vb, vc, delta;
	va = ntohs(*ptr_in);
	ptr_in++;
	vb = ntohs(*ptr_in);
	ptr_in++;
	vc = ntohs(*ptr_in);
	ptr_in++;
	delta = 2 * vb - va - vc;

	*ptr_tmp++ = va;
	*ptr_tmp++ = vb;
	*ptr_tmp++ = delta;

	while (ptr_in < ptr_in_end)
	{
		va = vb;
		vb = vc;
		vc = ntohs(*ptr_in);
		ptr_in++;		
		delta = 2 * vb - va - vc;
		*ptr_tmp++ = delta;
	}
	tmp_len = ptr_tmp - tmp_buffer;
	printf("nRecords: %d tmp_len: %d\n", nRecords, tmp_len);

	//-------------------------------------------------------------
	char* tmp_filename = malloc(MAX_PATHNAME_LEN);
	if (tmp_filename == NULL)
    {
		printf("Allocate tmp_filename error!\n");
        return -1;
    }
	sprintf(tmp_filename, "%s_tmp", input_filename);
	
	FILE* tmpFile = FIO_openDstFile(tmp_filename);
	if (!tmpFile)
	{
		printf("Write tmpFile error!\n");
    	return -1;
	}
	
	if (UTIL_writeFile(tmpFile, (char*)tmp_buffer, tmp_len * 2))
	{
		return -1;
	}
	fclose(tmpFile);

	return 0;
}


#if 0
#define HIGHT_DELTA_THRESHOLD 16
#define LOW_DELTA_THRESHOLD (-HIGHT_DELTA_THRESHOLD)


int approximation_coding_u16_advanced_xxx(char* input_filename, int width)
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
	DbgPrint("approximation_coding===input_filename: %s srcSize: %d\n", input_filename, srcSize);


	//-------------------------------------------------------------
	int nRecords = srcSize / width;
	U16* ptr_in_end = input_buffer + srcSize;
	U16* ptr_in = input_buffer;

	//Btimap
	BYTE* bm_buffer = malloc(srcSize);
	if (!bm_buffer)
	{
		printf("Allocate bm_buffer memory error!\n");
		return -1;
	}
	int bm_len = 0;
	BYTE* ptr_bm = bm_buffer;
	
	//OriginalValue
	U16* ov_buffer = malloc(srcSize * 2);
	if (!ov_buffer)
	{
		printf("Allocate ov_buffer memory error!\n");
		return -1;
	}
	int ov_len = 0;
	U16* ptr_ov = ov_buffer;

	//DeltaValue
	U16* dv_buffer = malloc(srcSize * 2);
	if (!dv_buffer)
	{
		printf("Allocate dv_buffer memory error!\n");
		return -1;
	}
	int dv_len = 0;
	U16* ptr_dv = dv_buffer;


	//============================================
	U16 va, vb, vc;
	S16 delta, delta1, delta2;
	U16 stub_va, stub_vb;
	int flag_stub_ok = 0;


	//
	va = ntohs(*ptr_in);
	ptr_in++;
	vb = ntohs(*ptr_in);
	ptr_in++;
	vc = ntohs(*ptr_in);
	ptr_in++;
	delta = 2 * vb - va - vc;
	delta1 = vb - va;
	delta2 = vc - vb;

	*ptr_ov++ = va;
	*ptr_ov++ = vb;
	if (delta > LOW_DELTA_THRESHOLD && delta < HIGHT_DELTA_THRESHOLD
			&& delta1 > LOW_DELTA_THRESHOLD && delta1 < HIGHT_DELTA_THRESHOLD
			&& delta2 > LOW_DELTA_THRESHOLD && delta2 < HIGHT_DELTA_THRESHOLD)
	{
		*ptr_dv++ = delta;
		stub_va = va;
		stub_vb = vb;
		flag_stub_ok = 1;
	}
	else
	{
		*ptr_ov++ = vc;
	}


	while (ptr_in < ptr_in_end)
	{
		if (flag_stub_ok == 0)
		{
			va = vb;
			vb = vc;
			vc = ntohs(*ptr_in);
			ptr_in++;		
		}
		else
		{
			
		}
	
		delta = 2 * vb - va - vc;
		delta1 = vb - va;
		delta2 = vc - vb;
		if (delta > LOW_DELTA_THRESHOLD && delta < HIGHT_DELTA_THRESHOLD 
			&& delta1 > LOW_DELTA_THRESHOLD && delta1 < HIGHT_DELTA_THRESHOLD
			&& delta2 > LOW_DELTA_THRESHOLD && delta2 < HIGHT_DELTA_THRESHOLD)
		{
			*ptr_dv++ = delta;
			stub_va = va;
			stub_vb = vb;
			flag_stub_ok = 1;
		}
		else
		{
			if (flag_stub_ok)
			{
				//go ahead in 16 values
				for (int i = 1; i < 10; i++)
				{
					vc = *(ptr_in+i);
					if ()
				}
			}
			else
			{
				*ptr_ov++ = vc;
			}	
		}
		
	}
	tmp_len = ptr_tmp - tmp_buffer;
	printf("nRecords: %d tmp_len: %d\n", nRecords, tmp_len);

	//-------------------------------------------------------------
	char* tmp_filename = malloc(MAX_PATHNAME_LEN);
	if (tmp_filename == NULL)
    {
		printf("Allocate tmp_filename error!\n");
        return -1;
    }
	sprintf(tmp_filename, "%s_tmp", input_filename);
	
	FILE* tmpFile = FIO_openDstFile(tmp_filename);
	if (!tmpFile)
	{
		printf("Write tmpFile error!\n");
    	return -1;
	}
	
	if (UTIL_writeFile(tmpFile, (char*)tmp_buffer, tmp_len * 2))
	{
		return -1;
	}
	fclose(tmpFile);

	return 0;
}
#endif




#define HIGHT_DELTA_THRESHOLD 16
#define LOW_DELTA_THRESHOLD (-HIGHT_DELTA_THRESHOLD)

int approximation_coding_u16_advanced(char* input_filename, int width)
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
	fclose(srcFile);
	DbgPrint("delta_coding===input_filename: %s srcSize: %d\n", input_filename, srcSize);


	//==================step1: calcluate delta values======================
	char* out_buffer = malloc(srcSize);
	if (out_buffer == NULL)
    {
		printf("Allocate out_buffer error!\n");
        return -1;
    }

	U16* ptr_in_end = (U16*)(input_buffer + srcSize);
	U16* ptr_in = input_buffer;
	S16* ptr_out = out_buffer;

	*ptr_out++ = *ptr_in++;
	while (ptr_in < ptr_in_end)
	{
		*ptr_out = htons(ntohs(*ptr_in) - ntohs(*(ptr_in - 1)));
		ptr_in += 1;
		ptr_out += 1;
	}
	S16* ptr_out_end = ptr_out;

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


	//==================step2: exclude anomaly values======================
	//Btimap
	BYTE* bm_buffer = malloc(srcSize);
	if (!bm_buffer)
	{
		printf("Allocate bm_buffer memory error!\n");
		return -1;
	}
	int bm_len = 0;
	BYTE* ptr_bm = bm_buffer;
	
	//OriginalValue
	U16* ov_buffer = malloc(srcSize * 2);
	if (!ov_buffer)
	{
		printf("Allocate ov_buffer memory error!\n");
		return -1;
	}
	int ov_len = 0;
	U16* ptr_ov = ov_buffer;

	//DeltaValue
	S16* dv_buffer = malloc(srcSize * 2);
	if (!dv_buffer)
	{
		printf("Allocate dv_buffer memory error!\n");
		return -1;
	}
	int dv_len = 0;
	S16* ptr_dv = dv_buffer;


	ptr_in = input_buffer;
	ptr_out = out_buffer;
	S16 v1, v2, vstub;
	vstub = *ptr_in;
	while (ptr_out < ptr_out_end)
	{
		v1 = *ptr_in;
		v2 = *ptr_out;
		if (v2 > LOW_DELTA_THRESHOLD && v2 < HIGHT_DELTA_THRESHOLD)
		{
			*ptr_dv++ = v2;
			vstub = v1;
		}
		else
		{
			if ((v1 - vstub) > LOW_DELTA_THRESHOLD && (v1 - vstub) < HIGHT_DELTA_THRESHOLD)
			{
				*ptr_dv++ = v1 - vstub;
				vstub = v1;
			}
			else
			{
				*ptr_ov++ = v1;
			}
		}
		ptr_out += 1;
		ptr_in += 1;
	}

	return 0;
}




