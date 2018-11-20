

/*
	zstd_api.c
*/
#include "zstd_api.h"
#include "file_op.h"



FIO_compressionType_t g_compressionType = FIO_zstdCompression; //zbc: FIO_zstdCompression, FIO_gzipCompression
U32 g_memLimit = (U32)1 << 27;
U32 g_sparseFileSupport = 0;
#define COMPRESS_BLOCK_LEN (128*1024)



cRess_t FIO_createCResources(const char* dictFileName, int cLevel, U64 srcSize, ZSTD_compressionParameters* comprParams)
{
    cRess_t ress;
    memset(&ress, 0, sizeof(ress));

    ress.cctx = ZSTD_createCCtx(); //zb: Create a struct to init the parameters!
    if (ress.cctx == NULL)
    {
        printf("allocation error : can't create ZSTD_CCtx");
		exit(-1);
    }

	//
    ress.srcBufferSize = ZSTD_CStreamInSize(); //zb: 128K
    ress.srcBuffer = malloc(ress.srcBufferSize);
    ress.dstBufferSize = ZSTD_CStreamOutSize(); //zb: 128K+64K+3+4
    ress.dstBuffer = malloc(ress.dstBufferSize);
    if (!ress.srcBuffer || !ress.dstBuffer)
    {
        printf("allocation error : not enough memory");
		exit(-1);
    }

	//
    /* Advanced parameters, including dictionary */
    {
		//
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_contentSizeFlag, 1);  
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_dictIDFlag, 1); //zb?
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_checksumFlag, 1); //zb?
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_compressionLevel, (unsigned)cLevel);

        /* compression parameters */
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_windowLog, comprParams->windowLog);
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_chainLog, comprParams->chainLog);
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_hashLog, comprParams->hashLog);
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_searchLog, comprParams->searchLog);
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_minMatch, comprParams->searchLength);
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_targetLength, comprParams->targetLength);
        ZSTD_CCtx_setParameter(ress.cctx, ZSTD_p_compressionStrategy, (U32)comprParams->strategy);

        /* dictionary */
        ZSTD_CCtx_setPledgedSrcSize(ress.cctx, srcSize); 
        ZSTD_CCtx_loadDictionary(ress.cctx, NULL, 0);
        ZSTD_CCtx_setPledgedSrcSize(ress.cctx, ZSTD_CONTENTSIZE_UNKNOWN);  /* reset */
    }
    return ress;
}



void FIO_freeCResources(cRess_t ress)
{
    free(ress.srcBuffer);
    free(ress.dstBuffer);

	//
    ZSTD_freeCStream(ress.cctx);   /* never fails */
}



int FIO_compressFilename_dstFile(cRess_t ress, const char* dstFileName, const char* srcFileName, int compressionLevel)
{
    int result;
    stat_t statbuf;
    int stat_result = 0;

	if (UTIL_getFileStat(srcFileName, &statbuf))
	{
		stat_result = 1;
	}

	//
    ress.dstFile = FIO_openDstFile(dstFileName);
    if (ress.dstFile == NULL)
    {
        return -1;    /* could not open dstFileName */
    }

    result = FIO_compressFilename_srcFile(ress, dstFileName, srcFileName, compressionLevel); //zbzb
    fclose(ress.dstFile);

	if (stat_result)
	{
		UTIL_setFileStat(dstFileName, &statbuf);	
	}

    return result;
}



int FIO_compressFilename_srcFile(cRess_t ress, const char* dstFileName, const char* srcFileName, int compressionLevel)
{
    int result;

	//
	//DbgPrint("zb===FIO_compressFilename_srcFile===1.0===\n\n");

    /* File check */
    if (UTIL_isDirectory(srcFileName))
    {
        printf("zstd: %s is a directory -- ignored \n", srcFileName);
        return -1;
    }


	//
    ress.srcFile = FIO_openSrcFile(srcFileName);
    if (!ress.srcFile)
    {
        return 1;    /* srcFile could not be opened */
    }

	//
	//zb: 
	//
    result = FIO_compressFilename_internal(ress, dstFileName, srcFileName, compressionLevel); //zbzb

	//
    fclose(ress.srcFile);

    return result;
}





