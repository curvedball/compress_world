



#ifndef ZSTD_CCOMMON_H_MODULE
#define ZSTD_CCOMMON_H_MODULE


/* this module contains definitions which must be identical
 * across compression, decompression and dictBuilder.
 * It also contains a few functions useful to at least 2 of them
 * and which benefit from being inlined */


/*-*************************************
*  Dependencies
***************************************/
#include "compiler.h"
#include "mem.h"
#include "debug.h"                 /* assert, DEBUGLOG, RAWLOG, g_debuglevel */
#include "error_private.h"

//
#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

//
#define FSE_STATIC_LINKING_ONLY
#include "fse.h"

//
#define HUF_STATIC_LINKING_ONLY
#include "huf.h"


#ifndef XXH_STATIC_LINKING_ONLY
#define XXH_STATIC_LINKING_ONLY
#endif


#include "xxhash.h"                /* XXH_reset, update, digest */


#if defined (__cplusplus)
extern "C" {
#endif

/* ---- static assert (debug) --- */
#define ZSTD_STATIC_ASSERT(c) DEBUG_STATIC_ASSERT(c)


/*-*************************************
*  shared macros
***************************************/
#undef MIN
#undef MAX
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define CHECK_F(f) { size_t const errcod = f; if (ERR_isError(errcod)) return errcod; }  /* check and Forward error code */
#define CHECK_E(f, e) { size_t const errcod = f; if (ERR_isError(errcod)) return ERROR(e); }  /* check and send Error code */


/*-*************************************
*  Common constants
***************************************/
#define ZSTD_OPT_NUM    (1<<12)


#define ZSTD_REP_NUM      3                 /* number of repcodes */
#define ZSTD_REP_MOVE     (ZSTD_REP_NUM-1)
static const U32 repStartValue[ZSTD_REP_NUM] = { 1, 4, 8 };


#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)


#define BIT7 128
#define BIT6  64
#define BIT5  32
#define BIT4  16
#define BIT1   2
#define BIT0   1




#define ZSTD_WINDOWLOG_ABSOLUTEMIN 10
#define ZSTD_WINDOWLOG_DEFAULTMAX 27 /* Default maximum allowed window log */


static const size_t ZSTD_fcs_fieldSize[4] = { 0, 2, 4, 8 };
static const size_t ZSTD_did_fieldSize[4] = { 0, 1, 2, 4 };



#define ZSTD_FRAMEIDSIZE 4
static const size_t ZSTD_frameIdSize = ZSTD_FRAMEIDSIZE;  /* magic number size */


#define ZSTD_BLOCKHEADERSIZE 3   /* C standard doesn't allow `static const` variable to be init using another `static const` variable */
static const size_t ZSTD_blockHeaderSize = ZSTD_BLOCKHEADERSIZE;

//
typedef enum 
{ 
	bt_raw, 
	bt_rle, 
	bt_compressed, 
	bt_reserved 
} blockType_e;


typedef enum 
{ 
	delta_disabled, 
	delta_enabled, 
} blockDeltaFlag;



#define MIN_SEQUENCES_SIZE 1 /* nbSeq==0 */
#define MIN_CBLOCK_SIZE (1 /*litCSize*/ + 1 /* RLE or RAW */ + MIN_SEQUENCES_SIZE /* nbSeq==0 */)   /* for a non-null block */

#define HufLog 12


typedef enum 
{ 
	set_basic, 
	set_rle, 
	set_compressed, 
	set_repeat 
} symbolEncodingType_e;



#define LONGNBSEQ 0x7F00

#define MINMATCH 3


#define Litbits  8
#define MaxLit ((1<<Litbits) - 1)
#define MaxML   52
#define MaxLL   35
#define DefaultMaxOff 28
#define MaxOff  31
#define MaxSeq MAX(MaxLL, MaxML)   /* Assumption : MaxOff < MaxLL,MaxML */
#define MLFSELog    9
#define LLFSELog    9
#define OffFSELog   8
#define MaxFSELog  MAX(MAX(MLFSELog, LLFSELog), OffFSELog)


static const U32 LL_bits[MaxLL + 1] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                        0, 0, 0, 0, 0, 0, 0, 0,
                                        1, 1, 1, 1, 2, 2, 3, 3,
                                        4, 6, 7, 8, 9,10,11,12,
                                        13,14,15,16
                                    };



static const S16 LL_defaultNorm[MaxLL + 1] = { 4, 3, 2, 2, 2, 2, 2, 2,
                                               2, 2, 2, 2, 2, 1, 1, 1,
                                               2, 2, 2, 2, 2, 2, 2, 2,
                                               2, 3, 2, 1, 1, 1, 1, 1,
                                               -1,-1,-1,-1
                                           };


#define LL_DEFAULTNORMLOG 6  /* for static allocation */
static const U32 LL_defaultNormLog = LL_DEFAULTNORMLOG;


