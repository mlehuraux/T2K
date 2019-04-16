/*******************************************************************************

                           _____________________

 File:        accu_array.h

 Description: An array of accumulators

 Author:      D. Calvet,        calvet@hep.saclay.cea.fr
              

 History:
  September 2014: created

*******************************************************************************/
#ifndef _ACCUARRAY_H
#define _ACCUARRAY_H

typedef struct _AccuArray {
	unsigned int   accu_sz;   // number of elements
	unsigned int  *accu;	  // pointer to array of accumulators
} AccuArray;

void AccuArray_Init(AccuArray *a, unsigned int accu_sz, unsigned int *b);
void AccuArray_Clear(AccuArray *a);
void AccuArray_Accumulate(AccuArray *a, unsigned int e, unsigned int v);
void AccuArray_Print(AccuArray *a, int v);

#endif