int FIO_compressFilename_internal(cRess_t ress, const char* dstFileName, const char* srcFileName, int compressionLevel)
{
    U64 readsize = 0;
    U64 compressedfilesize = 0;
    U64 const fileSize = UTIL_getFileSize(srcFileName);

	//
	//DbgPrint("zb===FIO_compressFilename_internal===1.0===\n\n");

    /* compression format selection */
    switch (g_compressionType)
    {
        default:
        case FIO_zstdCompression:
			//
			//zb: 
			//
            compressedfilesize = FIO_compressZstdFrame(&ress, srcFileName, fileSize, compressionLevel, &readsize);
            break;

        case FIO_gzipCompression:
            //compressedfilesize = FIO_compressGzFrame(&ress, srcFileName, fileSize, compressionLevel, &readsize);
            break;
    }


    /* Status */
    printf("\r===CompressRatio===%79s\r", "");
    printf("%-20s :%6.2f%%   (%6llu => %6llu bytes, %s) \n", srcFileName, (double)compressedfilesize / (readsize + (!readsize)/*avoid div by zero*/) * 100, (unsigned long long)readsize, (unsigned long long) compressedfilesize, dstFileName);
    return 0;
}




unsigned long long FIO_compressZstdFrame(const cRess_t* ressPtr, const char* srcFileName, U64 fileSize, int compressionLevel, U64* readsize)
{
    cRess_t const ress = *ressPtr;
    FILE* const srcFile = ress.srcFile;
    FILE* const dstFile = ress.dstFile;
    U64 compressedfilesize = 0;

	//
    ZSTD_EndDirective directive = ZSTD_e_continue;

	//DbgPrint("zb===FIO_compressZstdFrame===!!!===\n\n");

    /* init */
    if (fileSize != UTIL_FILESIZE_UNKNOWN)
    {
        ZSTD_CCtx_setPledgedSrcSize(ress.cctx, fileSize);
    }

	//==================================Main compression loop===============================
    do
    {
        size_t result;
        /* Fill input Buffer */
        size_t const inSize = fread(ress.srcBuffer, (size_t)1, ress.srcBufferSize, srcFile); //zb: Read File. 128K in each read!
        ZSTD_inBuffer inBuff = 
		{ 
			ress.srcBuffer, 
			inSize, 
			0 
		};

        *readsize += inSize;

        if ((inSize == 0) || (*readsize == fileSize))
        {
            directive = ZSTD_e_end;
        }

        result = 1;

		//==============================================
        while (inBuff.pos != inBuff.size || (directive == ZSTD_e_end && result != 0))
        {
            ZSTD_outBuffer outBuff = 
			{ 
				ress.dstBuffer, 
				ress.dstBufferSize, 
				0 
			};
			
			//
			//zb: Compress Data
			//
            result = ZSTD_compress_generic(ress.cctx, &outBuff, &inBuff, directive); //zbzb: 

            /* Write compressed stream */
            if (outBuff.pos)
            {
				//
				//zb: Write Data to File
				//
                size_t const sizeCheck = fwrite(ress.dstBuffer, 1, outBuff.pos, dstFile);
                if (sizeCheck!=outBuff.pos)
                {
                    printf("Write error : cannot write compressed block");
					exit(-1);
                }
                compressedfilesize += outBuff.pos;
            }
        }

		//
    }
    while (directive != ZSTD_e_end);

    return compressedfilesize;
}





