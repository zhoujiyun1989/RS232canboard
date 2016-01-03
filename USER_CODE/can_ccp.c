/*****************************************************************************
| Project Name: C C P - Driver adapter
|    File Name: CAN_CCP.C
|
|  Description: 
|   CCP driver customization module
|   This module is just an example how to adapt the CCP driver
|   It may be modified 
|       !!! Please contact Vector for Support !!!
|
|  Contains:
|   - Examples for indication, confirmation and pretransmit functions
|     for the vector CAN driver.
|   - Examples for FLASH programming, EEPROM programming, CalPage switching
|
|-----------------------------------------------------------------------------
|               C O P Y R I G H T
|-----------------------------------------------------------------------------
| Copyright (c)  2001-2002 by Vector Informatik GmbH.     All rights reserved.
|-----------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|-----------------------------------------------------------------------------
| Initials     Name                      Company
| --------     ---------------------     -------------------------------------
|    Ds        Sven Deckardt             Vector Informatik GmbH
|    Hp        Armin Happel              Vector Informatik GmbH
|    Za        Rainer Zaiser             Vector Informatik GmbH
|-----------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|-----------------------------------------------------------------------------
|  Date        Ver  Author  Description
| ----------  ----  ------  --------------------------------------------------
| 2002-04-17  1.00   Hp     Setting-up version and commonise the header with 
|                           CANbedded software modules.
|***************************************************************************/

#define CCP_CAN_INTERNAL


#include "can_inc.h"
#include "ccp.h"

#ifdef C_ENABLE_CCP

#define CCP_CAN_ADAPTER_VERSION  0x0100


/*----------------------------------------------------------------------------*/

#ifdef CCP_WRITE_EEPROM
  #include "eeprom.h"
#endif

/*----------------------------------------------------------------------------*/

#ifdef CCP_CALPAGE
  CCP_BYTE ccpCalPage = 0;
#endif


/******************************************************************************/
/* The following functions are the interface between CCP and the CAN driver   */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Indication function for rx message CRO */
void CCP_CRO_Indication(CanReceiveHandle rcvObject)
{
  rcvObject = rcvObject; /* Avoid compiler warning */

  /* CCP message received, data has been copied */
  /* Argument is pointer to copied data */

  /* Handle CCP commands on application level in ccpBackground */
  #ifdef CCP_CMD_NOT_IN_INTERRUPT

    ccp.SendStatus |= CCP_CMD_PENDING;

  // Handle CCP commands on CAN interrupt level
  #else

    /* Copy to a CCP_DWORD aligned location */
    /* This code may be removed, if CCP_RX_DATA_PTR is CCP_DWORD aligned */
    #ifdef C_CPUTYPE_32BIT
      
      {
        CCP_DWORD c[2];
        ((CCP_BYTEPTR)&c)[0] = CCP_RX_DATA_PTR[0];
        ((CCP_BYTEPTR)&c)[1] = CCP_RX_DATA_PTR[1];
        ((CCP_BYTEPTR)&c)[2] = CCP_RX_DATA_PTR[2];
        ((CCP_BYTEPTR)&c)[3] = CCP_RX_DATA_PTR[3];
        ((CCP_BYTEPTR)&c)[4] = CCP_RX_DATA_PTR[4];
        ((CCP_BYTEPTR)&c)[5] = CCP_RX_DATA_PTR[5];
        ((CCP_BYTEPTR)&c)[6] = CCP_RX_DATA_PTR[6];
        ((CCP_BYTEPTR)&c)[7] = CCP_RX_DATA_PTR[7];
        ccpCommand((CCP_BYTEPTR)&c);
      }
    #else

      ccpCommand(CCP_RX_DATA_PTR);

    #endif

  #endif
}

/*----------------------------------------------------------------------------*/
/* Confirmation function of tx message CCP_DTO is configured in generation tool */
void CCP_DTO_Confirmation(CanTransmitHandle tmtObject)
{
  tmtObject = tmtObject; /* Avoid compiler warning */

  ccpSendCallBack();
}

