/****************************************Copyright (c)****************************************************
**                            Guangzhou ZLGMCU Development Co., LTD
**
**                                 http://www.zlgmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           E2PROM.h
** Last modified Date:  2010-10-12
** Last Version:        V1.0
** Descriptions:        ����Flash����ģ��Ƭ��EEPROM
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Lanwuqiang
** Created date:        2010-10-12
** Version:             V1.00
** Descriptions:        ����Flash����ģ��Ƭ��EEPROM
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

#define IAP_ENTER_ADR   0x1FFF1FF1                                      /*  IAP��ڵ�ַ����             */

/*
 *  ����IAP������
 */
#define IAP_Prepare             50                                      /*  ѡ������                    */
                                                                        /*  ����ʼ�����š����������š�  */
#define IAP_RAMTOFLASH          51                                      /*  �������� FLASHĿ���ַ      */
                                                                        /*  RAMԴ��ַ    ����д���ֽ��� */
                                                                        /*  ϵͳʱ��Ƶ�ʡ�              */
#define IAP_ERASESECTOR         52                                      /*  ��������    ����ʼ������    */
                                                                        /*  ���������š�ϵͳʱ��Ƶ�ʡ�  */
#define IAP_BLANKCHK            53                                      /*  �������    ����ʼ�����š�  */
                                                                        /*  ���������š�                */
#define IAP_READPARTID          54                                      /*  ������ID    ���ޡ�          */
#define IAP_BOOTCODEID          55                                      /*  ��Boot�汾�š��ޡ�          */
#define IAP_COMPARE             56                                      /*  �Ƚ�����    ��Flash��ʼ��ַ */
                                                                        /*  RAM��ʼ��ַ����Ҫ�Ƚϵ�     */
                                                                        /*  �ֽ�����                    */
/*
 *  ����IAP����״̬��
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
 *  ����CCLKֵ��С����λΪKHz 
 */
#define IAP_FCCLK            (48000)                                    /* ��Ƶ��ֵ����ͬ��Ƶ��ͬ       */

#define EEPROM_STARTSEC            7                                    /* ������ΪEEPROM��Falsh        */
                                                                        /* ��ʼ�����ͽ�������           */
#define EEPROM_ENDSEC              7

#define EE_SUUCEESS                0                                    /* EEPROMд�����ɹ����         */
#define IAP_ERROR                  1                                    /* �ڲ�IAP�������ó���          */
#define PARA_BANK_ERROR            2                                    /* �����İ�����               */
#define PARA_SRCADDR_ERROR         3                                    /* д����ʱ��Դ��ַ����(z�ֶ���)*/
#define PARA_OVERFLOW_ERROR        4                                    /* д�����                     */

/*********************************************************************************************************
** ��bank��Ϊ1�����ʾдEEPROMƫ����Ϊ256*1��ʼ��256���ֽ�
**   bank��Ϊx�����ʾдEEPROMƫ����Ϊ256*x��ʼ��256���ֽ�
**   ע�⣺���øú�������ʹ����ΪEEPROM��Flashȫ���������Ȼ�ʹǰ��д���������ݶ�ʧ
*********************************************************************************************************/
extern uint8_t eepromWrite(uint8_t bank, uint8_t * src_addr);

/*********************************************************************************************************
** Function name:       eepromWriteNByte
** Descriptions:        ���øú�������ģ��дEEPROM���������д����256�ֽ�
** input parameters:    src_addr:�洢���ݵ�Դ��ַ
** output parameters:   ״̬��Ϣ
** Returned value:      ��
**                      һ��д�������Ϊ256�ֽ�
ע�⣺���øú�������ʹ����ΪEEPROM��Flashȫ������������ʹǰ��д���������ݶ�ʧ
*********************************************************************************************************/
extern uint8_t eepromWriteNByte(uint8_t * src_addr, uint32_t eeprom_addr, uint32_t ulNum);

/*********************************************************************************************************
** src_addr��ʾƫ����
*********************************************************************************************************/
extern void eepromRead(uint32_t src_addr, uint8_t *dst_addr, uint32_t num);


#endif

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/