//
dRess_t FIO_createDResources(const char* dictFileName)
{
    dRess_t ress;
    memset(&ress, 0, sizeof(ress));

    /* Allocation */
    ress.dctx = ZSTD_createDStream();
    if (ress.dctx == NULL)
    {
        printf("Can't create ZSTD_DStream");
		exit(-1);
    }
	
    ZSTD_setDStreamParameter(ress.dctx, DStream_p_maxWindowSize, g_memLimit);

	//
    ress.srcBufferSize = ZSTD_DStreamInSize();
    ress.srcBuffer = malloc(ress.srcBufferSize);
    ress.dstBufferSize = ZSTD_DStreamOutSize();
    ress.dstBuffer = malloc(ress.dstBufferSize);

	//
    if (!ress.srcBuffer || !ress.dstBuffer)
    {
        printf("Allocation error : not enough memory");
		exit(-1);
    }

    /* dictionary */
    {
        //void* dictBuffer;
        //size_t const dictBufferSize = FIO_createDictBuffer(&dictBuffer, dictFileName);
        //ZSTD_initDStream_usingDict(ress.dctx, dictBuffer, dictBufferSize);
        ZSTD_initDStream_usingDict(ress.dctx, NULL, 0);
        //free(dictBuffer);
    }

    return ress;
}



void FIO_freeDResources(dRess_t ress)
{
    ZSTD_freeDStream(ress.dctx);
    free(ress.srcBuffer);
    free(ress.dstBuffer);
}



int FIO_decompressDstFile(dRess_t ress, const char* dstFileName, const char* srcFileName)
{
    int result;
    stat_t statbuf;
    int stat_result = 0;

	//DbgPrint("zb===FIO_decompressDstFile===1.0===srcFileName: %s dstFileName: %s\n", srcFileName, dstFileName);

    ress.dstFile = FIO_openDstFile(dstFileName);
    if (ress.dstFile==0)
    {
        return 1;
    }

    if (UTIL_getFileStat(srcFileName, &statbuf) )
    {
        stat_result = 1;
    }

	//
    result = FIO_decompressSrcFile(ress, dstFileName, srcFileName);

    fclose(ress.dstFile);

    if (stat_result)               
    {
        UTIL_setFileStat(dstFileName, &statbuf);    /* transfer file permissions from src into dst */
    }

    return result;
}


int FIO_decompressSrcFile(dRess_t ress, const char* dstFileName, const char* srcFileName)
{
    FILE* srcFile;
    int result;

    if (UTIL_isDirectory(srcFileName))
    {
        printf("zstd: %s is a directory -- ignored \n", srcFileName);
        return -1;
    }

    srcFile = FIO_openSrcFile(srcFileName);
    if (srcFile==NULL)
    {
        return -1;
    }

	//
    result = FIO_decompressFrames(ress, srcFile, dstFileName, srcFileName);

    fclose(srcFile);

    return result;
}




int FIO_decompressFrames(dRess_t ress, FILE* srcFile, const char* dstFileName, const char* srcFileName)
{
    unsigned readSomething = 0;
    unsigned long long filesize = 0;

    /* for each frame */
    for ( ; ; )
    {
        /* check magic number -> version */
        size_t const toRead = 4;
        const BYTE* const buf = (const BYTE*)ress.srcBuffer;
        if (ress.srcBufferLoaded < toRead)  /* load up to 4 bytes for header */
        {
            ress.srcBufferLoaded += fread((char*)ress.srcBuffer + ress.srcBufferLoaded, (size_t)1, toRead - ress.srcBufferLoaded, srcFile);
        }
		
        if (ress.srcBufferLoaded == 0)
        {
            if (readSomething==0)    /* srcFile is empty (which is invalid) */
            {
                printf("zstd: %s: unexpected end of file \n", srcFileName);
                return -1;
            }
            break;   /* no more input */
        }

        readSomething = 1;   /* there is at least 1 byte in srcFile */
        if (ress.srcBufferLoaded < toRead)
        {
            printf("zstd: %s: unknown header \n", srcFileName);
            return 1;
        }


		//
        if (ZSTD_isFrame(buf, ress.srcBufferLoaded))
        {
			//
			//zb: 
			//
            unsigned long long const frameSize = FIO_decompressZstdFrame(&ress, srcFile, srcFileName, filesize);

			//
            if (frameSize == FIO_ERROR_FRAME_DECODING)
            {
                return 1;
            }
            filesize += frameSize;
        }
        else if (buf[0] == 31 && buf[1] == 139)     /* gz magic number */
        {
			/*
	            unsigned long long const frameSize = FIO_decompressGzFrame(&ress, srcFile, srcFileName); //zbd:
	            if (frameSize == FIO_ERROR_FRAME_DECODING)
	            {
	                return 1;
	            }
	            filesize += frameSize;
	            */
        }
        else
        {
            printf("zstd: %s: unsupported format \n", srcFileName);
            return 1;
        }
    }  /* for each frame */

    /* Final Status */
    printf("\r%79s\r", "");
    printf("%-20s: %llu bytes \n", srcFileName, filesize);

    return 0;
}


