/**************************************************************************//**
 * @file     main.c
 * @brief    N9H26 series EDMA demo code
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "N9H26.h"

#if defined(__GNUC__)
__attribute__((aligned(32))) UINT8 LoadAddr[]=
{
	#include "../VPOST/ASIC/sea_800x480_RGB565.dat"
};
#else
__align(32) UINT8 LoadAddr[]=
{
	#include "../VPOST/ASIC/sea_800x480_RGB565.dat"
};
#endif

LCDFORMATEX lcdFormat;

extern void spiSlaveTest(void);
void spiInit(void);

SPI_INFO_T  HostCPU_Slave;

int main()
{
	WB_UART_T uart;
	UINT32 u32ExtFreq, u32Item;

	u32ExtFreq = sysGetExternalClock();    	/* Hz unit */
	uart.uart_no = WB_UART_1; 
	uart.uiFreq = u32ExtFreq;
	uart.uiBaudrate = 115200;
	uart.uiDataBits = WB_DATA_BITS_8;
	uart.uiStopBits = WB_STOP_BITS_1;
	uart.uiParity = WB_PARITY_NONE;
	uart.uiRxTriggerLevel = LEVEL_1_BYTE;
	sysInitializeUART(&uart);	
	
	


    /********************************************************************************************** 
     * Clock Constraints: 
     * (a) If Memory Clock > System Clock, the source clock of Memory and System can come from
     *     different clock source. Suggestion MPLL for Memory Clock, UPLL for System Clock   
     * (b) For Memory Clock = System Clock, the source clock of Memory and System must come from 
     *     same clock source	 
     *********************************************************************************************/
#if 0 
    /********************************************************************************************** 
     * Slower down system and memory clock procedures:
     * If current working clock fast than desired working clock, Please follow the procedure below  
     * 1. Change System Clock first
     * 2. Then change Memory Clock
     * 
     * Following example shows the Memory Clock = System Clock case. User can specify different 
     * Memory Clock and System Clock depends on DRAM bandwidth or power consumption requirement. 
     *********************************************************************************************/
    sysSetSystemClock(eSYS_EXT, 12000000, 12000000);
    sysSetDramClock(eSYS_EXT, 12000000, 12000000);
#else 
    /********************************************************************************************** 
     * Speed up system and memory clock procedures:
     * If current working clock slower than desired working clock, Please follow the procedure below  
     * 1. Change Memory Clock first
     * 2. Then change System Clock
     * 
     * Following example shows to speed up clock case. User can specify different 
     * Memory Clock and System Clock depends on DRAM bandwidth or power consumption requirement.
     *********************************************************************************************/
    sysSetDramClock(eSYS_MPLL, 360000000, 360000000);
    sysSetSystemClock(eSYS_UPLL,            //E_SYS_SRC_CLK eSrcClk,
                      240000000,            //UINT32 u32PllKHz,
                      240000000);           //UINT32 u32SysKHz,
    sysSetCPUClock(240000000);
    sysSetAPBClock(60000000);	
#endif	

	sysEnableCache(CACHE_WRITE_BACK);

	sysSetLocalInterrupt(ENABLE_IRQ);

	/* init timer */		
	sysSetTimerReferenceClock (TIMER0, u32ExtFreq);	/* Hz unit */
	sysStartTimer(TIMER0, 
					100, 
					PERIODIC_MODE);

	//lcdFormat.ucVASrcFormat = DRVVPOST_FRAME_RGB565;	
	//vpostLCMInit(&lcdFormat, (UINT32*)LoadAddr);

	//EDMA_Init();
	//spiInit();

	do
	{
		sysprintf("==================================================================\n");
		sysprintf("This is demo code for HighTekVision PDMA SAMPLE CODE by SJLAB\n");
		sysprintf("If you want start DEMO press 'S' then quit 'Q'\n");
		sysprintf("[S] PDMA+SPI_SLAVE Test \n");	
		sysprintf("==================================================================\n");

		u32Item = sysGetChar();
		
		switch(u32Item) 
		{
			case 'S':
			  spiInit();  
			  EDMA_Init();
				spiSlaveTest();
				break;

			case 'Q':
			case 'q': 
				u32Item = 'Q';
				sysprintf("quit edma test...\n");				
				break;	
			}
		
	}while(u32Item!='Q');
	
}

void spiInit(void)
{
	  int volatile loop;
		UINT32 u32APBClk;
	
	  spiActive(0);
	
		HostCPU_Slave.nPort = 0;									// SPI Port Number
	  HostCPU_Slave.bIsSlaveMode = 1;						// Mater Mode = 0, Slave Mode = 1, Refer to main.c in SpiLoader folder
		HostCPU_Slave.bIsClockIdleHigh = 0;				// CLOCK State when idle (?CPOL) = 1
		HostCPU_Slave.bIsLSBFirst = 1;						// send MSB First = 0, send LSB First = 1   (test) MSB first
		HostCPU_Slave.bIsAutoSelect = 0;					// CS(Chip Select) : 1- Automatic Slave Select in Master, 0-Manual SSR[1:0]
		HostCPU_Slave.bIsActiveLow = 1;						// CS(Chip Select) Mode : Active High = 0, Active Low = 1
		HostCPU_Slave.bIsTxNegative = 1;					// CLOCK Phase (CPHA) : Tx falling-Rx rising = 1, Tx rising-Rx falling = 0
		HostCPU_Slave.bIsLevelTrigger = 0;				// Use Edge Trigger = 0. Level Trigger = 1
		HostCPU_Slave.bTxRxNum = 1;
	
		outpw(REG_APBCLK, inpw(REG_APBCLK) | SPIMS0_CKE);	// turn on SPI clk
		//Reset SPI controller first
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) | SPI0RST);
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~SPI0RST);
		// delay for time
		// Delay
	  for (loop=0; loop<500; loop++);  
		//configure SPI0 interface, Base Address 0xB800C000

		/* apb clock is 48MHz, output clock is 10MHz */
		u32APBClk = sysGetAPBClock();
		//spiIoctl(0, SPI_SET_CLOCK, u32APBClk/1000000, 48000);
		spiIoctl(0, SPI_SET_CLOCK, 48, 30000);
      
		#if 0 	
		//Startup SPI0 multi-function features, chip select using SS0
		//outpw(REG_GPDFUN1, (inpw(REG_GPDFUN1)  & ~0xFFFF0000) | 0x22220000);
#ifdef __OPT_O2DN__		
		outpw(REG_GPEFUN0, (inpw(REG_GPEFUN0)  & ~(MF_GPE0 |MF_GPE1)) | 0x33);
#else
		outpw(REG_GPEFUN1, (inpw(REG_GPEFUN1)  & ~(MF_GPE8 |MF_GPE9)) | 0x44);
#endif
     #endif
		
    while(1){
		  if(!spiOpen(&HostCPU_Slave));
			  break;
		}
		
		outpw(REG_SPI0_SSR, 	0x00000018);								
	  //outpw(REG_SPI0_CNTRL, 0x00040440);
		outpw(REG_SPI0_CNTRL, 0x00040040);
		
		outpw(REG_SPI0_CNTRL, inpw(REG_SPI0_CNTRL)|0x01);	// Waiting for the slave select input and serial clock input signals from the external master device as set GO_BUSY = 1
		
		
}







