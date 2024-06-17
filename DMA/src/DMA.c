

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "XScuGic.h"
#include "xttcps.h"
#include "XGpio.h"
#include "xiicps.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "xil_mpu.h"
#include "xzdma.h"

#define SIZE 2000
#define mask XZDMA_IXR_DMA_DONE_MASK
XZDma_DataConfig configure;
XZDma_Transfer Data;
int SrcBuf[SIZE]__attribute__ ((aligned (32)));
int DstBuf[SIZE]__attribute__ ((aligned (32)));
XZDma_Config *config ;
int interupt = 0 ;
int count = 0 ;

void arr_int (int* arr);
void arr_int2 (int* arr);

void DoneHandler(void * arg);
void ChannelConfiguration ();

int main()
{
    init_platform();
    int Status ;
	XZDma zdma;
	XScuGic intcInstance;

	arr_int(SrcBuf);
	u8 isSimpleMode = TRUE ;

	XScuGic_Config *intConfig = XScuGic_LookupConfig(0);
		if (intConfig == NULL) {
			return XST_FAILURE;
		}
		Status = XScuGic_CfgInitialize(&intcInstance, intConfig,
				intConfig->CpuBaseAddress);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}


    // Find Specific DMA configuration
    config = XZDma_LookupConfig(XPAR_PSU_GDMA_0_DEVICE_ID);
    if (config == NULL)
		return XST_FAILURE;

    Status = XZDma_CfgInitialize(&zdma, config, config->BaseAddress);
    if (Status != XST_SUCCESS)
    		return XST_FAILURE;

	Status = XScuGic_Connect(&intcInstance,
			XPAR_PSU_GDMA_0_INTR,
			(Xil_ExceptionHandler) XZDma_IntrHandler, // Xilinx interrupt handler
			(void *) &zdma);
    if (Status != XST_SUCCESS)
     		return XST_FAILURE;

    XZDma_SetCallBack(&zdma, XZDMA_HANDLER_DONE, (void *)DoneHandler, &zdma);
    ChannelConfiguration ();
	XZDma_SetChDataConfig(&zdma, &configure);

    XZDma_EnableIntr(&zdma,  mask);

	XScuGic_Enable(&intcInstance, XPAR_PSU_GDMA_0_INTR);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,             // the IRQ exception
				(Xil_ExceptionHandler) XScuGic_InterruptHandler, // a general interrupt handler
				&intcInstance                      // an interrupt handler argument
				// a pointer to the controller
				);

		// At some later point when everything will be ready the IRQ exceptions should be allowed:
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

    Status = XZDma_SetMode(&zdma, FALSE, XZDMA_NORMAL_MODE); // FALSE – means simple

	Data.DstAddr = (UINTPTR)DstBuf;
	Data.SrcAddr = (UINTPTR)SrcBuf;
	Data.Size = SIZE; // Size of data to transfer in bytes
	Data.DstCoherent = config->IsCacheCoherent ? 1 : 0;
	Data.SrcCoherent = config->IsCacheCoherent ? 1 : 0;
	Data.Pause = 0; // Valid only for scatter-gather

	 if (!(config->IsCacheCoherent) ){
	    	Xil_DCacheFlushRange((INTPTR)SrcBuf, SIZE);
	    	Xil_DCacheInvalidateRange((INTPTR)DstBuf, SIZE);
	    }

	XZDma_Start(&zdma, &Data, 1); // Start the data transfer,




while (1){
	if (interupt == 1 ){
		if (memcmp(SrcBuf,DstBuf,SIZE) == 0){
			xil_printf("is working");
		} else 		xil_printf("is not working");
		if (count == 1 ){
			arr_int2(SrcBuf);
    	    if (!(config->IsCacheCoherent)) {
    	    		Xil_DCacheFlushRange((INTPTR) SrcBuf, SIZE);
    	    		Xil_DCacheInvalidateRange((INTPTR) DstBuf, SIZE);
    	    	}
    	   XZDma_Start(&zdma, &Data, 1); // Start the data transfer
		}

		interupt = 0 ;
	}
}

    cleanup_platform();
    return 0;
}

void arr_int (int* arr ){
	for (int i = 0 ; i<SIZE;i++){
		arr[i]=i;
	}
	return;
}
void arr_int2 (int* arr ){
	for (int i = 0 ; i<SIZE;i++){
		arr[i]=2;
	}
	return;
}

void DoneHandler(void * arg){
	interupt = 1;
	count++;
	return;
}

void ChannelConfiguration ()
{
	configure.OverFetch = 1; // Allow over fetch
	configure.SrcIssue = 0x1F; // Number of outstanding transactions, 1F - maximum
	configure.SrcBurstType = XZDMA_INCR_BURST; // Fixed or Incremental
	configure.SrcBurstLen = 0xF; // 0 – 1, 1 – 2, 2 – 4, 3 – 8, 4+ - 16 data beats
	configure.DstBurstType = XZDMA_INCR_BURST; // Fixed or Incremental
	configure.DstBurstLen = 0xF; // 0 – 1, 1 – 2, 2 – 4, 3 – 8, 4+ - 16 data beats
	configure.SrcCache = config->IsCacheCoherent ? 0xF : 0x2;
	configure.DstCache = config->IsCacheCoherent ? 0xF : 0x2;
	configure.SrcQos = 0;
	configure.DstQos = 0;

	return;
}