//
static const U32 ML_bits[MaxML + 1] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0,
                                      1, 1, 1, 1, 2, 2, 3, 3,
                                      4, 4, 5, 7, 8, 9,10,11,
                                      12,13,14,15,16
                                    };


static const S16 ML_defaultNorm[MaxML+1] = { 1, 4, 3, 2, 2, 2, 2, 2,
                                             2, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1,-1,-1,
                                             -1,-1,-1,-1,-1
                                           };


#define ML_DEFAULTNORMLOG 6  /* for static allocation */

//
static const U32 ML_defaultNormLog = ML_DEFAULTNORMLOG;


static const S16 OF_defaultNorm[DefaultMaxOff+1] = { 1, 1, 1, 1, 1, 1, 2, 2,
                                                     2, 1, 1, 1, 1, 1, 1, 1,
                                                     1, 1, 1, 1, 1, 1, 1, 1,
                                                    -1,-1,-1,-1,-1
                                                   };


//
#define OF_DEFAULTNORMLOG 5  /* for static allocation */

//
static const U32 OF_defaultNormLog = OF_DEFAULTNORMLOG;


/*-*******************************************
*  Shared functions to include for inlining
*********************************************/
static void ZSTD_copy8(void* dst, const void* src)
{
    memcpy(dst, src, 8);
}


/*! ZSTD_wildcopy() :
 *  custom version of memcpy(), can overwrite up to WILDCOPY_OVERLENGTH bytes (if length==0) */
#define WILDCOPY_OVERLENGTH 8

//
MEM_STATIC void ZSTD_wildcopy(void* dst, const void* src, ptrdiff_t length)
{
    const BYTE* ip = (const BYTE*)src;
    BYTE* op = (BYTE*)dst;
    BYTE* const oend = op + length;
    do
    {
		ZSTD_copy8(op, ip);
		op += 8;
		ip += 8;
    }while (op < oend);
}


MEM_STATIC void ZSTD_wildcopy_e(void* dst, const void* src, void* dstEnd)   /* should be faster for decoding, but strangely, not verified on all platform */
{
    const BYTE* ip = (const BYTE*)src;
    BYTE* op = (BYTE*)dst;
    BYTE* const oend = (BYTE*)dstEnd;
    do
    {
		ZSTD_copy8(op, ip);
		op += 8;
		ip += 8;
    }while (op < oend);
}


/*-*******************************************
*  Private declarations
*********************************************/
typedef struct seqDef_s
{
    U32 offset;
    U16 litLength;
    U16 matchLength;
} seqDef;



typedef struct
{
    seqDef* sequencesStart;
    seqDef* sequences;

	//
    BYTE* litStart;        //zb@: LiteralsBuffer start
    BYTE* lit; 				//zb@: Save the current position of the stored literals of the original bytes!
    blockDeltaFlag	deltaFlag;

	//
    BYTE* llCode;
    BYTE* mlCode;
    BYTE* ofCode;
	
    U32   longLengthID;   /* 0 == no longLength; 1 == Lit.longLength; 2 == Match.longLength; */
    U32   longLengthPos;
} seqStore_t;


const seqStore_t* ZSTD_getSeqStore(const ZSTD_CCtx* ctx);   /* compress & dictBuilder */
void ZSTD_seqToCodes(const seqStore_t* seqStorePtr);   /* compress, dictBuilder, decodeCorpus (shouldn't get its definition from here) */


/* custom memory allocation functions */
void* ZSTD_malloc(size_t size, ZSTD_customMem customMem);
void* ZSTD_calloc(size_t size, ZSTD_customMem customMem);
void ZSTD_free(void* ptr, ZSTD_customMem customMem);


MEM_STATIC U32 ZSTD_highbit32(U32 val)   /* compress, dictBuilder, decodeCorpus */
{
    assert(val != 0);
    {
        return 31 - __builtin_clz(val);
    }
}


/* ZSTD_invalidateRepCodes() :
 * ensures next compression will not use repcodes from previous block.
 * Note : only works with regular variant;
 *        do not use with extDict variant ! */
void ZSTD_invalidateRepCodes(ZSTD_CCtx* cctx);   /* zstdmt, adaptive_compression (shouldn't get this definition from here) */


typedef struct
{
    blockType_e blockType;
    U32 lastBlock;
    U32 origSize;
	blockDeltaFlag deltaFlag;
} blockProperties_t;



/*! ZSTD_getcBlockSize() :
 *  Provides the size of compressed block from block header `src` */
/* Used by: decompress, fullbench (does not get its definition from here) */
size_t ZSTD_getcBlockSize(const void* src, size_t srcSize, blockProperties_t* bpPtr);



#define MIN_DELTA_LEN 100


#if defined (__cplusplus)
}
#endif


#endif



