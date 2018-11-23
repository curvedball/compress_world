
/*
  zstd_test.c

*/


#include "zstd_api.h"
#include "file_op.h"



int zstd_compress_file_test(char* srcFileName)
{
    U64 const fileSize = UTIL_getFileSize(srcFileName);
    U64 const srcSize = (fileSize == UTIL_FILESIZE_UNKNOWN) ? ZSTD_CONTENTSIZE_UNKNOWN : fileSize;

	char dstFileName[MAX_PATHNAME_LEN];
	sprintf(dstFileName, "%s.zst", srcFileName);

	//
	clock_t start = clock();

	//
	ZSTD_compressionParameters compressionParams;
	memset(&compressionParams, 0, sizeof(compressionParams));
    cRess_t ress = FIO_createCResources(NULL, ZSTDCLI_CLEVEL_DEFAULT, srcSize, &compressionParams);


	//
    int result = FIO_compressFilename_dstFile(ress, dstFileName, srcFileName, ZSTDCLI_CLEVEL_DEFAULT);

    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("Completed in %.2f sec \n", seconds);

    FIO_freeCResources(ress);
    return result;
}


int zstd_decompress_file_test(char* srcFileName)
{
	int len = strlen(srcFileName);
	if (len < 5)
	{
		printf("Filename is too short! len: %d\n", len);
		return -1;
	}
	char* ptr = srcFileName + len - 4;
	if (memcmp(ptr, ".zst", 4) != 0)
	{
		printf("File extension name is not zst! Filename: %s\n", srcFileName);
		return -1;
	}
	
	char dstFileName[MAX_PATHNAME_LEN];
	memset(dstFileName, 0, MAX_PATHNAME_LEN);
	memcpy(dstFileName, srcFileName, ptr - srcFileName);

	//
	clock_t start = clock();

	//
	dRess_t ress = FIO_createDResources(NULL);
    int decodingError = FIO_decompressDstFile(ress, dstFileName, srcFileName);

	double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("Completed in %.2f sec \n", seconds);
	
    FIO_freeDResources(ress);
    return decodingError;
}




int zstd_compress_decompress_small_data_test()
{
	int input_len = 5;
	char* input_buffer = malloc(input_len + 1);
	if (input_buffer == NULL)
	{
		printf("Allocate input_buffer error!\n");
		return -1;
	}

	int output_len = input_len + 100;
	char* output_buffer = malloc(output_len);
	if (output_buffer == NULL)
	{
		printf("Allocate output_buffer error!\n");
		return -1;
	}

	input_buffer[0] = 'a';
	input_buffer[1] = 'b';
	input_buffer[2] = 'c';
	input_buffer[3] = 'd';
	input_buffer[4] = 'e';
	input_buffer[5] = 0;


	//
	clock_t start = clock();

	//
	ZSTD_compressionParameters compressionParams;
	memset(&compressionParams, 0, sizeof(compressionParams));
    cRess_t ress = FIO_createCResources(NULL, ZSTDCLI_CLEVEL_DEFAULT, input_len, &compressionParams);
	int compressedSize = FIO_compressData(ress, input_buffer, input_len, output_buffer, output_len, ZSTDCLI_CLEVEL_DEFAULT);
	FIO_freeCResources(ress);
	//
    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("Compress completed in %.2f sec \n", seconds);


	//=====================================
	int output2_len = input_len + 1;
	char* output2_buffer = malloc(output2_len);
	if (output2_buffer == NULL)
	{
		printf("Allocate output2_buffer error!\n");
		return -1;
	}
	memset(output2_buffer, 0, output2_len);

	start = clock();

	dRess_t dRess = FIO_createDResources(NULL);
	//int decompressedSize = FIO_decompressData(dRess, output_buffer, compressedSize, output2_buffer, output2_len);
	int decompressedSize = FIO_decompressDataNoCopy(dRess, output_buffer, compressedSize, output2_buffer, output2_len);

	//
    seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("Decompress completed in %.2f sec \n", seconds);

    FIO_freeDResources(dRess);
	printf("output2_buffer: %s---end---\n", output2_buffer);

    return 0;
}



int zstd_compress_decompress_large_data_test()
{
	int input_len = 102400;
	char* input_buffer = malloc(input_len);
	if (input_buffer == NULL)
	{
		printf("Allocate input_buffer error!\n");
		return -1;
	}

	int output_len = input_len + 1024;
	char* output_buffer = malloc(output_len);
	if (output_buffer == NULL)
	{
		printf("Allocate output_buffer error!\n");
		return -1;
	}

	//
	int i;
	for (i = 0; i < input_len; i++)
	{
		if (i % 10 == 0)
		{
			input_buffer[i] = (char)i;
		}
		else
		{
			input_buffer[i] = (char)(i - i % 10);
		}
	}

	//
	clock_t start = clock();
	//
	ZSTD_compressionParameters compressionParams;
	memset(&compressionParams, 0, sizeof(compressionParams));
    cRess_t ress = FIO_createCResources(NULL, ZSTDCLI_CLEVEL_DEFAULT, input_len, &compressionParams);
	int compressedSize = FIO_compressData(ress, input_buffer, input_len, output_buffer, output_len, ZSTDCLI_CLEVEL_DEFAULT);
	FIO_freeCResources(ress);
	//
    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("Compress completed in %.2f sec\n\n", seconds);


	//=====================================
	int output2_len = input_len;
	char* output2_buffer = malloc(output2_len);
	if (output2_buffer == NULL)
	{
		printf("Allocate output2_buffer error!\n");
		return -1;
	}
	memset(output2_buffer, 0, output2_len);

	start = clock();
	dRess_t dRess = FIO_createDResources(NULL);
	//int decompressedSize = FIO_decompressData(dRess, output_buffer, compressedSize, output2_buffer, output2_len);
	int decompressedSize = FIO_decompressDataNoCopy(dRess, output_buffer, compressedSize, output2_buffer, output2_len);
	FIO_freeCResources(ress);
	//
    seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("Decompress completed in %.2f sec \n", seconds);

    FIO_freeDResources(dRess);

	//zb: compare data test
	if (input_len != decompressedSize)
	{
		printf("Error! input_len is not equal to decompressedSize!\n");
		return -1;	
	}

	for (i = 0; i < input_len; i++)
	{
		if (input_buffer[i] != output2_buffer[i])
		{
			printf("Error! orig data is different from the decompressed data!\n");
			return -1;
		}
	}
	printf("Check data OK!\n");	

    return 0;
}



