




#ifndef MEM_H_MODULE
#define MEM_H_MODULE


#if defined (__cplusplus)
extern "C" {
#endif


/*-****************************************
*  Dependencies
******************************************/
#include <stddef.h>     /* size_t, ptrdiff_t */
#include <string.h>     /* memcpy */



//
#define MEM_STATIC static __inline __attribute__((unused))


/* code only tested on 32 and 64 bits systems */
#define MEM_STATIC_ASSERT(c)   { enum { MEM_static_assert = 1/(int)(!!(c)) }; }


//
MEM_STATIC void MEM_check(void)
{
    MEM_STATIC_ASSERT((sizeof(size_t)==4) || (sizeof(size_t)==8));
}



#include <stdint.h>

//
typedef   uint8_t BYTE;
typedef  uint16_t U16;
typedef   int16_t S16;
typedef  uint32_t U32;
typedef   int32_t S32;
typedef  uint64_t U64;
typedef   int64_t S64;




//
#define MEM_FORCE_MEMORY_ACCESS 1




MEM_STATIC unsigned MEM_32bits(void)
{
    return sizeof(size_t)==4;
}
MEM_STATIC unsigned MEM_64bits(void)
{
    return sizeof(size_t)==8;
}

MEM_STATIC unsigned MEM_isLittleEndian(void)
{
    const union
    {
        U32 u;
        BYTE c[4];
    } one = { 1 };   /* don't use static : performance detrimental  */
    return one.c[0];
}





/* __pack instructions are safer, but compiler specific, hence potentially problematic for some compilers */
/* currently only defined for gcc and icc */
typedef struct
{
    U16 v;
} __attribute__((packed)) unalign16;



typedef struct
{
    U32 v;
} __attribute__((packed)) unalign32;



typedef struct
{
    U64 v;
} __attribute__((packed)) unalign64;



typedef struct
{
    size_t v;
} __attribute__((packed)) unalignArch;



MEM_STATIC U16 MEM_read16(const void* ptr)
{
    return ((const unalign16*)ptr)->v;
}


MEM_STATIC U32 MEM_read32(const void* ptr)
{
    return ((const unalign32*)ptr)->v;
}


MEM_STATIC U64 MEM_read64(const void* ptr)
{
    return ((const unalign64*)ptr)->v;
}



MEM_STATIC size_t MEM_readST(const void* ptr)
{
    return ((const unalignArch*)ptr)->v;
}



MEM_STATIC void MEM_write16(void* memPtr, U16 value)
{
    ((unalign16*)memPtr)->v = value;
}


MEM_STATIC void MEM_write32(void* memPtr, U32 value)
{
    ((unalign32*)memPtr)->v = value;
}


MEM_STATIC void MEM_write64(void* memPtr, U64 value)
{
    ((unalign64*)memPtr)->v = value;
}





MEM_STATIC U32 MEM_swap32(U32 in)
{
    return __builtin_bswap32(in);
}


MEM_STATIC U64 MEM_swap64(U64 in)
{
    return __builtin_bswap64(in);
}


MEM_STATIC size_t MEM_swapST(size_t in)
{
    if (MEM_32bits())
    {
        return (size_t)MEM_swap32((U32)in);
    }
    else
    {
        return (size_t)MEM_swap64((U64)in);
    }
}



/*=== Little endian r/w ===*/
MEM_STATIC U16 MEM_readLE16(const void* memPtr)
{
    if (MEM_isLittleEndian())
    {
        return MEM_read16(memPtr);
    }
    else
    {
        const BYTE* p = (const BYTE*)memPtr;
        return (U16)(p[0] + (p[1]<<8));
    }
}


MEM_STATIC void MEM_writeLE16(void* memPtr, U16 val)
{
    if (MEM_isLittleEndian())
    {
        MEM_write16(memPtr, val);
    }
    else
    {
        BYTE* p = (BYTE*)memPtr;
        p[0] = (BYTE)val;
        p[1] = (BYTE)(val>>8);
    }
}


MEM_STATIC U32 MEM_readLE24(const void* memPtr)
{
    return MEM_readLE16(memPtr) + (((const BYTE*)memPtr)[2] << 16);
}


MEM_STATIC void MEM_writeLE24(void* memPtr, U32 val)
{
    MEM_writeLE16(memPtr, (U16)val);
    ((BYTE*)memPtr)[2] = (BYTE)(val>>16);
}


MEM_STATIC U32 MEM_readLE32(const void* memPtr)
{
    if (MEM_isLittleEndian())
    {
        return MEM_read32(memPtr);
    }
    else
    {
        return MEM_swap32(MEM_read32(memPtr));
    }
}


MEM_STATIC void MEM_writeLE32(void* memPtr, U32 val32)
{
    if (MEM_isLittleEndian())
    {
        MEM_write32(memPtr, val32);
    }
    else
    {
        MEM_write32(memPtr, MEM_swap32(val32));
    }
}


MEM_STATIC U64 MEM_readLE64(const void* memPtr)
{
    if (MEM_isLittleEndian())
    {
        return MEM_read64(memPtr);
    }
    else
    {
        return MEM_swap64(MEM_read64(memPtr));
    }
}


MEM_STATIC void MEM_writeLE64(void* memPtr, U64 val64)
{
    if (MEM_isLittleEndian())
    {
        MEM_write64(memPtr, val64);
    }
    else
    {
        MEM_write64(memPtr, MEM_swap64(val64));
    }
}


MEM_STATIC size_t MEM_readLEST(const void* memPtr)
{
    if (MEM_32bits())
    {
        return (size_t)MEM_readLE32(memPtr);
    }
    else
    {
        return (size_t)MEM_readLE64(memPtr);
    }
}


MEM_STATIC void MEM_writeLEST(void* memPtr, size_t val)
{
    if (MEM_32bits())
    {
        MEM_writeLE32(memPtr, (U32)val);
    }
    else
    {
        MEM_writeLE64(memPtr, (U64)val);
    }
}



/*=== Big endian r/w ===*/
MEM_STATIC U32 MEM_readBE32(const void* memPtr)
{
    if (MEM_isLittleEndian())
    {
        return MEM_swap32(MEM_read32(memPtr));
    }
    else
    {
        return MEM_read32(memPtr);
    }
}


MEM_STATIC void MEM_writeBE32(void* memPtr, U32 val32)
{
    if (MEM_isLittleEndian())
    {
        MEM_write32(memPtr, MEM_swap32(val32));
    }
    else
    {
        MEM_write32(memPtr, val32);
    }
}


MEM_STATIC U64 MEM_readBE64(const void* memPtr)
{
    if (MEM_isLittleEndian())
    {
        return MEM_swap64(MEM_read64(memPtr));
    }
    else
    {
        return MEM_read64(memPtr);
    }
}


MEM_STATIC void MEM_writeBE64(void* memPtr, U64 val64)
{
    if (MEM_isLittleEndian())
    {
        MEM_write64(memPtr, MEM_swap64(val64));
    }
    else
    {
        MEM_write64(memPtr, val64);
    }
}


MEM_STATIC size_t MEM_readBEST(const void* memPtr)
{
    if (MEM_32bits())
    {
        return (size_t)MEM_readBE32(memPtr);
    }
    else
    {
        return (size_t)MEM_readBE64(memPtr);
    }
}


MEM_STATIC void MEM_writeBEST(void* memPtr, size_t val)
{
    if (MEM_32bits())
    {
        MEM_writeBE32(memPtr, (U32)val);
    }
    else
    {
        MEM_writeBE64(memPtr, (U64)val);
    }
}


#if defined (__cplusplus)
}
#endif


#endif