/*----------------------------------------------------------------------------*/
/* Transmit the CCP message */
/* Id is CCP_DTO_ID, which is configured at compile time */
void ccpSend( CCP_BYTEPTR msg )
{
  CCP_TX_DATA_PTR[0] = msg[0];
  CCP_TX_DATA_PTR[1] = msg[1];
  CCP_TX_DATA_PTR[2] = msg[2];
  CCP_TX_DATA_PTR[3] = msg[3];
  CCP_TX_DATA_PTR[4] = msg[4];
  CCP_TX_DATA_PTR[5] = msg[5];
  CCP_TX_DATA_PTR[6] = msg[6];
  CCP_TX_DATA_PTR[7] = msg[7];

  #ifdef C_ENABLE_TRANSMIT_QUEUE

    if (CanTransmit(CCP_TX_HANDLE)!=kCanTxOk) {
      /* Fatal Error, should never fail */
      ccpInit();
    }

  #else

    #ifndef C_DISABLE_TRANSMIT_QUEUE
      #error CAN driver transmit queue must be enabled or disabled
    #endif

    if (CanTransmit(CCP_TX_HANDLE)!=kCanTxOk) {

      /* Set transmission request flag */
      ccp.SendStatus |= CCP_TX_PENDING;
    }

  #endif

}

/*----------------------------------------------------------------------------*/
/* Perform backgound calculation if needed */

void ccpUserBackground( void ) {

  /* Try to retransmit if CAN driver transmit queue is not enabled */
  #ifdef C_DISABLE_TRANSMIT_QUEUE

    if( ccp.SendStatus & CCP_TX_PENDING) {
      if (CanTransmit(CCP_TX_HANDLE)==kCanTxOk) {
        ccp.SendStatus &= ~CCP_TX_PENDING;
      }
    }

  #endif

  /* Check if a pending EEPROM write access is finished */
  #ifdef CCP_WRITE_EEPROM

    void ccpCheckPendingEEPROM( void );
    ccpCheckPendingEEPROM();

  #endif

  /* ... */
  /* Insert any other user actions here */
  /* Call ccpSendCrm() to finish pending EEPROM or FLASH cycles */

}

/******************************************************************************/
/* The following functions must be individually implemented if required       */
/* There are samples available for C16x, HC12, SH7055                              */
/******************************************************************************/



/*
ccpGetDaqPointer

Convert a memory address from CCP 8/32bit into a address
stored in the ODT entries for DAQ.

This is for reducing memory space used for DAQ lists.
For example on a 32 bit microcontroller, a DAQ pointer may be stored as 16 bit
value. DAQ will add the base address CCP_DAQ_BASE_ADDR before dereferencing the
pointer. This will limit data acquisition to a single 64K memory range, but it
will save 50% memory used for DAQ lists.

Note: It must be possible to calculate the final address for DAQ like
 value = * (CCP_DAQ_BASE_ADDR + addr);
*/

#ifdef CCP_DAQ_BASE_ADDR
CCP_DAQBYTEPTR ccpGetDaqPointer( CCP_BYTE addr_ext, CCP_DWORD addr )
{
  return (CCP_DAQBYTEPTR)( ccpGetPointer(addr_ext,addr) - CCP_DAQ_BASE_ADDR );
}
#endif



/*----------------------------------------------------------------------------*/
/* Check addresses for valid write access */
/* Used only if Write Protection is required */
/* Returns false if access denied */
#ifdef CCP_WRITE_PROTECTION
CCP_BYTE ccpCheckWriteAccess( CCP_MTABYTEPTR a, CCP_BYTE s )
{
  // Protect CCP
  if (a+s>=(CCP_MTABYTEPTR)&ccp && a<(CCP_MTABYTEPTR)&ccp+sizeof(ccp)) {
    return 0;
  }

  return 1;
}
#endif


/*----------------------------------------------------------------------------*/
/* Flash Kernel Download */
/* Used only of Download of the Flash Kernel is required */
#ifdef CCP_BOOTLOADER_DOWNLOAD

