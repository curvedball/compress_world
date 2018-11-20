


/* ======   Dependencies   ======= */
#include <stddef.h>    /* size_t */
#include "debug.h"     /* assert */
#include "zstd_internal.h"  /* ZSTD_malloc, ZSTD_free */
#include "pool.h"




/* ========================== */
/* No multi-threading support */
/* ========================== */


/* We don't need any data, but if it is empty, malloc() might return NULL. */
struct POOL_ctx_s
{
    int dummy;
};


//
static POOL_ctx g_ctx;



POOL_ctx* POOL_create(size_t numThreads, size_t queueSize)
{
    return POOL_create_advanced(numThreads, queueSize, ZSTD_defaultCMem);
}


POOL_ctx* POOL_create_advanced(size_t numThreads, size_t queueSize, ZSTD_customMem customMem)
{
    (void)numThreads;
    (void)queueSize;
    (void)customMem;
    return &g_ctx;
}


void POOL_free(POOL_ctx* ctx)
{
    assert(!ctx || ctx == &g_ctx);
    (void)ctx;
}


int POOL_resize(POOL_ctx* ctx, size_t numThreads)
{
    (void)ctx;
    (void)numThreads;
    return 0;
}


void POOL_add(POOL_ctx* ctx, POOL_function function, void* opaque)
{
    (void)ctx;
    function(opaque);
}


int POOL_tryAdd(POOL_ctx* ctx, POOL_function function, void* opaque)
{
    (void)ctx;
    function(opaque);
    return 1;
}

size_t POOL_sizeof(POOL_ctx* ctx)
{
    if (ctx==NULL)
    {
        return 0;    /* supports sizeof NULL */
    }
    assert(ctx == &g_ctx);
    return sizeof(*ctx);
}






