/*******************************************************************************
                           
                           _____________________

 File:        platform_spec.h

 Description: Specific include file and definitions for MicroBlaze standalone
 in Spartan 6

 Author:      D. Calvet,        calvet@hep.saclay.cea.fr
              

 History:
  May 2011: created
  November 2014: added cast to unsigned short for each half of the expression
  of ntohs and htons. Without this, the macro failed on the MicroBlaze in
  Artix 7 with -O1 and -O2 optimisation when x is aligned on a 2 byte boundary.
  The reason is not completely understood.

*******************************************************************************/
#ifndef PLATFORM_SPEC_H
#define PLATFORM_SPEC_H

#include "xparameters.h"
#include "xil_cache.h"

#define __int64 long long

#define yield();

#define htons(x) (((unsigned short)(((x)>>8) & 0x00FF)) | ((unsigned short)(((x) & 0xFF)<<8)))
#define ntohs(x) (((unsigned short)(((x)>>8) & 0x00FF)) | ((unsigned short)(((x) & 0xFF)<<8)))
#define htonl(x) ((x))
#define ntohl(x) ((x))

#endif
