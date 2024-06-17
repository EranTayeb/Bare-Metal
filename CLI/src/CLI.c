/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "XScuGic.h"
#include "xttcps.h"
#include "XGpio.h"
#include <stdio.h>
#include <string.h>

#include "xuartps.h"

#define LED1 0x01 << 20
#define LED2 0x01 << 21
#define LED_CHANNEL 1
#define MASK_BUTTON 0x01 <<17
#define DeviceId 0
int Counter = 0 ;
char  BufferPtr[100] ;
XGpio Gpio;
XScuGic IntcInstance;
int interruptReceived = 0;
u32  LED ;

XUartPs InstancePtr;

int main()
{
	u32 Status;
	u32 NumBytes = 1 ;
	int count =0 ;
	char * stringon = "led_on" ;
	char * stringoff = "led_off" ;

    init_platform();

    XUartPs_Config  *Config =  XUartPs_LookupConfig(DeviceId);

    Status = XUartPs_CfgInitialize( &InstancePtr, Config, Config->BaseAddress);

    //XUartPs_SetOperMode(&InstancePtr, XUARTPS_OPER_MODE_AUTO_ECHO);

    int Statuss = XGpio_Initialize(&Gpio, XPAR_GPIO_0_DEVICE_ID);
    	XGpio_SetDataDirection(&Gpio, LED_CHANNEL, ~LED1);

    XScuGic_Config *intConfig = XScuGic_LookupConfig(0);
    	if (intConfig == NULL) {
    		return XST_FAILURE;
    	}

    	u32 val;

    char command[100];
    int ledNumber ;
    int readerCounter =0 ;
    while (1) {
    	char ch;
		Status = XUartPs_Recv(&InstancePtr, &ch, NumBytes);
		if (Status != 0) {
			Status = XUartPs_Send(&InstancePtr, &ch, NumBytes);
			BufferPtr[count] = ch;
			count++;
			if ((BufferPtr[count-1] == '\n') || (BufferPtr[count-1] == '\r')) {
				BufferPtr[count - 1] = '\0';
				count = 0 ;
				readerCounter = sscanf(BufferPtr, "%s %d", command, &ledNumber);
				if(readerCounter < 2){
								printf("need more information for the command\r\n");
								continue;
							}


				if (strcmp(command, stringon) == 0){
					if (ledNumber == 1 ){
						LED = LED1 ;
					}
					if (ledNumber == 2 ){
						LED = LED2 ;
							}
					val = XGpio_DiscreteRead(&Gpio, LED_CHANNEL);
					XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, val | LED);
				}
				if (strcmp(command,stringoff) == 0) {
					if (ledNumber == 1 ){
										LED = LED1 ;
									}
									if (ledNumber == 2 ){
										LED = LED2 ;
											}
					        val = XGpio_DiscreteRead(&Gpio, LED_CHANNEL);
							XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, val & ~LED);
						}



				}

			}
		}

    return 0;
}