unsigned long long FIO_decompressZstdFrame(dRess_t* ress, FILE* finput, const char* srcFileName, U64 alreadyDecoded)
{	
    U64 frameSize = 0;
    U32 storedSkips = 0;

    size_t const srcFileLength = strlen(srcFileName);
    if (srcFileLength > 20)
    {
        srcFileName += srcFileLength - 20;    /* display last 20 characters only */
    }

	//
    ZSTD_resetDStream(ress->dctx); //zb: change the context state


    /* Header loading : ensures ZSTD_getFrameHeader() will succeed */
    {
        size_t const toDecode = ZSTD_FRAMEHEADERSIZE_MAX;
        if (ress->srcBufferLoaded < toDecode)
        {
            size_t const toRead = toDecode - ress->srcBufferLoaded;
            void* const startPosition = (char*)ress->srcBuffer + ress->srcBufferLoaded;

			//
            ress->srcBufferLoaded += fread(startPosition, 1, toRead, finput); //zb: Continue to read 14 bytes.
        }
    }

    
    while (1) /* main_loop of ZSTD decompress,  zbzbzbzbzb */
    {
        ZSTD_inBuffer  inBuff = 
		{ 
			ress->srcBuffer, 
			ress->srcBufferLoaded, 
			0
		};
		
        ZSTD_outBuffer outBuff= 
		{ 
			ress->dstBuffer, 
			ress->dstBufferSize, 
			0
		};

		//
        size_t const readSizeHint = ZSTD_decompressStream(ress->dctx, &outBuff, &inBuff);

		//
        if (ZSTD_isError(readSizeHint))
        {
            printf("%s : Decoding error (36) : %s \n", srcFileName, ZSTD_getErrorName(readSizeHint));
            return FIO_ERROR_FRAME_DECODING;
        }

        /* Write block */
        FIO_fwriteData(ress->dstFile, ress->dstBuffer, outBuff.pos);

		//
        frameSize += outBuff.pos;

        if (inBuff.pos > 0)
        {
            memmove(ress->srcBuffer, (char*)ress->srcBuffer + inBuff.pos, inBuff.size - inBuff.pos); //zb?: too slow?
            ress->srcBufferLoaded -= inBuff.pos;
        }

        if (readSizeHint == 0)
        {
            break;    /* end of frame */
        }

		//DbgPrint("zb===ress->srcBufferLoaded: %d\n", ress->srcBufferLoaded);

        if (inBuff.size != inBuff.pos)
        {
            printf("%s : Decoding error (37) : should consume entire input \n", srcFileName);
            return FIO_ERROR_FRAME_DECODING;
        }

        /* Fill input buffer */
        {
            size_t const toDecode = MIN(readSizeHint, ress->srcBufferSize);  /* support large skippable frames */
            if (ress->srcBufferLoaded < toDecode)
            {
                size_t const toRead = toDecode - ress->srcBufferLoaded;   /* > 0 */
                void* const startPosition = (char*)ress->srcBuffer + ress->srcBufferLoaded;
                size_t const readSize = fread(startPosition, 1, toRead, finput); //zb: 
                //DbgPrint("zb===toRead: %d readSize: %d\n", toRead, readSize);
				
                if (readSize == 0)
                {
                    printf("%s : Read error (39) : premature end \n", srcFileName);
                    return FIO_ERROR_FRAME_DECODING;
                }
                ress->srcBufferLoaded += readSize;
            }
        }
    }

    return frameSize;
}



