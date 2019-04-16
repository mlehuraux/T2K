/*******************************************************************************
                           
                           _____________________

 File:        frame_scan.h

 Description: Feminos Frame Decoder


 Author:      D. Calvet,        calvet@hep.saclay.cea.fr
              

 History:

   Semptember 2014: created 

*******************************************************************************/
#ifndef FRAME_SCAN_H
#define FRAME_SCAN_H

#include "mreader.h"

void Frame_Scan(Features *f, void *fp, void *fr, int fr_sz, unsigned int vflg);

#endif
