


/* Note : this module is expected to remain private, do not expose it */

#ifndef ERROR_H_MODULE
#define ERROR_H_MODULE


#if defined (__cplusplus)
extern "C" {
#endif


/* ****************************************
*  Dependencies
******************************************/
#include <stddef.h>        /* size_t */
#include "zstd_errors.h"  /* enum list */



#define ERR_STATIC static __attribute__((unused))



/*-****************************************
*  Customization (error_public.h)
******************************************/
typedef ZSTD_ErrorCode ERR_enum;

//
#define PREFIX(name) ZSTD_error_##name


/*-****************************************
*  Error codes handling
******************************************/
#undef ERROR   /* reported already defined on VS 2015 (Rich Geldreich) */
#define ERROR(name) ZSTD_ERROR(name)
#define ZSTD_ERROR(name) ((size_t)-PREFIX(name))



ERR_STATIC unsigned ERR_isError(size_t code)
{
    return (code > ERROR(maxCode));
}



ERR_STATIC ERR_enum ERR_getErrorCode(size_t code)
{
    if (!ERR_isError(code))
    {
        return (ERR_enum)0;
    }
    return (ERR_enum) (0-code);
}


/*-****************************************
*  Error Strings
******************************************/
const char* ERR_getErrorString(ERR_enum code);   /* error_private.c */



ERR_STATIC const char* ERR_getErrorName(size_t code)
{
    return ERR_getErrorString(ERR_getErrorCode(code));
}


#if defined (__cplusplus)
}
#endif



#endif




