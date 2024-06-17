//#define GPIO_EXAMPLE_DEVICE_ID  XPAR_GPIO_0_DEVICE_ID
//#define LED1 1<<17   /* Assumes bit 0 of GPIO is connected to an LED  */
//#define LED2 1<<22
//#define LED_CHANNEL 1
//#define CLOCKLED_ID 0
//#define CLOCKPRINT_ID 1
//#define INTC_DEVICE_ID 0
//#define INTERRUPT_LED_ID XPS_TTC0_0_INT_ID
//#define INTERRUPT_PRINT_ID XPS_TTC0_1_INT_ID
//
//
//
//
//#ifdef PRE_2_00A_APPLICATION
//
///*
// * The following macros are provided to allow an application to compile that
// * uses an older version of the driver (pre 2.00a) which did not have a channel
// * parameter. Note that the channel parameter is fixed as channel 1.
// */
//#define XGpio_SetDataDirection(InstancePtr, DirectionMask) \
//        XGpio_SetDataDirection(InstancePtr, LED_CHANNEL, DirectionMask)
//
//#define XGpio_DiscreteRead(InstancePtr) \
//        XGpio_DiscreteRead(InstancePtr, LED_CHANNEL)
//
//#define XGpio_DiscreteWrite(InstancePtr, Mask) \
//        XGpio_DiscreteWrite(InstancePtr, LED_CHANNEL, Mask)
//
//#define XGpio_DiscreteSet(InstancePtr, Mask) \
//        XGpio_DiscreteSet(InstancePtr, LED_CHANNEL, Mask)
//
//#endif
//
//
//#include <stdio.h>
//#include "platform.h"
//#include "xil_printf.h"
//#include "xparameters.h"
//#include "xgpio.h"
//#include "sleep.h"
//#include "XScuGic.h"
//#include "XTtcPs.h"
//
//XGpio Gpio; /* The Instance of the GPIO Driver */
//XScuGic intController;
//int status = 0;
//int counter_led = 0;
//int counter_print = 0;
//
//void led_hendler(void * callbackRef, u32 StatusEvent){
//	counter_led++;
//	if(counter_led%2 == 0){
//		u32 reg1 = XGpio_DiscreteRead(&Gpio, LED_CHANNEL);
//		XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, LED1^reg1);
//	}
//	if(counter_led % 3 == 0){
//		u32 reg2 = XGpio_DiscreteRead(&Gpio, LED_CHANNEL);
//		XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, LED2^reg2);
//	}
//}
//
//void print_hendler(void * callbackRef, u32 StatusEvent){
//	counter_print++;
//	if(counter_print % 10 == 0){
//		printf("%d\n\r",counter_print/10);
//	}
//}
//
//int initTtcClock(XTtcPs *clock,u16 device_ID, u32 interrupt_id,XTtcPs_StatusHandler handler,void * callBackRef){
//	XTtcPs_Config * ttc_config = XTtcPs_LookupConfig(device_ID);
//	status = XTtcPs_CfgInitialize(clock,ttc_config, ttc_config -> BaseAddress);
//	if(status != 0){
//		return 1;
//	}
//
//	status = XScuGic_Connect(
//				&intController,
//				interrupt_id,
//				(Xil_InterruptHandler)XTtcPs_InterruptHandler,
//				clock);
//
//	XTtcPs_SetStatusHandler(clock,
//							callBackRef,
//							handler
//							);
//	status =  XTtcPs_SetOptions(clock,XTTCPS_OPTION_INTERVAL_MODE);
//	if(status != 0){
//		return 1;
//	}
//	XInterval Interval;
//	u8 Prescaler;
//	XTtcPs_CalcIntervalFromFreq(clock,10,&Interval,&Prescaler);
//	XTtcPs_SetInterval(clock, Interval);
//	XTtcPs_SetPrescaler(clock, Prescaler);
//
//	XTtcPs_EnableInterrupts(clock, XTTCPS_IXR_INTERVAL_MASK);
//	XScuGic_Enable(&intController,interrupt_id);
//
//	XTtcPs_Start(clock);
//
//	return 0;
//}
//
//
//
//
//int main(){
//	init_platform();
//    XTtcPs clockLED;
//    XTtcPs clockPrint;
//	status = XGpio_Initialize(&Gpio, GPIO_EXAMPLE_DEVICE_ID);
//	if (status != 0) {
//		xil_printf("Gpio Initialization Failed\r\n");
//		return 1;
//	}
//
//	XGpio_SetDataDirection(&Gpio, LED_CHANNEL, ~(LED1 | LED2));
//
//	XScuGic_Config* intcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
//	if (intcConfig == NULL) {
//		return XST_FAILURE;
//	}
//	status = XScuGic_CfgInitialize(&intController, intcConfig, intcConfig->CpuBaseAddress);
//	if (status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//    status = initTtcClock(&clockLED,CLOCKLED_ID, INTERRUPT_LED_ID,(XTtcPs_StatusHandler)led_hendler,&Gpio);
//    status = initTtcClock(&clockPrint,CLOCKPRINT_ID, INTERRUPT_PRINT_ID,(XTtcPs_StatusHandler)print_hendler,&Gpio);
//
//
//    Xil_ExceptionInit();
//	Xil_ExceptionRegisterHandler(
//			XIL_EXCEPTION_ID_IRQ_INT,
//			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
//			&intController);
//
//    //Enable the CPU to receive interrupts from the device
//    Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
//
//    while(1);
//    cleanup_platform();
//    return 0;
//}