unsigned FIO_fwriteData(FILE* file, const void* buffer, size_t bufferSize)
{
    size_t const sizeCheck = fwrite(buffer, 1, bufferSize, file);
    if (sizeCheck != bufferSize)
    {
        printf("Write error : cannot write decoded block");
		exit(-1);
    }
    return 0;
}



//==========================================================================================

int FIO_compressData(cRess_t ress, char* input_buffer, int input_len, char* output_buffer, int output_len, int compressionLevel)
{
    U64 compressedfilesize = 0;

	//
    switch (g_compressionType)
    {
        default:
        case FIO_zstdCompression:
            compressedfilesize = FIO_compressZstdData(ress, input_buffer, input_len, output_buffer, output_len, compressionLevel);
            break;

        case FIO_gzipCompression:
            //compressedfilesize = FIO_compressGzipData(&ress, input_buffer, input_len, output_buffer, poutput_len, compressionLevel);
            break;
    }

    printf("%6.2f%%   (%6llu => %6llu bytes) \n", (double)compressedfilesize / (input_len + (!input_len)/*avoid div by zero*/) * 100, (unsigned long long)input_len, (unsigned long long) compressedfilesize);
    return compressedfilesize;
}



int FIO_compressZstdData(cRess_t ress, char* input_buffer, int input_len, char* output_buffer, int output_len, int compressionLevel)
{
	U64 readSize;
	U64 tailSize = input_len;
	char* cur_input_ptr = input_buffer;
	char* cur_output_ptr = output_buffer;
	int cur_available_output_len = output_len;
    U64 compressedfilesize = 0;
    ZSTD_EndDirective directive = ZSTD_e_continue;

    if (input_len != UTIL_FILESIZE_UNKNOWN)
    {
        ZSTD_CCtx_setPledgedSrcSize(ress.cctx, input_len);
    }

	//=====Main compression loop====
    do
    {
        size_t result;
		readSize = tailSize > COMPRESS_BLOCK_LEN ? COMPRESS_BLOCK_LEN : tailSize;
		tailSize = tailSize - readSize;
		if (readSize == 0 || tailSize == 0)
		{
			directive = ZSTD_e_end;
		}


		result = 1;
		
        ZSTD_inBuffer inBuff = 
		{ 
			cur_input_ptr, 
			readSize, 
			0
		};

        while (inBuff.pos != inBuff.size || (directive == ZSTD_e_end && result != 0))
        {
            ZSTD_outBuffer outBuff = 
			{ 
				cur_output_ptr, 
				cur_available_output_len,
				0
			};
			
            result = ZSTD_compress_generic(ress.cctx, &outBuff, &inBuff, directive);
            if (outBuff.pos)
            {
                compressedfilesize += outBuff.pos;

				//zb: 
				cur_output_ptr += outBuff.pos;
				cur_available_output_len -= outBuff.pos;
            }
        }

		//zb:
		cur_input_ptr += readSize;
		
    }
    while (directive != ZSTD_e_end);

    return compressedfilesize;
}



int FIO_compressGzipData(cRess_t ress, char* input_buffer, int input_len, char* output_buffer, int output_len, int compressionLevel)
{
	return 0;
}






