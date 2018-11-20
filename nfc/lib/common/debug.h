



#ifndef DEBUG_H_12987983217
#define DEBUG_H_12987983217


#if defined (__cplusplus)
extern "C" {
#endif



/* static assert is triggered at compile time, leaving no runtime artefact,
 * but can only work with compile-time constants.
 * This variant can only be used inside a function. */
#define DEBUG_STATIC_ASSERT(c) (void)sizeof(char[(c) ? 1 : -1])



/* DEBUGLEVEL is expected to be defined externally,
 * typically through compiler command line.
 * Value must be a number. */
#ifndef DEBUGLEVEL
#define DEBUGLEVEL 2
#endif



/* recommended values for DEBUGLEVEL :
 * 0 : no debug, all run-time functions disabled
 * 1 : no display, enables assert() only
 * 2 : reserved, for currently active debug path
 * 3 : events once per object lifetime (CCtx, CDict, etc.)
 * 4 : events once per frame
 * 5 : events once per block
 * 6 : events once per sequence (verbose)
 * 7+: events at every position (*very* verbose)
 *
 * It's generally inconvenient to output traces > 5.
 * In which case, it's possible to selectively enable higher verbosity levels
 * by modifying g_debug_level.
 */
#if (DEBUGLEVEL >= 1)
	#include <assert.h>
#else
	#ifndef assert   /* assert may be already defined, due to prior #include <assert.h> */
		#define assert(condition) ((void)0)   /* disable assert (default) */
	#endif
#endif




//
#if (DEBUGLEVEL >= 2)
	#include <stdio.h>
	extern int g_debuglevel; /* here, this variable is only declared,
                           it actually lives in debug.c,
                           and is shared by the whole process.
                           It's typically used to enable very verbose levels
                           on selective conditions (such as position in src) */

	#define RAWLOG(l, ...) {                                      \
                if (l<=g_debuglevel) {                          \
                    fprintf(stderr, __VA_ARGS__);               \
            }   }


	#define DEBUGLOG(l, ...) {                                    \
                if (l<=g_debuglevel) {                          \
                    fprintf(stderr, __FILE__ ": " __VA_ARGS__); \
                    fprintf(stderr, " \n");                     \
            }   }
#else
	#define RAWLOG(l, ...)      {}    /* disabled */
	#define DEBUGLOG(l, ...)    {}    /* disabled */
#endif



#define DbgPrintSwitch 	0

#if DbgPrintSwitch
	#define DbgPrint printf
#else
	#define DbgPrint(fmt, ...)
#endif







#if defined (__cplusplus)
}
#endif


#endif





