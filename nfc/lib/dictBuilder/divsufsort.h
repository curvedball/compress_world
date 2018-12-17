


/*
 * divsufsort.h for libdivsufsort-lite
 * Copyright (c) 2003-2008 Yuta Mori All Rights Reserved.
 */

#ifndef _DIVSUFSORT_H
#define _DIVSUFSORT_H 1



#ifdef __cplusplus
extern "C" {
#endif


/*- Prototypes -*/

/**
 * Constructs the suffix array of a given string.
 * @param T [0..n-1] The input string.
 * @param SA [0..n-1] The output array of suffixes.
 * @param n The length of the given string.
 * @param openMP enables OpenMP optimization.
 * @return 0 if no error occurred, -1 or -2 otherwise.
 */
int divsufsort(const unsigned char *T, int *SA, int n, int openMP);



/**
 * Constructs the burrows-wheeler transformed string of a given string.
 * @param T [0..n-1] The input string.
 * @param U [0..n-1] The output string. (can be T)
 * @param A [0..n-1] The temporary array. (can be NULL)
 * @param n The length of the given string.
 * @param num_indexes The length of secondary indexes array. (can be NULL)
 * @param indexes The secondary indexes array. (can be NULL)
 * @param openMP enables OpenMP optimization.
 * @return The primary index if no error occurred, -1 or -2 otherwise.
 */
int divbwt(const unsigned char *T, unsigned char *U, int *A, int n, unsigned char * num_indexes, int * indexes, int openMP);


#ifdef __cplusplus
}
#endif


#endif



