

#ifndef ZSTD_ERRORS_H_398273423
#define ZSTD_ERRORS_H_398273423

#if defined (__cplusplus)
extern "C" {
#endif


#include <stddef.h>   /* size_t */


#define ZSTDERRORLIB_VISIBILITY __attribute__ ((visibility ("default")))



#define ZSTDERRORLIB_API ZSTDERRORLIB_VISIBILITY



typedef enum
{
    ZSTD_error_no_error = 0,
    ZSTD_error_GENERIC  = 1,
    ZSTD_error_prefix_unknown                = 10,
    ZSTD_error_version_unsupported           = 12,
    ZSTD_error_frameParameter_unsupported    = 14,
    ZSTD_error_frameParameter_windowTooLarge = 16,
    ZSTD_error_corruption_detected = 20,
    ZSTD_error_checksum_wrong      = 22,
    ZSTD_error_dictionary_corrupted      = 30,
    ZSTD_error_dictionary_wrong          = 32,
    ZSTD_error_dictionaryCreation_failed = 34,
    ZSTD_error_parameter_unsupported   = 40,
    ZSTD_error_parameter_outOfBound    = 42,
    ZSTD_error_tableLog_tooLarge       = 44,
    ZSTD_error_maxSymbolValue_tooLarge = 46,
    ZSTD_error_maxSymbolValue_tooSmall = 48,
    ZSTD_error_stage_wrong       = 60,
    ZSTD_error_init_missing      = 62,
    ZSTD_error_memory_allocation = 64,
    ZSTD_error_workSpace_tooSmall= 66,
    ZSTD_error_dstSize_tooSmall = 70,
    ZSTD_error_srcSize_wrong    = 72,
    /* following error codes are __NOT STABLE__, they can be removed or changed in future versions */
    ZSTD_error_frameIndex_tooLarge = 100,
    ZSTD_error_seekableIO          = 102,
    ZSTD_error_maxCode = 120  /* never EVER use this value directly, it can change in future versions! Use ZSTD_isError() instead */
} ZSTD_ErrorCode;




/*! ZSTD_getErrorCode() :
    convert a `size_t` function result into a `ZSTD_ErrorCode` enum type,
    which can be used to compare with enum list published above */
ZSTDERRORLIB_API ZSTD_ErrorCode ZSTD_getErrorCode(size_t functionResult);


ZSTDERRORLIB_API const char* ZSTD_getErrorString(ZSTD_ErrorCode code);   /**< Same as ZSTD_getErrorName, but using a `ZSTD_ErrorCode` enum argument */


#if defined (__cplusplus)
}
#endif


#endif




