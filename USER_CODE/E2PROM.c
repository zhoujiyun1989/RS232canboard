/****************************************Copyright (c)****************************************************
**                            Guangzhou ZLGMCU Development Co., LTD
**
**                                 http://www.zlgmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           E2PROM.c
** Last modified Date:  2010-10-12
** Last Version:        V1.0
** Descriptions:        利用FlashIAP特性进行模拟片内EEPROM
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Lanwuqiang
** Created date:        2010-10-12
** Version:             V1.00
** Descriptions:        利用FlashIAP特性进行模拟片内EEPROM
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Modified by:        
** Modified date:      
** Version:            
** Descriptions:       
**
** Rechecked by:
*********************************************************************************************************/
#include "E2PROM.h"
#include <stdio.h>
#include <stdlib.h>

/*
 *  定义函数指针  
 */
void (*IAP_Entry) (unsigned long ulParam_tab[], unsigned long ulPesult_tab[]) = (void(*)())IAP_ENTER_ADR;

unsigned long   GulParamin[8];                                           /*  IAP入口参数缓冲区           */
unsigned long   GulParamout[8];                                          /*  IAP出口参数缓冲区           */
/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP操作扇区选择，命令代码50
** input parameters:    ucSec1:           起始扇区
**                      ucSec2:           终止扇区
** output parameters:   GulParamout[0]:   IAP操作状态码,IAP返回值
** Returned value:      GulParamout[0]:   IAP操作状态码,IAP返回值
*********************************************************************************************************/
unsigned long  sectorPrepare (unsigned char  ucSec1, unsigned char  ucSec2)
{  
    GulParamin[0] = IAP_Prepare;                                        /*  设置命令字                  */
    GulParamin[1] = ucSec1;                                             /*  设置参数                    */
    GulParamin[2] = ucSec2;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  调用IAP服务程序             */
   
    return (GulParamout[0]);                                            /*  返回状态码                  */
}

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        复制RAM的数据到FLASH，命令代码51
** input parameters:    ulDst:             目标地址，即FLASH起始地址。以512字节为分界
**                      ulSrc:             源地址，即RAM地址。地址必须字对齐
**                      ulNo:              复制字节个数，为512/1024/4096/8192
** output parameters:   GulParamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      GulParamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
unsigned long  ramCopy (unsigned long  ulDst, unsigned long  ulSrc, unsigned long  ulNo)
{  
    GulParamin[0] = IAP_RAMTOFLASH;                                     /*  设置命令字                  */
    GulParamin[1] = ulDst;                                              /*  设置参数                    */
    GulParamin[2] = ulSrc;
    GulParamin[3] = ulNo;
    GulParamin[4] = IAP_FCCLK;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  调用IAP服务程序             */
    
    return (GulParamout[0]);                                            /*  返回状态码                  */
}

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        扇区擦除，命令代码52
** input parameters:    ucSec1             起始扇区
**                      ucSec2             终止扇区92
** output parameters:   GulParamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      GulParamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
unsigned long  sectorErase (unsigned char ucSec1, unsigned char  ucSec2)
{  
    GulParamin[0] = IAP_ERASESECTOR;                                    /*  设置命令字                  */
    GulParamin[1] = ucSec1;                                             /*  设置参数                    */
    GulParamin[2] = ucSec2;
    GulParamin[3] = IAP_FCCLK;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  调用IAP服务程序             */

    return (GulParamout[0]);                                            /*  返回状态码                  */
}

/*********************************************************************************************************
** Function name:       blankChk
** Descriptions:        扇区查空，命令代码53
** input parameters:    ucSec1:              起始扇区
**                      ucSec2:              终止扇区92
** output parameters:   GulParamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      GulParamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
unsigned long  blankChk (unsigned char  ucSec1, unsigned char  ucSec2)
{  
    GulParamin[0] = IAP_BLANKCHK;                                       /*  设置命令字                  */
    GulParamin[1] = ucSec1;                                             /*  设置参数                    */
    GulParamin[2] = ucSec2;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  调用IAP服务程序             */

    return (GulParamout[0]);                                            /*  返回状态码                  */
}

/*********************************************************************************************************
** Function name:       parIdRead
** Descriptions:        读器件标识号，命令代码54
** input parameters:    无
** output parameters:   GulParamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      GulParamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
unsigned long  parIdRead (void)
{
    GulParamin[0] = IAP_READPARTID;                                     /*  设置命令字                  */
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  调用IAP服务程序             */

    return (GulParamout[0]);                                            /*  返回状态码                  */
}

/*********************************************************************************************************
** Function name:       codeIdBoot
** Descriptions:        读Boot代码版本号，命令代码55
** input parameters:    无
** output parameters:   GulParamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      GulParamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
unsigned long  codeIdBoot (void)
{
    GulParamin[0] = IAP_BOOTCODEID;                                     /*  设置命令字                  */
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  调用IAP服务程序             */

    return (GulParamout[0]);                                            /*  返回状态码                  */
}

/*********************************************************************************************************
** Function name:       dataCompare
** Descriptions:        校验数据，命令代码56
** input parameters:    ulDst:             目标地址，即RAM/FLASH起始地址。地址必须字对齐
**                      ulSrc:             源地址，即FLASH/RAM地址。地址必须字对齐
**                      ulNo:              待比较的字节数。计数值应当为4的倍数字
** output parameters:   GulParamout[0]:    IAP操作状态码,IAP返回值
** Returned value:      GulParamout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
unsigned long  dataCompare (unsigned long  ulDst, unsigned long  ulSrc, unsigned long  ulNo)
{  
    GulParamin[0] = IAP_COMPARE;                                        /*  设置命令字                  */
    GulParamin[1] = ulDst;                                              /*  设置参数                    */
    GulParamin[2] = ulSrc;
    GulParamin[3] = ulNo;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  调用IAP服务程序             */

    return (GulParamout[0]);                                            /*  返回状态码                  */
}

