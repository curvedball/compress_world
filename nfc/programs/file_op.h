

#ifndef _FILE_OP_INC
#define _FILE_OP_INC


#include <stdlib.h>       /* malloc */
#include <stddef.h>       /* size_t, ptrdiff_t */
#include <stdio.h>        /* fprintf */
#include <string.h>       /* strncmp */
#include <sys/types.h>    /* stat, utime */
#include <sys/stat.h>     /* stat */
#include <unistd.h>     /* chown, stat */
#include <utime.h>      /* utime */
#include <time.h>         /* clock_t, clock, CLOCKS_PER_SEC, nanosleep */
#include <errno.h>
#include <unistd.h>
#include "debug.h"




#define UTIL_FILESIZE_UNKNOWN  ((U64)(-1))


typedef struct stat stat_t;




FILE* FIO_openSrcFile(const char* srcFileName);
FILE* FIO_openDstFile(const char* srcFileName);


//
int UTIL_getFileStat(const char* infilename, stat_t *statbuf);
int UTIL_setFileStat(const char *filename, stat_t *statbuf);



int UTIL_isRegularFile(const char* infilename);
U32 UTIL_isDirectory(const char* infilename);


U64 UTIL_getFileSize(const char* infilename);
int UTIL_readFile(FILE* fp, char* buffer, int size);
int UTIL_writeFile(FILE* fp, char* buffer, int size);


int UTIL_getCurrentWorkingDirectory(char* buffer, int size);
int UTIL_writeTempFile(char* buffer, int size);

#endif


