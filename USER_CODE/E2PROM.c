/****************************************Copyright (c)****************************************************
**                            Guangzhou ZLGMCU Development Co., LTD
**
**                                 http://www.zlgmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           E2PROM.c
** Last modified Date:  2010-10-12
** Last Version:        V1.0
** Descriptions:        ����FlashIAP���Խ���ģ��Ƭ��EEPROM
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Lanwuqiang
** Created date:        2010-10-12
** Version:             V1.00
** Descriptions:        ����FlashIAP���Խ���ģ��Ƭ��EEPROM
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
 *  ���庯��ָ��  
 */
void (*IAP_Entry) (unsigned long ulParam_tab[], unsigned long ulPesult_tab[]) = (void(*)())IAP_ENTER_ADR;

unsigned long   GulParamin[8];                                           /*  IAP��ڲ���������           */
unsigned long   GulParamout[8];                                          /*  IAP���ڲ���������           */
/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP��������ѡ���������50
** input parameters:    ucSec1:           ��ʼ����
**                      ucSec2:           ��ֹ����
** output parameters:   GulParamout[0]:   IAP����״̬��,IAP����ֵ
** Returned value:      GulParamout[0]:   IAP����״̬��,IAP����ֵ
*********************************************************************************************************/
unsigned long  sectorPrepare (unsigned char  ucSec1, unsigned char  ucSec2)
{  
    GulParamin[0] = IAP_Prepare;                                        /*  ����������                  */
    GulParamin[1] = ucSec1;                                             /*  ���ò���                    */
    GulParamin[2] = ucSec2;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  ����IAP�������             */
   
    return (GulParamout[0]);                                            /*  ����״̬��                  */
}

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        ����RAM�����ݵ�FLASH���������51
** input parameters:    ulDst:             Ŀ���ַ����FLASH��ʼ��ַ����512�ֽ�Ϊ�ֽ�
**                      ulSrc:             Դ��ַ����RAM��ַ����ַ�����ֶ���
**                      ulNo:              �����ֽڸ�����Ϊ512/1024/4096/8192
** output parameters:   GulParamout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      GulParamout[0]:    IAP����״̬��,IAP����ֵ
*********************************************************************************************************/
unsigned long  ramCopy (unsigned long  ulDst, unsigned long  ulSrc, unsigned long  ulNo)
{  
    GulParamin[0] = IAP_RAMTOFLASH;                                     /*  ����������                  */
    GulParamin[1] = ulDst;                                              /*  ���ò���                    */
    GulParamin[2] = ulSrc;
    GulParamin[3] = ulNo;
    GulParamin[4] = IAP_FCCLK;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  ����IAP�������             */
    
    return (GulParamout[0]);                                            /*  ����״̬��                  */
}

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        �����������������52
** input parameters:    ucSec1             ��ʼ����
**                      ucSec2             ��ֹ����92
** output parameters:   GulParamout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      GulParamout[0]:    IAP����״̬��,IAP����ֵ
*********************************************************************************************************/
unsigned long  sectorErase (unsigned char ucSec1, unsigned char  ucSec2)
{  
    GulParamin[0] = IAP_ERASESECTOR;                                    /*  ����������                  */
    GulParamin[1] = ucSec1;                                             /*  ���ò���                    */
    GulParamin[2] = ucSec2;
    GulParamin[3] = IAP_FCCLK;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  ����IAP�������             */

    return (GulParamout[0]);                                            /*  ����״̬��                  */
}

/*********************************************************************************************************
** Function name:       blankChk
** Descriptions:        ������գ��������53
** input parameters:    ucSec1:              ��ʼ����
**                      ucSec2:              ��ֹ����92
** output parameters:   GulParamout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      GulParamout[0]:    IAP����״̬��,IAP����ֵ
*********************************************************************************************************/
unsigned long  blankChk (unsigned char  ucSec1, unsigned char  ucSec2)
{  
    GulParamin[0] = IAP_BLANKCHK;                                       /*  ����������                  */
    GulParamin[1] = ucSec1;                                             /*  ���ò���                    */
    GulParamin[2] = ucSec2;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  ����IAP�������             */

    return (GulParamout[0]);                                            /*  ����״̬��                  */
}

/*********************************************************************************************************
** Function name:       parIdRead
** Descriptions:        ��������ʶ�ţ��������54
** input parameters:    ��
** output parameters:   GulParamout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      GulParamout[0]:    IAP����״̬��,IAP����ֵ
*********************************************************************************************************/
unsigned long  parIdRead (void)
{
    GulParamin[0] = IAP_READPARTID;                                     /*  ����������                  */
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  ����IAP�������             */

    return (GulParamout[0]);                                            /*  ����״̬��                  */
}

