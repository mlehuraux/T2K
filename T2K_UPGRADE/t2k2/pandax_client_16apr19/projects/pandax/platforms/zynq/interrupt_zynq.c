/*******************************************************************************
                           Minos / Feminos - TCM
                           _____________________

 File:        interrupt_zynq.c

 Description: Interrupt System.

 Adapted from code provided by Xilinx

 Author:      D. Calvet,        calvet@hep.saclay.cea.fr
              

 History:
  February 2015: created from Mars MX2 version
  
*******************************************************************************/
#include "platform_spec.h"
#include "interrupt_zynq.h"

#include <stdio.h>

/*
 * Global variable
 */
XScuGic InterruptSystem;
static XScuGic_Config *GicConfig;

/*****************************************************************************
 InterruptSystem_Setup
******************************************************************************/
int InterruptSystem_Setup(XScuGic *x_scugic)
{
	int Status;

	//printf("InterruptSystem_Setup started\n");

	Xil_ExceptionInit();

	// Initialize the interrupt controller driver so that it is ready to use
	if ((GicConfig = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID)) == NULL)
	{
		return (-1);
	}
	if ((Status = XScuGic_CfgInitialize(x_scugic, GicConfig, GicConfig->CpuBaseAddress)) != XST_SUCCESS)
	{
		return (-1);
	}

	/*
	* Connect the interrupt controller interrupt handler to the hardware
	* interrupt handling logic in the ARM processor.
	*/
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, x_scugic);

	// Enable interrupts in the ARM
	Xil_ExceptionEnable();

	//printf("InterruptSystem_Setup done\n");

	return XST_SUCCESS;
}

