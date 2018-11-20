

#ifndef ZSTD_COMPILER_H
#define ZSTD_COMPILER_H


/* force inlining */
#define INLINE_KEYWORD inline


#define FORCE_INLINE_ATTR __attribute__((always_inline))



/**
 * FORCE_INLINE_TEMPLATE is used to define C "templates", which take constant
 * parameters. They must be inlined for the compiler to elimininate the constant
 * branches.
 */
#define FORCE_INLINE_TEMPLATE static INLINE_KEYWORD FORCE_INLINE_ATTR



//
#define HINT_INLINE static INLINE_KEYWORD FORCE_INLINE_ATTR



/* force no inlining */
#define FORCE_NOINLINE static __attribute__((__noinline__))



/* target attribute */
#ifndef __has_attribute
#define __has_attribute(x) 0  /* Compatibility with non-clang compilers. */
#endif


//
#define TARGET_ATTRIBUTE(target) __attribute__((__target__(target)))



/* prefetch */
#define PREFETCH(ptr)   __builtin_prefetch(ptr, 0, 0)


#endif



