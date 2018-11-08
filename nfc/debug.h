



#ifndef DEBUG_INC
#define DEBUG_INC


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>



#define DbgPrintSwitch 	1


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







#endif