CCP_BYTE ccpDisableNormalOperation( CCP_MTABYTEPTR a, CCP_WORD s ) {

  a = a; /* Avoid compiler warning */
  s = s;

  /* CANape attempts to download the flash kernel to CCP_RAM */
  /* Address is a */
  /* Size is s bytes */

  /* ... */
  //disable_interrupt();                  // disables all interrupts, e.g. TIMER_0
	zyIrqDisable();							//在下载之前，禁止所有中断
  // return 0; // Refused
  return 1; // Ok
}


#endif

/*----------------------------------------------------------------------------*/
/* Example: Flash Programming */
/* Used only if integrated Flash Programming is required */

#ifdef CCP_PROGRAM

#include "flash.h"

void ccpFlashClear( CCP_MTABYTEPTR a, CCP_DWORD size )
{
	unsigned char ucErr;
  #if defined(CANBOX) || defined(PHYTEC_MM167)
    #ifdef CCP_CALPAGE
      if (a>=(CCP_MTABYTEPTR)0x40000) a -= 0x30000; /* Compensate CCP_RAM/CCP_ROM mapping */
    #endif
  #endif

  #ifndef CCP_BOOTLOADER
    CCP_DISABLE_INTERRUPT;
    flashEraseBlock(a);
    CCP_ENABLE_INTERRUPT;
  #endif
  
	ucErr = sectorPrepare(7, 7);
	ucErr = sectorErase(7, 7);
	ucErr = blankChk(7, 7);
	
}

#ifndef CCP_BOOTLOADER
CCP_BYTE ccpFlashProgramm( CCP_BYTEPTR data, CCP_MTABYTEPTR a, CCP_BYTE size )
{
  #if defined(CANBOX) || defined(PHYTEC_MM167)
    #ifdef CCP_CALPAGE
      if (a>=(CCP_MTABYTEPTR)0x40000) a -= 0x30000; /* Compensate CCP_RAM/CCP_ROM mapping */
    #endif
  #endif

  if (size==0) { /* End of programing sequence */

    /* Software Reset */
    #ifdef C_COMP_TASKING_C16X
      #pragma asm
        srst
      #pragma endasm
    #endif
  }

  while (size>0) {
    CCP_DISABLE_INTERRUPT;
    flashByteWrite(a,*data);
    CCP_ENABLE_INTERRUPT;
    data++;
    a++;
    size--;
  }

  return CCP_WRITE_OK;
}

#endif
#endif

/*----------------------------------------------------------------------------*/
/* Example: Calibration CCP_RAM/CCP_ROM Selection */
/* Used if Flash Programming is required */

#if defined(CCP_CALPAGE) || defined(CCP_PROGRAM)

CCP_DWORD ccpGetCalPage( void )
{
  return (CCP_DWORD)ccpCalPage;
}

void ccpSetCalPage( CCP_DWORD a )
{
  ccpCalPage = (CCP_BYTE)a;

  #if defined(CANBOX) || defined(PHYTEC_MM167)
    if (ccpCalPage==1) { /* CCP_RAM */
      #pragma asm
        mov DPP1,#11h
      #pragma endasm
    } else {             /* CCP_ROM */
      #pragma asm
        mov DPP1,#05h
      #pragma endasm
    }
  #endif

}

void ccpInitCalPage( void ) {

  #if defined(CANBOX) || defined(PHYTEC_MM167)
    #define CALROM_ADDR 0x14000
    #define CALRAM_ADDR 0x44000
    huge unsigned char *p1 = (huge unsigned char *)CALROM_ADDR;
    huge unsigned char *p2 = (huge unsigned char *)CALRAM_ADDR;
    unsigned int i;
    for (i=0;i<0x4000;i++) {
      *p2++ = *p1++;
    }
  #endif
}

#endif

/*----------------------------------------------------------------------------*/
/* Example: Seed&Key*/
/* Used only if Seed&Key is required */

#ifdef CCP_SEED_KEY

CCP_BYTE ccpResourceMask = 0;
CCP_DWORD ccpLastSeed = 0;

