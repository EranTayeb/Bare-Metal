
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

void Funbctm () __attribute__((section(".bctm")));
void demi () __attribute__((section(".bctm")));

INTPTR phys_addr = 0x20000;
u64 size = 0x10000 ;
u32 attributes = 0x070b;

int main()
{
	u32 num;
    init_platform();
	u32 Status;

    Funbctm ();
    num = Xil_GetNumOfFreeRegions();
    xil_printf("Get Num Of Free Regions : %u \n\r",num);

    Xil_DisableMPU();

    Status =  Xil_SetMPURegion(phys_addr,size,attributes);
    Xil_EnableMPU();
    num = Xil_GetNumOfFreeRegions();

    xil_printf("Get Num Of Free Regions : %u \n\r",num);




    return 0;
}

void Funbctm (){
	xil_printf("in fun");
	return;
}

void demi (){

}
