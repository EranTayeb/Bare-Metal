
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "XScuGic.h"
#include "xttcps.h"
#include "XGpio.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

u32 Global ;
static int intstatic ;

int intbtcm __attribute__((section(".btcm")));

void Funatcm () __attribute__((section(".atcm")));
void printAddress(const char * message, u32 addr);


int main()
{
    init_platform();

	static int intstaticmain ;
	int Local ;

	printAddress ("static variable in the main          ",(u32) &intstaticmain);
	printAddress ("local variable in the main           ",(u32) &Local);
	printAddress ("global variable that located in BTCM ",(u32) &intbtcm);
	printAddress ("function that located in ATCM        ",(u32) &Funatcm);
	printAddress ("global variable                      ",(u32) &Global);
	printAddress ("static variable defined in file      ",(u32) &intstatic);




    return 0;
}

void printAddress(const char * message, u32 addr) {
    const char* memoryType;

    if (addr >= 0xFFFC0000 && addr < (0xFFFC0000 + 0x40000)) {
        memoryType = "OCM";
    }
    else if (addr >= 0xC0000000 && addr < (0xC0000000 + 0x20000000)) {
        memoryType = "QSPI";
    }
    else if (addr >= 0x0 && addr < (0x0 + 0x10000)) {
        memoryType = "ATCM";
    }
    else if (addr >= 0x20000 && addr < (0x20000 + 0x10000)) {
        memoryType = "BTCM";
    }
    else if (addr >= 0x100000 && addr < (0x100000 + 0x7FE00000)) {
        memoryType = "DDR";
    }
    else {
        memoryType = "Unknown";
    }

    xil_printf("%s: %x located in %s\n\r", message, addr, memoryType);
}

void Funatcm () {
}