CCP_DWORD ccpGetSeed( CCP_BYTE resourceMask )
{
  ccpResourceMask = resourceMask;

  /* Generate a seed */

  /* Example: */
  /* Optimum would be a number which never appears twice */
 #ifdef CCP_TIMESTAMPING
   ccpLastSeed = ccpGetTimestamp() * ccpGetTimestamp();
   return ccpLastSeed;
 #endif

  return 0;
}

CCP_BYTE ccpUnlock( CCP_BYTE *key )
{
  /* Check the key */

  /* Example: */
  ccpLastSeed = (ccpLastSeed>>5) | (ccpLastSeed<<23);
  ccpLastSeed *= 7;
  ccpLastSeed ^= 0x26031961;
  if ( *(CCP_DWORD*)key != ccpLastSeed ) {
    return 0;
  }

  /* Reset resource protection bit */
  return ccpResourceMask;
}

#endif


/*----------------------------------------------------------------------------*/
/* Example: EEPROM write access */
/* Used only if required */

#ifdef CCP_WRITE_EEPROM

/* Pending EEPROM write cycle */
CCP_BYTE ccpEEPROMState = 0;

/* EEPROM write */
/* Return values for ccpCheckWriteEEPROM:
   CCP_WRITE_OK      - EEPROM written
   CCP_WRITE_DENIED  - This address is not in EEPROM
   CCP_WRITE_PENDING - EEPROM write in progress, call ccpSendCrm() when done
   CCP_WRITE_ERROR   - EEPROM write failed
*/
CCP_BYTE ccpCheckWriteEEPROM( CCP_MTABYTEPTR addr, CCP_BYTE size, CCP_BYTEPTR data )
{
  /* Check address for EEPROM */
  if (addr<EEPROM_START || addr>EEPROM_END) {

    /* Not EEPROM */
    /* Let the CCP driver perform a standard CCP_RAM write access */
    return CCP_WRITE_DENIED;
  }

  /* Alternative 1: */
  /* Initiate EEPROM write */
  /* When finished, call ccpSendCrm() */
  #ifdef C_CLIENT_BMWAG

    #if EEBUFFER<5
      #error CCP will need at least 5 Entries in EEBuffer
    #endif
    if (EECheckBuffer()>EEBUFFER_MAX-size) return CCP_WRITE_ERROR;
    while (size--) {
      if (E_OK!=EEAddByte((unsigned int) addr++,*data++)) return CCP_WRITE_ERROR;
    }

    ccpEEPROMState = CCP_WRITE_PENDING;
    return CCP_WRITE_PENDING;

  /* Alternative 2: */
  /* Write to EEPROM here and wait until finished */
  #else

    eeWrite(addr,data,size);
    return CCP_WRITE_OK;

  #endif


}

/* Check for EEPROM write finished */
void ccpCheckPendingEEPROM( void ) {

  #ifdef C_CLIENT_BMWAG

    if (ccpEEPROMState==CCP_WRITE_PENDING) {
      if (EECheckBuffer()==0) {
        ccpSendCrm();
        ccpEEPROMState = 0;
      }
    }

  #endif
}

#endif


#ifdef CCP_READ_EEPROM

/* EEPROM read */
/* Return values for ccpCheckReadEEPROM:
   0 (FALSE)    - This address is not in EEPROM
   1 (TRUE)     - EEPROM read
*/
CCP_BYTE ccpCheckReadEEPROM( CCP_MTABYTEPTR addr, CCP_BYTE size, CCP_BYTEPTR data )
{

  /* Read EEPROM */
  #ifdef C_CLIENT_BMWAG

    /* Check address for EEPROM */
    if (addr<EEPROM_START || addr>EEPROM_END) {

      /* Not EEPROM */
      /* Let the CCP driver perform a standard CCP_RAM read access */
      return 0;
    }

    while (size-->0) {
      *data++ = EEReadByte(addr++);
    }

    return 1;

  #else

    return 0;

  #endif

}

#endif


#endif /* C_ENABLE_CCP */
