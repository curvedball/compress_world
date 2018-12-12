



#ifndef _DEBUG_INC
#define _DEBUG_INC


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>



#define DbgPrintSwitch 	0


#if DbgPrintSwitch
	#define DbgPrint printf
#else
	#define DbgPrint(fmt, ...)
#endif


#define UTIL_STATIC static __attribute__((unused))




//
typedef   uint8_t BYTE;
typedef  uint16_t U16;
typedef   int16_t S16;
typedef  uint32_t U32;
typedef   int32_t S32;
typedef  uint64_t U64;
typedef   int64_t S64;


#define MAX_PATHNAME_LEN 256


#ifndef BOOL
#define BOOL BYTE
#endif

#ifndef bool
#define bool BYTE
#endif



#ifndef TRUE
#define TRUE 1
#endif

#ifndef true
#define true 1
#endif


#ifndef FALSE
#define FALSE 0
#endif

#ifndef false
#define false 0
#endif


#define OUTPUT_TEST_TOOL 0


#endif