//==========================================================================================
int FIO_decompressData(dRess_t ress, char* input_buffer, int input_len, char* output_buffer, int output_len)
{
	unsigned readSomething = 0;
	U64 decompressedSize = 0;

	//
	DataBuffer_t dataBuffer;
	dataBuffer.in_buffer = input_buffer;
	dataBuffer.in_length = input_len;
	dataBuffer.in_cur_ptr = input_buffer;
	dataBuffer.in_ptr_end = input_buffer + input_len;

	//
	dataBuffer.out_buffer = output_buffer;
	dataBuffer.out_length = output_len;
	dataBuffer.out_cur_ptr = output_buffer;
	dataBuffer.out_ptr_end = output_buffer + output_len;


    /* for each frame */
    for ( ; ; )
    {
		//printf("FIO_decompressData  for  loop...\n");
        /* check magic number -> version */
        size_t const toRead = 4;
        const BYTE* const buf = (const BYTE*)ress.srcBuffer;
        if (ress.srcBufferLoaded < toRead)  /* load up to 4 bytes for header */
        {
			//zb: substitue fread func.
			//ress.srcBufferLoaded += fread((char*)ress.srcBuffer + ress.srcBufferLoaded, (size_t)1, toRead - ress.srcBufferLoaded, srcFile);
			ress.srcBufferLoaded += FIO_decompressReadData(ress.srcBuffer + ress.srcBufferLoaded, toRead - ress.srcBufferLoaded, &dataBuffer);
        }
		
        if (ress.srcBufferLoaded == 0)
        {
            if (readSomething == 0)    /* srcFile is empty (which is invalid) */
            {
                printf("zstd: unexpected end!\n");
                return -1;
            }
            break;   /* no more input */
        }

        readSomething = 1;   /* there is at least 1 byte in srcFile */
        if (ress.srcBufferLoaded < toRead)
        {
            printf("zstd: unknown header!\n");
            return -1;
        }

		//printf("FIO_decompressData ====\n");

		//
        if (ZSTD_isFrame(buf, ress.srcBufferLoaded))
        {
            unsigned long long frameSize = FIO_decompressZstdData(&ress, input_buffer, input_len, output_buffer, output_len, &dataBuffer);
            if (frameSize == FIO_ERROR_FRAME_DECODING)
            {
                return -1;
            }
            decompressedSize += frameSize;
        }
        else if (buf[0] == 31 && buf[1] == 139)     /* gz magic number */
        {
			/*
	            unsigned long long const frameSize = FIO_decompressGzFrame(&ress, srcFile, srcFileName); //zbd:
	            if (frameSize == FIO_ERROR_FRAME_DECODING)
	            {
	                return 1;
	            }
	            filesize += frameSize;
	            */
        }
        else
        {
            printf("zstd: unsupported format!\n");
            return -1;
        }
    }  /* for each frame */

    printf("          (%6llu => %6llu bytes) \n", (unsigned long long)input_len, (unsigned long long) decompressedSize);

	return decompressedSize;
}



