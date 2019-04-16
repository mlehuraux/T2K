/*******************************************************************************

 File:        programflash.h

 Description: Definitions for functions to reprogram Feminos or TCM SPI
 Flash memory with a new bitstream


 Author:      D. Calvet,        denis.calvetATcea.fr
              

 History:
   October 2012 : created

   December 2017: added definition of page size and sector size for the two
   different types of flash supported (Winbond W25Q128BV and Spansion S25FL512S)

   December 2018: changed page size of S25FL512S to the correct value of 512
   bytes instead of 256

*******************************************************************************/

#ifndef _PROGRAM_FLASH_H
#define _PROGRAM_FLASH_H

/* Definition of flash types */
#define FLASH_TYPE_WINBOND_W25Q128BV  1
#define FLASH_TYPE_SPANSION_S25FL512S 2

/* For Winbond W25Q128BV flash */
#define WINBOND_W25Q128BV_FLASH_PAGE_SIZE     256
#define WINBOND_W25Q128BV_FLASH_SECTOR_SIZE  4096

/* For Spansion S25FL512S */
#define SPANSION_S25FL512S_FLASH_PAGE_SIZE       512
#define SPANSION_S25FL512S_FLASH_SECTOR_SIZE  262144

#define MAX_FLASH_PAGE_SIZE     ((WINBOND_W25Q128BV_FLASH_PAGE_SIZE > SPANSION_S25FL512S_FLASH_PAGE_SIZE) ? WINBOND_W25Q128BV_FLASH_PAGE_SIZE : SPANSION_S25FL512S_FLASH_PAGE_SIZE)
#define MAX_FLASH_SECTOR_SIZE   ((WINBOND_W25Q128BV_FLASH_SECTOR_SIZE > SPANSION_S25FL512S_FLASH_SECTOR_SIZE) ? WINBOND_W25Q128BV_FLASH_SECTOR_SIZE : SPANSION_S25FL512S_FLASH_SECTOR_SIZE)

#include "cmdfetcher.h"

int CmdFetcher_ProgramFlash(CmdFetcher *cf, int ft);

#endif
