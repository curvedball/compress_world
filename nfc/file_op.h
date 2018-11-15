

#ifndef _FILE_OP_INC
#define _FILE_OP_INC

#include <stdio.h>

FILE* FIO_openSrcFile(const char* srcFileName);
FILE* FIO_openDstFile(const char* srcFileName);


int UTIL_isRegularFile(const char* infilename);
U32 UTIL_isDirectory(const char* infilename);


U64 UTIL_getFileSize(const char* infilename);
int UTIL_readFile(FILE* fp, char* buffer, int size);
int UTIL_writeFile(FILE* fp, char* buffer, int size);


int UTIL_getCurrentWorkingDirectory(char* buffer, int size);


#endif