int zstd_compress_decompress_huge_data_test()
{
	//int input_len = 1024000; //zb: test ok
	//int input_len = 10240000; //zb: test ok
	//int input_len = 102400000; //zb: test ok, 0.02s
	int input_len = 1024000000; //zb: test ok, 0.19s	
	char* input_buffer = malloc(input_len);
	if (input_buffer == NULL)
	{
		printf("Allocate input_buffer error!\n");
		return -1;
	}

	int output_len = input_len + 1024;
	char* output_buffer = malloc(output_len);
	if (output_buffer == NULL)
	{
		printf("Allocate output_buffer error!\n");
		return -1;
	}

#if 0
	//
	int i;
	for (i = 0; i < input_len; i++)
	{
		if (i % 10 == 0)
		{
			input_buffer[i] = (char)i;
		}
		else
		{
			input_buffer[i] = (char)(i - i % 10);
		}
	}
#else
	//
	int i;
	for (i = 0; i < input_len; i++)
	{
		if (i % 5 == 0)
		{
			//input_buffer[i] = (char)i;
			input_buffer[i] = i % 256;
		}
		else
		{
			input_buffer[i] = (char)i % 256;
			//input_buffer[i] = (char)(i - i % 5);
		}
	}
#endif




	//
	clock_t start = clock();

	//
	ZSTD_compressionParameters compressionParams;
	memset(&compressionParams, 0, sizeof(compressionParams));
    cRess_t ress = FIO_createCResources(NULL, ZSTDCLI_CLEVEL_DEFAULT, input_len, &compressionParams);
	int compressedSize = FIO_compressData(ress, input_buffer, input_len, output_buffer, output_len, ZSTDCLI_CLEVEL_DEFAULT);

	//
    double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("Compress completed in %.2f sec\n\n", seconds);


	//=====================================
	int output2_len = input_len;
	char* output2_buffer = malloc(output2_len);
	if (output2_buffer == NULL)
	{
		printf("Allocate output2_buffer error!\n");
		return -1;
	}
	memset(output2_buffer, 0, output2_len);


	start = clock();
	dRess_t dRess = FIO_createDResources(NULL);
	//int decompressedSize = FIO_decompressData(dRess, output_buffer, compressedSize, output2_buffer, output2_len);
	int decompressedSize = FIO_decompressDataNoCopy(dRess, output_buffer, compressedSize, output2_buffer, output2_len);

    seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("Decompress completed in %.2f sec \n", seconds);

    FIO_freeDResources(dRess);

	//zb: compare data test
	if (input_len != decompressedSize)
	{
		printf("Error! input_len is not equal to decompressedSize!\n");
		return -1;	
	}

	for (i = 0; i < input_len; i++)
	{
		if (input_buffer[i] != output2_buffer[i])
		{
			printf("Error! orig data is different from the decompressed data!\n");
			return -1;
		}
	}
	printf("Check data OK!\n");	

    return 0;
}




int main(int argc , char* argv[])
{
	DbgPrint("%s main begin.\n", argv[0]);
	if (argc != 3 && argc != 1)
	{
		printf("Usage: ./program opration(c or d) input_file\n");
		printf("       ./program\n");
		return -1;
	}
	
	if (argc == 3)
	{
		if (memcmp(argv[1], "c", 1) == 0)
		{
			if (zstd_compress_file_test(argv[2]))
			{
				return -1;
			}
		}
		else if (memcmp(argv[1], "d", 1) == 0)
		{
			if (zstd_decompress_file_test(argv[2]))
			{
				return -1;
			}		
		}	
		else
		{
			printf("The first parameter must be c or d! [compress or decompress]\n");
			return -1;
		}
	}
	else if (argc == 1)
	{
#if 0
		if (zstd_compress_decompress_small_data_test())
		{
			return -1;
		}
#endif


#if 0
		if (zstd_compress_decompress_large_data_test())
		{
			return -1;
		}	
#endif


#if 1
		if (zstd_compress_decompress_huge_data_test())
		{
			return -1;
		}	
#endif

	}

	else
	{
		printf("argc must be 3 or 1!\n");
		return -1;
	}

	DbgPrint("%s main end.\n\n", argv[0]);
	return 0;
}




