/*******************************************************************************
                             PandaX-III / T2K-II
                             ___________________

File:        fdecoder.h

Description: Header file for decoder of binary data files recorded by the TDCM.


Author:      D. Calvet,        denis.calvetATcea.fr


History:
April 2019    : created


*******************************************************************************/

#ifndef FDECODER_H
#define FDECODER_H


#include "platform_spec.h"

typedef struct _Features {
	__int64  tot_file_rd;                    // Total number of bytes read from file
	char run_str[256];                       // Run information string
} Features;

#endif
