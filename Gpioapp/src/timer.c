#include "xparameters.h"
#include "xgpio.h"
#include "xil_printf.h"
#include "sleep.h"
#include "XScuGic.h"
#include "xttcps.h"
#include <stdio.h>

#define LED1 0x01 << 20
#define LED2 0x01 << 21
#define LED_CHANNEL 1
#define MASK_BUTTON 0x01 <<17
#define DeviceId 0
int Counter = 0 ;
XGpio Gpio;
XScuGic IntController;
XTtcPs InstancePtr_TTC;
/* The Instance of the GPIO Driver */
void TickHander(void * callbackRef, u32 StatusEvent);

int main() {
	//init_platform();
	u32 Status;
	u32 Freq = 1;
	XInterval Interval;
	u8 Prescaler;
	XTtcPs_Config *Config = XTtcPs_LookupConfig(DeviceId);

	Status = XTtcPs_CfgInitialize(&InstancePtr_TTC, Config,Config->BaseAddress);
	if (Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	XTtcPs_CalcIntervalFromFreq(&InstancePtr_TTC, Freq, &Interval, &Prescaler);

	XScuGic_Config *intConfig = XScuGic_LookupConfig(0);
	if (intConfig == NULL) {
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(&IntController, intConfig,
			intConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XScuGic_Connect(&IntController, // The address of the interrupt controller
			XPS_TTC0_0_INT_ID, // Interrupt ID in the system
			XTtcPs_InterruptHandler, // System timer interrupt handler
			&InstancePtr_TTC); // Argument for the interrupt handler

	XTtcPs_SetStatusHandler(&InstancePtr_TTC, // The address of the timer controller
			&InstancePtr_TTC, // Argument for the interrupt handler
			TickHander); // User timer interrupt handler


	 XTtcPs_SetOptions(&InstancePtr_TTC, XTTCPS_OPTION_INTERVAL_MODE);

	 XTtcPs_SetInterval(&InstancePtr_TTC, Interval); // is a macro writing to a register
	 XTtcPs_SetPrescaler(&InstancePtr_TTC,Prescaler);

	 XTtcPs_EnableInterrupts(&InstancePtr_TTC, XTTCPS_IXR_INTERVAL_MASK);
	 XScuGic_Enable(&IntController,XPS_TTC0_0_INT_ID);

	 Xil_ExceptionInit();
	 Xil_ExceptionRegisterHandler(
	 	XIL_EXCEPTION_ID_IRQ_INT,             // the IRQ exception
	 			(Xil_ExceptionHandler) XScuGic_InterruptHandler, // a general interrupt handler
	 			&IntController                      // an interrupt handler argument
	 			// a pointer to the controller
	 			);


	 // At some later point when everything will be ready the IRQ exceptions should be allowed:
	 	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
	 	XTtcPs_Start(&InstancePtr_TTC);
        xil_printf("mamamamam\n\r");
	 	while (1) {
             ;
	 		}

	 	return 0;

}

void TickHander(void * callbackRef, u32 StatusEvent) {

	XTtcPs *InstancePtr_TTC = (XTtcPs *) callbackRef;
	xil_printf("the Counter is %d \n\r",Counter);
	Counter ++ ;


}