int FIO_decompressZstdData(dRess_t* ress, char* input_buffer, int input_len, char* output_buffer, int output_len, DataBuffer_t* dataBuffer)
{
	U64 frameSize = 0;
    U32 storedSkips = 0;

	//
    ZSTD_resetDStream(ress->dctx);

    /* Header loading : ensures ZSTD_getFrameHeader() will succeed */
    {
        size_t const toDecode = ZSTD_FRAMEHEADERSIZE_MAX;
        if (ress->srcBufferLoaded < toDecode)
        {
            size_t const toRead = toDecode - ress->srcBufferLoaded;
            void* const startPosition = (char*)ress->srcBuffer + ress->srcBufferLoaded;

			//zb: substitue fread func.
			//ress->srcBufferLoaded += fread(startPosition, 1, toRead, finput); //zb: Continue to read 14 bytes.
			ress->srcBufferLoaded += FIO_decompressReadData(startPosition, toRead, dataBuffer);
        }
    }

    while (1)
    {
        ZSTD_inBuffer  inBuff =
		{ 
			ress->srcBuffer, 
			ress->srcBufferLoaded, 
			0
		};
		
        ZSTD_outBuffer outBuff= 
		{ 
			ress->dstBuffer, 
			ress->dstBufferSize, 
			0
		};

		//printf("FIO_decompressZstdData loop...\n");

        size_t const readSizeHint = ZSTD_decompressStream(ress->dctx, &outBuff, &inBuff);
        if (ZSTD_isError(readSizeHint))
        {
            printf("Decoding error (36) : %s \n", ZSTD_getErrorName(readSizeHint));
            return FIO_ERROR_FRAME_DECODING;
        }

		
        /* Write block */
        FIO_decompressWriteData(ress->dstBuffer, outBuff.pos, dataBuffer);
 
        frameSize += outBuff.pos;

		//zb: Do this so that we can process the input buffer from the beginning!
        if (inBuff.pos > 0)
        {
            memmove(ress->srcBuffer, (char*)ress->srcBuffer + inBuff.pos, inBuff.size - inBuff.pos); //zb?: too slow?
            ress->srcBufferLoaded -= inBuff.pos;
        }

        if (readSizeHint == 0)
        {
            break;    /* end of frame */
        }

        if (inBuff.size != inBuff.pos)
        {
            printf("Decoding error (37) : should consume entire input \n");
            return FIO_ERROR_FRAME_DECODING;
        }

        /* Fill input buffer */
        {
            size_t const toDecode = MIN(readSizeHint, ress->srcBufferSize);  /* support large skippable frames */
            if (ress->srcBufferLoaded < toDecode)
            {
                size_t const toRead = toDecode - ress->srcBufferLoaded;   /* > 0 */
                void* const startPosition = (char*)ress->srcBuffer + ress->srcBufferLoaded;

				//zb: substitue fread func.
				//size_t const readSize = fread(startPosition, 1, toRead, finput); //zb:
				size_t const readSize = FIO_decompressReadData(startPosition, toRead, dataBuffer);
                if (readSize == 0)
                {
                    printf("Read error (39) : premature end \n");
                    return FIO_ERROR_FRAME_DECODING;
                }
                ress->srcBufferLoaded += readSize;		
            }
        }
    }

    return frameSize;
}



int FIO_decompressReadData(char* buf, int len, DataBuffer_t* dataBuffer)
{
	if (dataBuffer->in_cur_ptr >= dataBuffer->in_ptr_end)
	{
		return 0;
	}

	if (len <= 0)
	{
		return 0;
	}

	int nToRead = len;
	if (dataBuffer->in_cur_ptr + len >= dataBuffer->in_ptr_end)
	{
		nToRead = dataBuffer->in_ptr_end - dataBuffer->in_cur_ptr;
	}

	memcpy(buf, dataBuffer->in_cur_ptr, nToRead);
	dataBuffer->in_cur_ptr += nToRead;
	return nToRead;
}


int FIO_decompressWriteData(char* buf, int len, DataBuffer_t* dataBuffer)
{
	if (len == 0)
	{
		return 0;
	}

	if (dataBuffer->out_cur_ptr >= dataBuffer->out_ptr_end)
	{
		printf("Output buffer is full! len: %d\n", len);
		return -1;
	}

	if (dataBuffer->out_cur_ptr + len > dataBuffer->out_ptr_end)
	{
		printf("Error! Not enough output buffer to save the data! len: %d\n", len);
		return -1;	
	}

	memcpy(dataBuffer->out_cur_ptr, buf, len);
	dataBuffer->out_cur_ptr += len;
	return 0;
}




int FIO_decompressGzipData(dRess_t* ress, char* input_buffer, int input_len, char* output_buffer, int output_len, DataBuffer_t* dataBuffer)
{
	return 0;
}



