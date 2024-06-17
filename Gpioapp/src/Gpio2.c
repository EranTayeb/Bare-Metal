#include "xparameters.h"
#include "xgpio.h"
#include "xil_printf.h"
#include "sleep.h"
#include "XScuGic.h"
#define LED1 0x01 << 17
#define LED2 0x01 << 22
#define LED_CHANNEL 1
#define MASK_BUTTON 0x01 <<27
XGpio Gpio;
XScuGic IntcInstance;
int interruptReceived = 0;
/* The Instance of the GPIO Driver */
void GpioHandler(void *callbackRef);

int main() {
	init_platform();
	int led1_2 = LED1 | LED2;

	//Initialize GPIO
	int Status = XGpio_Initialize(&Gpio, XPAR_GPIO_0_DEVICE_ID);
	XGpio_SetDataDirection(&Gpio, LED_CHANNEL, ~led1_2);
	// Initialize and enable Interrupt controller
	//Status = XScuGic_Initialize(&IntcInstance, 0);
	XScuGic_Config *intConfig = XScuGic_LookupConfig(0);
	if (intConfig == NULL) {
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(&IntcInstance, intConfig,
			intConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XScuGic_Connect(&IntcInstance, XPAR_FABRIC_GPIO_0_VEC_ID,
			(Xil_InterruptHandler) GpioHandler, (void *) &Gpio);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE; // Indicates a failure to connect the interrupt handler
	}
	XScuGic_Enable(&IntcInstance, XPAR_FABRIC_GPIO_0_VEC_ID);
	// Enable the GPIO interrupt in the GIC
	XGpio_InterruptEnable(&Gpio, 1);
	// Enable the interrupt for GPIO channel
	XGpio_InterruptGlobalEnable(&Gpio);
	// Enable all interrupts in the GPIO
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(
	XIL_EXCEPTION_ID_IRQ_INT,             // the IRQ exception
			(Xil_ExceptionHandler) XScuGic_InterruptHandler, // a general interrupt handler
			&IntcInstance                      // an interrupt handler argument
			// a pointer to the controller
			);

	// At some later point when everything will be ready the IRQ exceptions should be allowed:
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
	while (1) {
		u32 val;
		if (interruptReceived == 1) {

			val = XGpio_DiscreteRead(&Gpio, LED_CHANNEL);
			val ^= LED1;
			XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, val);
			interruptReceived = 0;
		}
	}
	return 0;
}

void GpioHandler(void *callbackRef) {
	XGpio *gpioPtr = (XGpio *) callbackRef; // Convert generic pointer to a pointer to the specific object
	// Implement the interrupt logic
	u32 val = XGpio_DiscreteRead(&Gpio, 1);
	if ((MASK_BUTTON & val) == 0) {

		interruptReceived = 1;
	}
	XGpio_InterruptClear(gpioPtr, 1); // Clear the interrupt
}
