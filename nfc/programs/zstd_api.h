
/*
	zstd_api.h
*/

#ifndef _ZSTD_API_INC
#define _ZSTD_API_INC


//
#define ZSTD_STATIC_LINKING_ONLY
#include "../lib/zstd.h"


#include <stdio.h>
#include <stdint.h>
#include "debug.h"


#ifndef NULL
#define NULL 0
#endif


#define KB *(1<<10)
#define MB *(1<<20)
#define GB *(1U<<30)


typedef   uint8_t BYTE;
typedef  uint16_t U16;
typedef   int16_t S16;
typedef  uint32_t U32;
typedef   int32_t S32;
typedef  uint64_t U64;
typedef   int64_t S64;




#ifndef ZSTDCLI_CLEVEL_DEFAULT
#define ZSTDCLI_CLEVEL_DEFAULT 3
#endif



typedef enum 
{ 
	FIO_zstdCompression, 
	FIO_gzipCompression, 
} FIO_compressionType_t;



typedef struct
{
    FILE* srcFile;
    FILE* dstFile;
    void*  srcBuffer;
    size_t srcBufferSize;
    void*  dstBuffer;
    size_t dstBufferSize;
    ZSTD_CStream* cctx; //zb: NULL if we don't need to customize!
}cRess_t;



cRess_t FIO_createCResources(const char* dictFileName, int cLevel, U64 srcSize, ZSTD_compressionParameters* comprParams);
void FIO_freeCResources(cRess_t ress);


int FIO_compressFilename_dstFile(cRess_t ress, const char* dstFileName, const char* srcFileName, int compressionLevel);
int FIO_compressFilename_srcFile(cRess_t ress, const char* dstFileName, const char* srcFileName, int compressionLevel);
int FIO_compressFilename_internal(cRess_t ress, const char* dstFileName, const char* srcFileName, int compressionLevel);
unsigned long long FIO_compressZstdFrame(const cRess_t* ressPtr, const char* srcFileName, U64 fileSize, int compressionLevel, U64* readsize);




//===============================================================================
typedef struct
{
    void*  srcBuffer;
    size_t srcBufferSize;
    size_t srcBufferLoaded;
    void*  dstBuffer;
    size_t dstBufferSize;
    ZSTD_DStream* dctx;
    FILE*  dstFile;
} dRess_t;



typedef struct
{
    void*  in_buffer;
    size_t in_length;
    char*  in_cur_ptr;
	char*  in_ptr_end;

    void*  out_buffer;
    size_t out_length;
    char*  out_cur_ptr;
	char*  out_ptr_end;	
} DataBuffer_t;



#define FIO_ERROR_FRAME_DECODING   ((unsigned long long)(-2))
#define LONG_SEEK fseek
#define MIN(a,b)    ((a) < (b) ? (a) : (b))


dRess_t FIO_createDResources(const char* dictFileName);
void FIO_freeDResources(dRess_t ress);


int FIO_decompressDstFile(dRess_t ress, const char* dstFileName, const char* srcFileName);
int FIO_decompressSrcFile(dRess_t ress, const char* dstFileName, const char* srcFileName);
int FIO_decompressFrames(dRess_t ress, FILE* srcFile, const char* dstFileName, const char* srcFileName);
unsigned long long FIO_decompressZstdFrame(dRess_t* ress, FILE* finput, const char* srcFileName, U64 alreadyDecoded);
unsigned FIO_fwriteData(FILE* file, const void* buffer, size_t bufferSize);




//===============================================

int FIO_compressData(cRess_t ress, char* input_buffer, int input_len, char* output_buffer, int output_len, int compressionLevel);
int FIO_compressZstdData(cRess_t ress, char* input_buffer, int input_len, char* output_buffer, int output_len, int compressionLevel);
int FIO_compressGzipData(cRess_t ress, char* input_buffer, int input_len, char* output_buffer, int output_len, int compressionLevel);




//===============================================

int FIO_decompressData(dRess_t ress, char* input_buffer, int input_len, char* output_buffer, int output_len);
int FIO_decompressZstdData(dRess_t* ress, char* input_buffer, int input_len, char* output_buffer, int output_len, DataBuffer_t* dataBuffer);
int FIO_decompressReadData(char* buf, int len, DataBuffer_t* dataBuffer);
int FIO_decompressWriteData(char* buf, int len, DataBuffer_t* dataBuffer);

int FIO_decompressGzipData(dRess_t* ress, char* input_buffer, int input_len, char* output_buffer, int output_len, DataBuffer_t* dataBuffer);


#endif



