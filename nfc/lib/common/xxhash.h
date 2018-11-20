

/*
   xxHash - Extremely Fast Hash algorithm
   - xxHash source repository : https://github.com/Cyan4973/xxHash
*/


#if defined (__cplusplus)
extern "C" {
#endif



#ifndef XXHASH_H_5627135585666179
#define XXHASH_H_5627135585666179 1


/* ****************************
*  Definitions
******************************/
#include <stddef.h>   /* size_t */
typedef enum { XXH_OK=0, XXH_ERROR } XXH_errorcode;


#define XXH_PUBLIC_API   /* do nothing */



/* *************************************
*  Version
***************************************/
#define XXH_VERSION_MAJOR    0
#define XXH_VERSION_MINOR    6
#define XXH_VERSION_RELEASE  2
#define XXH_VERSION_NUMBER  (XXH_VERSION_MAJOR *100*100 + XXH_VERSION_MINOR *100 + XXH_VERSION_RELEASE)

//
XXH_PUBLIC_API unsigned XXH_versionNumber (void);


/* ****************************
*  Simple Hash Functions
******************************/
typedef unsigned int       XXH32_hash_t;
typedef unsigned long long XXH64_hash_t;



XXH_PUBLIC_API XXH32_hash_t XXH32 (const void* input, size_t length, unsigned int seed);
XXH_PUBLIC_API XXH64_hash_t XXH64 (const void* input, size_t length, unsigned long long seed);

/*!
XXH32() :
    Calculate the 32-bits hash of sequence "length" bytes stored at memory address "input".
    The memory between input & input+length must be valid (allocated and read-accessible).
    "seed" can be used to alter the result predictably.
    Speed on Core 2 Duo @ 3 GHz (single thread, SMHasher benchmark) : 5.4 GB/s
XXH64() :
    Calculate the 64-bits hash of sequence of length "len" stored at memory address "input".
    "seed" can be used to alter the result predictably.
    This function runs 2x faster on 64-bits systems, but slower on 32-bits systems (see benchmark).
*/


/* ****************************
*  Streaming Hash Functions
******************************/
typedef struct XXH32_state_s XXH32_state_t;   /* incomplete type */
typedef struct XXH64_state_s XXH64_state_t;   /* incomplete type */



/*! State allocation, compatible with dynamic libraries */
XXH_PUBLIC_API XXH32_state_t* XXH32_createState(void);
XXH_PUBLIC_API XXH_errorcode  XXH32_freeState(XXH32_state_t* statePtr);

XXH_PUBLIC_API XXH64_state_t* XXH64_createState(void);
XXH_PUBLIC_API XXH_errorcode  XXH64_freeState(XXH64_state_t* statePtr);



/* hash streaming */
XXH_PUBLIC_API XXH_errorcode XXH32_reset  (XXH32_state_t* statePtr, unsigned int seed);
XXH_PUBLIC_API XXH_errorcode XXH32_update (XXH32_state_t* statePtr, const void* input, size_t length);
XXH_PUBLIC_API XXH32_hash_t  XXH32_digest (const XXH32_state_t* statePtr);

XXH_PUBLIC_API XXH_errorcode XXH64_reset  (XXH64_state_t* statePtr, unsigned long long seed);
XXH_PUBLIC_API XXH_errorcode XXH64_update (XXH64_state_t* statePtr, const void* input, size_t length);
XXH_PUBLIC_API XXH64_hash_t  XXH64_digest (const XXH64_state_t* statePtr);

/*
These functions generate the xxHash of an input provided in multiple segments.
Note that, for small input, they are slower than single-call functions, due to state management.
For small input, prefer `XXH32()` and `XXH64()` .

XXH state must first be allocated, using XXH*_createState() .

Start a new hash by initializing state with a seed, using XXH*_reset().

Then, feed the hash state by calling XXH*_update() as many times as necessary.
Obviously, input must be allocated and read accessible.
The function returns an error code, with 0 meaning OK, and any other value meaning there is an error.

Finally, a hash value can be produced anytime, by using XXH*_digest().
This function returns the nn-bits hash as an int or long long.

It's still possible to continue inserting input into the hash state after a digest,
and generate some new hashes later on, by calling again XXH*_digest().

When done, free XXH state space if it was allocated dynamically.
*/




XXH_PUBLIC_API void XXH32_copyState(XXH32_state_t* restrict dst_state, const XXH32_state_t* restrict src_state);
XXH_PUBLIC_API void XXH64_copyState(XXH64_state_t* restrict dst_state, const XXH64_state_t* restrict src_state);


/* **************************
*  Canonical representation
****************************/
/* Default result type for XXH functions are primitive unsigned 32 and 64 bits.
*  The canonical representation uses human-readable write convention, aka big-endian (large digits first).
*  These functions allow transformation of hash result into and from its canonical format.
*  This way, hash values can be written into a file / memory, and remain comparable on different systems and programs.
*/
typedef struct
{
    unsigned char digest[4];
} XXH32_canonical_t;



typedef struct
{
    unsigned char digest[8];
} XXH64_canonical_t;



XXH_PUBLIC_API void XXH32_canonicalFromHash(XXH32_canonical_t* dst, XXH32_hash_t hash);
XXH_PUBLIC_API void XXH64_canonicalFromHash(XXH64_canonical_t* dst, XXH64_hash_t hash);

XXH_PUBLIC_API XXH32_hash_t XXH32_hashFromCanonical(const XXH32_canonical_t* src);
XXH_PUBLIC_API XXH64_hash_t XXH64_hashFromCanonical(const XXH64_canonical_t* src);

#endif /* XXHASH_H_5627135585666179 */




/* ================================================================================================
   This section contains definitions which are not guaranteed to remain stable.
   They may change in future versions, becoming incompatible with a different version of the library.
   They shall only be used with static linking.
   Never use these definitions in association with dynamic linking !
=================================================================================================== */
#if defined(XXH_STATIC_LINKING_ONLY) && !defined(XXH_STATIC_H_3543687687345)
#define XXH_STATIC_H_3543687687345

/* These definitions are only meant to allow allocation of XXH state
   statically, on stack, or in a struct for example.
   Do not use members directly. */


struct XXH32_state_s
{
    unsigned total_len_32;
    unsigned large_len;
    unsigned v1;
    unsigned v2;
    unsigned v3;
    unsigned v4;
    unsigned mem32[4];   /* buffer defined as U32 for alignment */
    unsigned memsize;
    unsigned reserved;   /* never read nor write, will be removed in a future version */
};   /* typedef'd to XXH32_state_t */



struct XXH64_state_s
{
    unsigned long long total_len;
    unsigned long long v1;
    unsigned long long v2;
    unsigned long long v3;
    unsigned long long v4;
    unsigned long long mem64[4];   /* buffer defined as U64 for alignment */
    unsigned memsize;
    unsigned reserved[2];          /* never read nor write, will be removed in a future version */
};   /* typedef'd to XXH64_state_t */


#endif /* XXH_STATIC_LINKING_ONLY && XXH_STATIC_H_3543687687345 */


#if defined (__cplusplus)
}
#endif