/********************************************************************************************************
** 用户接口层
********************************************************************************************************/
/*********************************************************************************************************
** Function name:       eepromWrite
** Descriptions:        调用该函数可以模拟写EEPROM操作，最大写数据256字节
** input parameters:    bank:从EEPROM空间开始的存储器bank编号,一个bank 256字节
**                      src_addr:存储数据的源地址
** output parameters:   状态信息
** Returned value:      无
**                      一次写入的数据为256字节
注意：调用该函数将会使定义为EEPROM的Flash全部擦除，即会使前面写的所有数据丢失
*********************************************************************************************************/
uint8_t eepromWrite(uint8_t bank, uint8_t * src_addr)
{
    uint8_t ucErr;
  
    zyIrqDisable();                                                     /*  在IAP操作前必须关闭所有中断 */
    
    if (bank > ((EEPROM_ENDSEC-EEPROM_STARTSEC+1) * 16)) {              /*  bank参数检查                */
       zyIrqEnable(); 
       return PARA_BANK_ERROR;
    }
  
    if ( (((unsigned long)src_addr)%4 )!=0 ) {
       zyIrqEnable(); 
       return PARA_SRCADDR_ERROR;  
    }
  
    ucErr = sectorPrepare(EEPROM_STARTSEC,EEPROM_ENDSEC);
    ucErr = sectorErase(EEPROM_STARTSEC, EEPROM_ENDSEC);
    ucErr = blankChk(EEPROM_STARTSEC, EEPROM_ENDSEC);
  
    ucErr = sectorPrepare(EEPROM_STARTSEC, EEPROM_ENDSEC);
    ucErr = ramCopy(EEPROM_STARTSEC*1024*4+bank*256, (unsigned long)src_addr, 256);
    ucErr = dataCompare(EEPROM_STARTSEC*1024*4+bank*256, (unsigned long)src_addr, 256);
                                                                        /*  比较数据                    */

    if (ucErr != CMD_SUCCESS) {                                         /*  IAP函数调用出错             */
      zyIrqEnable();  
      return IAP_ERROR;
    }
  
    zyIrqEnable(); 
  
    return EE_SUUCEESS;
}

/*********************************************************************************************************
** Function name:       eepromWriteNByte
** Descriptions:        调用该函数可以模拟写EEPROM操作，最大写数据256字节
** input parameters:    src_addr:存储数据的源地址
** output parameters:   状态信息
** Returned value:      无
**                      一次写入的数据为256字节
注意：调用该函数将会使定义为EEPROM的Flash全部擦除，即会使前面写的所有数据丢失
*********************************************************************************************************/
uint8_t eepromWriteNByte(uint8_t * src_addr, uint32_t eeprom_addr, uint32_t ulNum)
{
    uint8_t ucErr, startSec, endSec, ucSecNum;
    uint8_t *pIapData, *pIapDatatemp;
    uint32_t ulOffset, len;
    
    zyIrqDisable();                                                     /*  在IAP操作前必须关闭所有中断 */
    
  
    if ( (((unsigned long)src_addr)%4 )!=0 ) {
       zyIrqEnable(); 
       return PARA_SRCADDR_ERROR;  
    }
    startSec = EEPROM_STARTSEC + eeprom_addr;
    ucSecNum = ulNum/4096+1;
    if (ulNum > 4096) {
       endSec   = ucSecNum + startSec;
       if (endSec > EEPROM_ENDSEC) {
           return PARA_OVERFLOW_ERROR;
       }
       len = 4096*(ulNum/4096 + 1);
       pIapDatatemp = malloc(len);
    } else {
       endSec = startSec;
       len = 4096;
       pIapDatatemp = malloc(len);
    }
    if (pIapDatatemp == NULL) {
        return PARA_OVERFLOW_ERROR;
    }
    pIapData = (uint8_t *)((uint32_t)pIapDatatemp&(~0x03));

    eepromRead( startSec << 12, pIapData, len);
    /* 修改EEPROM内容 */
    for (ulOffset = 0; ulOffset < ulNum; ulOffset++) {
        pIapData[eeprom_addr + ulOffset] =  *(src_addr + ulOffset);
    }
    /* 写入EEPROM     */
    ucErr = sectorPrepare(startSec, endSec);
    ucErr = sectorErase(startSec, endSec);
    ucErr = blankChk(startSec, endSec);
  
    ucErr = sectorPrepare(startSec, endSec);
    ucErr = ramCopy(startSec << 12, (uint32_t)pIapData, len);
    ucErr = dataCompare(startSec << 12, (uint32_t)pIapData, len);
                                                                        /*  比较数据                    */

    if (ucErr != CMD_SUCCESS) {                                         /*  IAP函数调用出错             */
      zyIrqEnable();  
      free(pIapDatatemp);
      return IAP_ERROR;
    }
    free(pIapDatatemp);
    zyIrqEnable(); 
    return EE_SUUCEESS;
}

/*********************************************************************************************************
** Function name:       eepromRead
** Descriptions:        调用该函数可以读从EEPROM_STARTSEC(EEPROM起始扇区开始的数据)
** input parameters:    src_addr:当做EEPROM存储空间的偏移量地址
**                      dst_addr:接收读取数据的源地址
**                      num:读取的字节数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void eepromRead(uint32_t src_addr, uint8_t *dst_addr, uint32_t num)
{
    uint32_t i;
  
    for (i = 0; num != 0; num--, i++) {    
        *(dst_addr + i)= *(((uint8_t * )(EEPROM_STARTSEC*1024*4 + src_addr)) + i);
    }
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
