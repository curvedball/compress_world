



/* --- dependencies --- */
#include <stddef.h>   /* size_t */



/* --- simple histogram functions --- */

/*! HIST_count():
 *  Provides the precise count of each byte within a table 'count'.
 *  'count' is a table of unsigned int, of minimum size (*maxSymbolValuePtr+1).
 *  Updates *maxSymbolValuePtr with actual largest symbol value detected.
 *  @return : count of the most frequent symbol (which isn't identified).
 *            or an error code, which can be tested using HIST_isError().
 *            note : if return == srcSize, there is only one symbol.
 */
size_t HIST_count(unsigned* count, unsigned* maxSymbolValuePtr, const void* src, size_t srcSize);


//
unsigned HIST_isError(size_t code);  /*< tells if a return value is an error code */


/* --- advanced histogram functions --- */

#define HIST_WKSP_SIZE_U32 1024



/** HIST_count_wksp() :
 *  Same as HIST_count(), but using an externally provided scratch buffer.
 *  Benefit is this function will use very little stack space.
 * `workSpace` must be a table of unsigned of size >= HIST_WKSP_SIZE_U32
 */
size_t HIST_count_wksp(unsigned* count, unsigned* maxSymbolValuePtr, const void* src, size_t srcSize, unsigned* workSpace);



/** HIST_countFast() :
 *  same as HIST_count(), but blindly trusts that all byte values within src are <= *maxSymbolValuePtr.
 *  This function is unsafe, and will segfault if any value within `src` is `> *maxSymbolValuePtr`
 */
size_t HIST_countFast(unsigned* count, unsigned* maxSymbolValuePtr, const void* src, size_t srcSize);



/** HIST_countFast_wksp() :
 *  Same as HIST_countFast(), but using an externally provided scratch buffer.
 * `workSpace` must be a table of unsigned of size >= HIST_WKSP_SIZE_U32
 */
size_t HIST_countFast_wksp(unsigned* count, unsigned* maxSymbolValuePtr, const void* src, size_t srcSize, unsigned* workSpace);




/*! HIST_count_simple() :
 *  Same as HIST_countFast(), this function is unsafe,
 *  and will segfault if any value within `src` is `> *maxSymbolValuePtr`.
 *  It is also a bit slower for large inputs.
 *  However, it does not need any additional memory (not even on stack).
 * @return : count of the most frequent symbol.
 *  Note this function doesn't produce any error (i.e. it must succeed).
 */
unsigned HIST_count_simple(unsigned* count, unsigned* maxSymbolValuePtr, const void* src, size_t srcSize);



