/*******************************************************************************

                           _____________________

 File:        accu_array.c

 Description: An array of accumulators

 Author:      D. Calvet,        calvet@hep.saclay.cea.fr
              

 History:
  September 2014: created

*******************************************************************************/
#include "accu_array.h"
#include <stdio.h>

/*******************************************************************************
AccuArray_Init()
*******************************************************************************/
void AccuArray_Init(AccuArray *a, unsigned int accu_sz, unsigned int *b)
{
	a->accu = b;
	a->accu_sz = accu_sz;
	AccuArray_Clear(a);
}

/*******************************************************************************
AccuArray_Clear()
*******************************************************************************/
void AccuArray_Clear(AccuArray *a)
{
	int i;

	for (i = 0; i < a->accu_sz; i++)
	{
		*(a->accu + i) = 0;
	}
}

/*******************************************************************************
AccuArray_Accumulate()
*******************************************************************************/
void AccuArray_Accumulate(AccuArray *a, unsigned int e, unsigned int v)
{
	unsigned int sgn, tmp;
	
	if (e < a->accu_sz)
	{
		sgn = *(a->accu + e) & 0x80000000;
		tmp = *(a->accu + e) + v;
		if ((!sgn) || (sgn & tmp))
		{
			*(a->accu + e) = tmp;
		}
		else
		{
			*(a->accu + e) = 0xFFFFFFFF;
		}
	}
}

/*******************************************************************************
AccuArray_Print()
*******************************************************************************/
void AccuArray_Print(AccuArray *a, int v)
{
	unsigned int i;

	for (i = 0; i<a->accu_sz; i++)
	{
		if ((v == 1) || (*(a->accu + i) != 0))
		{
			printf("Accu (%03d)=%5d\n", i, *(a->accu + i));
		}
	}
}
