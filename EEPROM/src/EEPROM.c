
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

#define DeviceId 0
#define ByteCount 256
#define READ_CHUNK_SIZE 32

void PrintData(u8 *Buffer, int Length);
int main()
{
    init_platform();
	u32 Status;
	XIicPs InstancePtr ;
	u32 SclHz = 100000;
	u16 StartAddress = 0x0 ;
	u16 EEPROM = 0x50;
	u8 WriteBuffer[2] ;
	u8 ReadBuffer[32] ;


    XIicPs_Config *Config = XIicPs_LookupConfig(DeviceId);

    Status = XIicPs_CfgInitialize(&InstancePtr,Config,Config->BaseAddress);
	if (Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = XIicPs_SetSClk(&InstancePtr,SclHz);

		WriteBuffer[0]=0;
		WriteBuffer[1]=0;

         Status = XIicPs_MasterSendPolled(&InstancePtr, &WriteBuffer, 2, EEPROM);
         if (Status != XST_SUCCESS) {
             return XST_FAILURE;
         }
        while( XIicPs_BusIsBusy(&InstancePtr));



	for (int i = 0; i < ByteCount; i += READ_CHUNK_SIZE) {


	            Status = XIicPs_MasterRecvPolled(&InstancePtr, ReadBuffer, 32, EEPROM);
		        if (Status != XST_SUCCESS) {
			         return XST_FAILURE;
			    }
		           while( XIicPs_BusIsBusy(&InstancePtr));

		        PrintData(ReadBuffer, 32);

	}

    return 0;
}

void PrintData(u8 *Buffer, int Length) {
    for (int i = 0; i < Length; i++) {
        if (Buffer[i] < 32 || Buffer[i] > 127) {
            xil_printf(" ");
        } else {
            xil_printf("%c", Buffer[i]);
        }
    }
    xil_printf("\r\n");
}
