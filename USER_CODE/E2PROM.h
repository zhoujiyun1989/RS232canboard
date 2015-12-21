/****************************************Copyright (c)****************************************************
**                            Guangzhou ZLGMCU Development Co., LTD
**
**                                 http://www.zlgmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           E2PROM.h
** Last modified Date:  2010-10-12
** Last Version:        V1.0
** Descriptions:        利用Flash特性模拟片内EEPROM
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Lanwuqiang
** Created date:        2010-10-12
** Version:             V1.00
** Descriptions:        利用Flash特性模拟片内EEPROM
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
#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "LPC11xx.h"

#define zyIrqEnable()   __enable_irq() 
#define zyIrqDisable()  __disable_irq()

#define IAP_ENTER_ADR   0x1FFF1FF1                                      /*  IAP入口地址定义             */

/*
 *  定义IAP命令字
 */
#define IAP_Prepare             50                                      /*  选择扇区                    */
                                                                        /*  【起始扇区号、结束扇区号】  */
#define IAP_RAMTOFLASH          51                                      /*  拷贝数据 FLASH目标地址      */
                                                                        /*  RAM源地址    【、写入字节数 */
                                                                        /*  系统时钟频率】              */
#define IAP_ERASESECTOR         52                                      /*  擦除扇区    【起始扇区号    */
                                                                        /*  结束扇区号、系统时钟频率】  */
#define IAP_BLANKCHK            53                                      /*  查空扇区    【起始扇区号、  */
                                                                        /*  结束扇区号】                */
#define IAP_READPARTID          54                                      /*  读器件ID    【无】          */
#define IAP_BOOTCODEID          55                                      /*  读Boot版本号【无】          */
#define IAP_COMPARE             56                                      /*  比较命令    【Flash起始地址 */
                                                                        /*  RAM起始地址、需要比较的     */
                                                                        /*  字节数】                    */
/*
 *  定义IAP返回状态字
 */
#define CMD_SUCCESS                                0
#define INVALID_COMMAND                            1
#define SRC_ADDR_ERROR                             2
#define DST_ADDR_ERROR                             3
#define SRC_ADDR_NOT_MAPPED                        4
#define DST_ADDR_NOT_MAPPED                        5
#define COUNT_ERROR                                6
#define INVALID_SECTOR                             7
#define SECTOR_NOT_BLANK                           8
#define SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION    9
#define COMPARE_ERROR                              10
#define BUSY                                       11

/* 
 *  定义CCLK值大小，单位为KHz 
 */
#define IAP_FCCLK            (48000)                                    /* 此频率值必须同主频相同       */

#define EEPROM_STARTSEC            7                                    /* 定义作为EEPROM的Falsh        */
                                                                        /* 起始扇区和结束扇区           */
#define EEPROM_ENDSEC              7

#define EE_SUUCEESS                0                                    /* EEPROM写操作成功完成         */
#define IAP_ERROR                  1                                    /* 内部IAP函数调用出错          */
#define PARA_BANK_ERROR            2                                    /* 操作的板块错误               */
#define PARA_SRCADDR_ERROR         3                                    /* 写数据时的源地址错误(z字对齐)*/
#define PARA_OVERFLOW_ERROR        4                                    /* 写入溢出                     */

/*********************************************************************************************************
** 如bank设为1，则表示写EEPROM偏移量为256*1开始的256个字节
**   bank设为x，则表示写EEPROM偏移量为256*x开始的256个字节
**   注意：调用该函数将会使定义为EEPROM的Flash全部擦除，既会使前面写的所有数据丢失
*********************************************************************************************************/
extern uint8_t eepromWrite(uint8_t bank, uint8_t * src_addr);

/*********************************************************************************************************
** Function name:       eepromWriteNByte
** Descriptions:        调用该函数可以模拟写EEPROM操作，最大写数据256字节
** input parameters:    src_addr:存储数据的源地址
** output parameters:   状态信息
** Returned value:      无
**                      一次写入的数据为256字节
注意：调用该函数将会使定义为EEPROM的Flash全部擦除，即会使前面写的所有数据丢失
*********************************************************************************************************/
extern uint8_t eepromWriteNByte(uint8_t * src_addr, uint32_t eeprom_addr, uint32_t ulNum);

/*********************************************************************************************************
** src_addr表示偏移量
*********************************************************************************************************/
extern void eepromRead(uint32_t src_addr, uint8_t *dst_addr, uint32_t num);


#endif

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/