/*********************************************************************************************************
** Function name:       codeIdBoot
** Descriptions:        ��Boot����汾�ţ��������55
** input parameters:    ��
** output parameters:   GulParamout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      GulParamout[0]:    IAP����״̬��,IAP����ֵ
*********************************************************************************************************/
unsigned long  codeIdBoot (void)
{
    GulParamin[0] = IAP_BOOTCODEID;                                     /*  ����������                  */
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  ����IAP�������             */

    return (GulParamout[0]);                                            /*  ����״̬��                  */
}

/*********************************************************************************************************
** Function name:       dataCompare
** Descriptions:        У�����ݣ��������56
** input parameters:    ulDst:             Ŀ���ַ����RAM/FLASH��ʼ��ַ����ַ�����ֶ���
**                      ulSrc:             Դ��ַ����FLASH/RAM��ַ����ַ�����ֶ���
**                      ulNo:              ���Ƚϵ��ֽ���������ֵӦ��Ϊ4�ı�����
** output parameters:   GulParamout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      GulParamout[0]:    IAP����״̬��,IAP����ֵ
*********************************************************************************************************/
unsigned long  dataCompare (unsigned long  ulDst, unsigned long  ulSrc, unsigned long  ulNo)
{  
    GulParamin[0] = IAP_COMPARE;                                        /*  ����������                  */
    GulParamin[1] = ulDst;                                              /*  ���ò���                    */
    GulParamin[2] = ulSrc;
    GulParamin[3] = ulNo;
    (*IAP_Entry)(GulParamin, GulParamout);                              /*  ����IAP�������             */

    return (GulParamout[0]);                                            /*  ����״̬��                  */
}

/********************************************************************************************************
** �û��ӿڲ�
********************************************************************************************************/
/*********************************************************************************************************
** Function name:       eepromWrite
** Descriptions:        ���øú�������ģ��дEEPROM���������д����256�ֽ�
** input parameters:    bank:��EEPROM�ռ俪ʼ�Ĵ洢��bank���,һ��bank 256�ֽ�
**                      src_addr:�洢���ݵ�Դ��ַ
** output parameters:   ״̬��Ϣ
** Returned value:      ��
**                      һ��д�������Ϊ256�ֽ�
ע�⣺���øú�������ʹ����ΪEEPROM��Flashȫ������������ʹǰ��д���������ݶ�ʧ
*********************************************************************************************************/
uint8_t eepromWrite(uint8_t bank, uint8_t * src_addr)
{
    uint8_t ucErr;
  
    zyIrqDisable();                                                     /*  ��IAP����ǰ����ر������ж� */
    
    if (bank > ((EEPROM_ENDSEC-EEPROM_STARTSEC+1) * 16)) {              /*  bank�������                */
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
                                                                        /*  �Ƚ�����                    */

    if (ucErr != CMD_SUCCESS) {                                         /*  IAP�������ó���             */
      zyIrqEnable();  
      return IAP_ERROR;
    }
  
    zyIrqEnable(); 
  
    return EE_SUUCEESS;
}

/*********************************************************************************************************
** Function name:       eepromWriteNByte
** Descriptions:        ���øú�������ģ��дEEPROM���������д����256�ֽ�
** input parameters:    src_addr:�洢���ݵ�Դ��ַ
** output parameters:   ״̬��Ϣ
** Returned value:      ��
**                      һ��д�������Ϊ256�ֽ�
ע�⣺���øú�������ʹ����ΪEEPROM��Flashȫ������������ʹǰ��д���������ݶ�ʧ
*********************************************************************************************************/
uint8_t eepromWriteNByte(uint8_t * src_addr, uint32_t eeprom_addr, uint32_t ulNum)
{
    uint8_t ucErr, startSec, endSec, ucSecNum;
    uint8_t *pIapData, *pIapDatatemp;
    uint32_t ulOffset, len;
    
    zyIrqDisable();                                                     /*  ��IAP����ǰ����ر������ж� */
    
  
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
    /* �޸�EEPROM���� */
    for (ulOffset = 0; ulOffset < ulNum; ulOffset++) {
        pIapData[eeprom_addr + ulOffset] =  *(src_addr + ulOffset);
    }
    /* д��EEPROM     */
    ucErr = sectorPrepare(startSec, endSec);
    ucErr = sectorErase(startSec, endSec);
    ucErr = blankChk(startSec, endSec);
  
    ucErr = sectorPrepare(startSec, endSec);
    ucErr = ramCopy(startSec << 12, (uint32_t)pIapData, len);
    ucErr = dataCompare(startSec << 12, (uint32_t)pIapData, len);
                                                                        /*  �Ƚ�����                    */

    if (ucErr != CMD_SUCCESS) {                                         /*  IAP�������ó���             */
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
** Descriptions:        ���øú������Զ���EEPROM_STARTSEC(EEPROM��ʼ������ʼ������)
** input parameters:    src_addr:����EEPROM�洢�ռ��ƫ������ַ
**                      dst_addr:���ն�ȡ���ݵ�Դ��ַ
**                      num:��ȡ���ֽ���
** output parameters:   ��
** Returned value:      ��
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
