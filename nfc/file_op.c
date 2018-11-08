
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
#include "file_op.h"


typedef struct stat stat_t;


FILE* FIO_openSrcFile(const char* srcFileName)
{
    if (!UTIL_isRegularFile(srcFileName))
    {
        DbgPrint("%s is not a regular file -- ignored!\n", srcFileName);
        return NULL;
    }

    FILE* const f = fopen(srcFileName, "rb");
    if (f == NULL)
    {
        DbgPrint("open %s error!\n", srcFileName);
    }
    return f;
}


FILE* FIO_openDstFile(const char* dstFileName)
{
    FILE* const f = fopen(dstFileName, "wb");
    if (f == NULL)
    {
        DbgPrint("Create or open %s error!\n", dstFileName);
		return NULL;
    }
    return f;
}


int UTIL_getFileStat(const char* infilename, stat_t *statbuf)
{
    int r;
    r = stat(infilename, statbuf);
    if (r || !S_ISREG(statbuf->st_mode))
    {
        return 0;    /* No good... */
    }

    return 1;
}



int UTIL_isRegularFile(const char* infilename)
{
    stat_t statbuf;
    return UTIL_getFileStat(infilename, &statbuf); /* Only need to know whether it is a regular file */
}



U32 UTIL_isDirectory(const char* infilename)
{
    int r;
    stat_t statbuf;

    r = stat(infilename, &statbuf);
    if (!r && S_ISDIR(statbuf.st_mode))
    {
        return 1;
    }

    return 0;
}




#define UTIL_FILESIZE_UNKNOWN  ((U64)(-1))
U64 UTIL_getFileSize(const char* infilename)
{
    if (!UTIL_isRegularFile(infilename))
    {
        return UTIL_FILESIZE_UNKNOWN;
    }

	//
    {
        int r;
        struct stat statbuf;
        r = stat(infilename, &statbuf);
        if (r || !S_ISREG(statbuf.st_mode))
        {
            return UTIL_FILESIZE_UNKNOWN;
        }
        return (U64)statbuf.st_size;
    }
}


int UTIL_readFile(FILE* fp, char* buffer, int size)
{
	int nRead = 0;
	int toRead = size;
	int n;
	while (1)
	{
		n = fread(buffer + nRead, 1, toRead, fp);
		if (n > 0)
		{
			nRead += n;
			if (nRead >= size)
			{
				return 0;
			}
			toRead -= n;
		}
		else if (n == 0)
		{
			printf("Read no data error!\n");
			return -1;			
		}
		else
		{
			printf("Read data error!\n");
			return -1;
		}
		
	}

    return 0;
}



int UTIL_writeFile(FILE* fp, char* buffer, int size)
{
	//DbgPrint("Data to be written size: %d!\n", size);	

	int nWrite = 0;
	int toWrite = size;
	int n;
	while (1)
	{
		n = fwrite(buffer + nWrite, 1, toWrite, fp);
		if (n > 0)
		{
			nWrite += n;
			if (nWrite >= size)
			{
				return 0;
			}
			toWrite -= n;
		}
		else if (n == 0)
		{
			printf("Write no data error!\n");
			return -1;			
		}
		else
		{
			printf("Write data error!\n");
			return -1;
		}
		
	}

    return 0;
}





