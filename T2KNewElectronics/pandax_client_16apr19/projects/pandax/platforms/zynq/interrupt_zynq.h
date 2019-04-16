/*******************************************************************************
                           Minos / Feminos
                           _____________________

 File:        interrupt.h

 Description: Interrupt System.

 Author:      D. Calvet,        calvet@hep.saclay.cea.fr
              

 History:
  January 2015: created
  
*******************************************************************************/

#ifndef INTERRUPT_ZYNQ_H
#define INTERRUPT_ZYNQ_H

#include "xscugic.h"

extern XScuGic InterruptSystem;

int InterruptSystem_Setup();

#endif
