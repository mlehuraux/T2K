/*******************************************************************************

 File:        remotecopy.h

 Description: Definitions for functions to make remote copy of files on 
 SD Card of controlled node


 Author:      D. Calvet,        denis.calvetATcea.fr
              

 History:
   December 2017 : created

*******************************************************************************/

#ifndef _REMOTE_COPY_H
#define _REMOTE_COPY_H

#include "cmdfetcher.h"

int CmdFetcher_RemoteCopy(CmdFetcher *cf, char *src, char *dst);

#endif
