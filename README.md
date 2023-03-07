# N9H26K5_MPU_Pdma

### Description
Just simple SPI slave PDMA interrupt test code 4 Hightek

### MPU Enviorment
|N|Name|Description|Note|
|---|---|---|---|
|1|ARM MDK|Keil uVision Ver. 5.34.0.0|IDE|
|2|N9H26_NonOS_BSP|2023_02_15 Released|[Link](https://github.com/OpenNuvoton/N9H26_NonOS_BSP)|

### Debug Image
![image](https://user-images.githubusercontent.com/108905975/223362388-6e89752b-6865-4660-8f8a-9959eafd7456.png)

### How to edit code

``` C
//Smpl_Pdma_SPI.c

//If you want to increase the size of the PDMA buffer, Change the TEST_SIZE of Smpl_Pdma_SPI.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "N9H26.h"

#define TEST_SIZE 16  //←Edit this Value
...
```

``` C
//spi.c

//I modified the function below to modify the spislavtx/rx FIFO size.

INT32 spiOpen(SPI_INFO_T *pInfo)
{
...
  if (pInfo->bTxRxNum)
    outpw(REG_SPI0_CNTRL, inpw(REG_SPI0_CNTRL) | 0XE00000);  //for 8byte SPI TX/RX Burst Mode
}
```

### Turbo Writer Setting
![image](https://user-images.githubusercontent.com/108905975/223366356-98804788-1ce1-4e3b-bc27-db8b5f110d64.png)
|N|Name|Description|Note|
|---|---|---|---|
|1|N9H26_NANDLoader_240MHz_Fast.bin|for NAND flash booting|
|2|EDMA.bin|excute code|
