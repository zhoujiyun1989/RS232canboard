/********************************************************************************************************
--Design name:          main.c
--Target devices:       TinyM0-CAN-T
--Create data:          2015-4-18
--Revision:             v1.0
--Copyright(c):         AngDe
--Additional comments:  
*********************************************************************************************************/
#include "LPC11xx.h"
#include "CAN_API.h"
#include "buffer.h"
#include "E2PROM.h"
#include "data.h"	

/*********************************************************************************************************
  flash��д������CAN����
*********************************************************************************************************/
//0-3����ΪCAN�Ĳ�����

/*********************************************************************************************************
  �궨��
*********************************************************************************************************/

//define Transparent  ͸������
//define Identification   ����ʶ����
//define Modbus   modbusЭ�鴫��
//���ڵ�CAN���ᴫ��ID��dlc
//����ȥ��IDʵ��ֻ��29λ,��30λΪ1ʱΪ��չ֡Ϊ0ʱΪ��׼֡����31λΪ1ʱΪԶ��֡Ϊ0ʱΪ����֡
//CAN�����ڣ��ᴫ��ID��dlc

#define UART_BPS       					115200                                        /* ����ͨ�Ų�����               */
#define CAN_SPEED						250
#define RID 							0x00000000
#define	RID_STD_EXT						1																							//0ʱΪ��׼֡,1ʱΪ��չ֡
#define	RID_DAT_RTR						0																							//0ʱΪ����֡,1ʱΪԶ��֡
#define RID_CHECK						(RID&0x1fffffff)|(RID_STD_EXT<<29)|(RID_DAT_RTR<<30)
#define RMASK 							0x00																		//����0x00-0xFF��ID֡
struct uartframe {
	uint8_t start_symbol;
	uint8_t fixation[2];
	uint8_t protocol[2];
	uint8_t leng[2];
	uint8_t cmd;
	uint8_t data[13];
	uint8_t check[2];
	uint8_t end_symbol;
};
enum State {
	findhear = 0,
	cmdhandle,
	lenhandle,
	dataread,
	checkhandle
}enum_UartHandleState;

/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
int timer_ms_cont = 0;
volatile   		uint8_t          GucRcvNew;                                  /* ���ڽ��������ݵı�־         */
uint8_t      	GucRcvBuf[9] ;                                            /* ���ڽ������ݻ�����           */
uint32_t     	GulNum;                                                    /* ���ڽ������ݵĸ���           */
struct uartframe uartframesend;
struct uartframe uartframereceive;
uint8_t	receivedata[13];
uint8_t success_config[24] = {0x7E,0xFF,0x03,0xA0,0x21,0x00,0x0E,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x71,0xD5,0x7E}
uint8_t fail_config[24] = {0x7E,0xFF,0x03,0xA0,0x21,0x00,0x0E,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0xD4,0x7E}

/*********************************************************************************************************
** Function name:       delay_ms
** Descriptions:        �����ʱʱ��Ϊ1ms
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void delay_ms(INT32U ulTime)
{
    INT32U i;
    i = 0;
    while (ulTime--) {
      for (i = 0; i < 5000; i++);
    }
}

/*********************************************************************************************************
* Function Name:        TIMER16_0_IRQHandler
* Description:          TIMER16_0 �жϴ�����
* Input:                ��
* Output:               ��
* Return:               ��
*********************************************************************************************************/
void TIMER16_0_IRQHandler (void)
{
    LPC_TMR16B0->IR = 0x01;                                             /* ����жϱ�־                 */
    if (timer_ms_cont++ == 1000) {
				timer_ms_cont = 0;
				if(LPC_GPIO2->DATA & (1ul << 7))
					LPC_GPIO2->DATA &= ~(1ul << 7);          //�õ�
				else
					LPC_GPIO2->DATA |= (1ul << 7);            //�ø�
    } 
}

/*********************************************************************************************************
** Function name:       timer0Init
** Descriptions:        16λ��ʱ��0��ʼ������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void timer0Init (void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);                              /* �򿪶�ʱ��ģ��               */

    LPC_TMR16B0->IR      = 1;
    LPC_TMR16B0->PR      = 100;                                        /* ���÷�Ƶϵ��                 */
    LPC_TMR16B0->MCR     = 3;                                           /* ����MR0ƥ���λTC�������ж�*/
    //LPC_TMR16B0->MR0     = SystemFrequency/1000;                        /* �����ж�ʱ��1S               */
	LPC_TMR16B0->MR0     = SystemFrequency/100000;                        /* �����ж�ʱ��1mS               */
                                                                        /* 16λ��ʱ���������ֵ65535    */
    LPC_TMR16B0->TCR     = 0x01;                                        /* ������ʱ��                   */
    NVIC_EnableIRQ(TIMER_16_0_IRQn);                                    /* �����жϲ�ʹ��               */
    NVIC_SetPriority(TIMER_16_0_IRQn, 2);
}

/*********************************************************************************************************
** Function name:       __nop
** Descriptions:        ִ�п�ָ���C�����У����������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
__asm void nop(void)
{
    NOP
}
/*********************************************************************************************************
** Function name:       GPIOInit
** Descriptions:        ��ʼ��P1_10Ϊ����,������ģ���������ţ��͵�ƽ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void GPIOInit(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);                          // ʹ��GPIO����ʱ��
		LPC_IOCON->PIO2_7  &= ~0x07;
    LPC_GPIO2->DIR |= (1ul << 7);  

}

/*********************************************************************************************************
** Function name:       CANInit
** Descriptions:        ��ʼ��CAN����
**											ժ��    LPC111x/LPC11Cxx User manual
**											C_CAN controller
**											The C_CAN clock control bit 17 in the SYSAHBCLKCTRL register (Table 21) and the
**											C_CAN reset control bit 3 in the PRESETCTRL register (Table 9) are only functional for
**											parts LPC11Cxx/101/201/301.
**											 
**											The SYSAHBCLKCTRL register gates the system clock to the various peripherals and
**											memories. UART, the WDT, and SPI0/1 have individual clock dividers to derive peripheral
**											clocks from the main clock. 
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void CANInit(void)
{

    LPC_SYSCON->PRESETCTRL    |= (1 << 3);                            // ���CAN���踴λ
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 17);                           //��ʼ��SystemInit�У��Ѿ�ʹ�ܣ�ʹ��CAN����
		
	if(CAN_SPEED == 250)
    	(*rom)->pCANAPI->init_can(&CanApiClkInitTable_250[0]);                //����CAN�Ĳ�����Ϊ250k
	else if(CAN_SPEED == 500)
		(*rom)->pCANAPI->init_can(&CanApiClkInitTable_500[0]);                //����CAN�Ĳ�����Ϊ500k	

    /*����CAN�ж�*/
    NVIC_EnableIRQ(CAN_IRQn);                                         //ʹ��CAN���ж�
    NVIC_SetPriority(CAN_IRQn, 1);                                    //����CAN�����ȼ�Ϊ1
    (*rom)->pCANAPI->config_calb((CAN_CALLBACKS *)&callbacks);        //����CAN�Ļص�����  

	/*���ý���֡*/
	msg_cof0.msgobj = 0;
	msg_cof0.mask = RMASK;                                        //���ý��յ�������
	msg_cof0.mode_id = (RID&0x1fffffff)|(0<<29)|(0<<30); //���ý��յ�ID
	(*rom)->pCANAPI-> config_rxmsgobj(&msg_cof0);                      //���ý��ն���

	msg_cof1.msgobj = 1;
	msg_cof1.mask = RMASK;                                        //���ý��յ�������
	msg_cof1.mode_id = (RID&0x1fffffff)|(1<<29)|(0<<30); //���ý��յ�ID
	(*rom)->pCANAPI-> config_rxmsgobj(&msg_cof1);                      //���ý��ն���

	msg_cof2.msgobj = 2;
	msg_cof2.mask = RMASK;                                        //���ý��յ�������
	msg_cof2.mode_id = (RID&0x1fffffff)|(0<<29)|(1<<30); //���ý��յ�ID
	(*rom)->pCANAPI-> config_rxmsgobj(&msg_cof2);                      //���ý��ն���

	msg_cof3.msgobj = 3;
	msg_cof3.mask = RMASK;                                        //���ý��յ�������
	msg_cof3.mode_id = (RID&0x1fffffff)|(1<<29)|(1<<30); //���ý��յ�ID
	(*rom)->pCANAPI-> config_rxmsgobj(&msg_cof3);                      //���ý��ն���

	
}

/*********************************************************************************************************
** Function name:        uartInit
** Descriptions:        ���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ�飬������Ϊ115200
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uartInit (void)
{
    uint16_t usFdiv;
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);                             /* ʹ��IOCONʱ��                */
    LPC_IOCON->PIO1_6 |= 0x01;                                          /* ��P1.6 1.7����ΪRXD��TXD     */
    LPC_IOCON->PIO1_7 |= 0x01;

    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);                               /* ��UART���ܲ���ʱ��         */
    LPC_SYSCON->UARTCLKDIV       = 0x01;                                /* UARTʱ�ӷ�Ƶ                 */

    LPC_UART->LCR  = 0x83;                                              /* �������ò�����               */
    usFdiv = (SystemFrequency/LPC_SYSCON->UARTCLKDIV/16)/UART_BPS;      /* ���ò�����                   */
    LPC_UART->DLM  = usFdiv / 256;
    LPC_UART->DLL  = usFdiv % 256;
    LPC_UART->LCR  = 0x03;                                              /* ����������                   */
    LPC_UART->FCR  = 0x87;                                              /* ʹ��FIFO������8���ֽڴ�����  */
    
    NVIC_EnableIRQ(UART_IRQn);                                          /* ʹ��UART�жϣ����������ȼ�   */
    NVIC_SetPriority(UART_IRQn, 2);

    LPC_UART->IER  = 0x01;                                              /* ʹ�ܽ����ж�                 */
}

/*********************************************************************************************************
** Function name:       uartSendByte
** Descriptions:        �򴮿ڷ����ӽ����ݣ����ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
** input parameters:    ucDat:   Ҫ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uartSendByte (uint8_t ucDat)
{
    LPC_UART->THR = ucDat;                                              /*  д������                    */
    while ((LPC_UART->LSR & 0x40) == 0);                                /*  �ȴ����ݷ������            */
}

/*********************************************************************************************************
** Function name:       uartSendStr
** Descriptions:        �򴮿ڷ����ַ���
** input parameters:    pucStr:  Ҫ���͵��ַ���ָ��
**                      ulNum:   Ҫ���͵����ݸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uartSendStr (uint8_t const *pucStr, uint32_t ulNum)
{
    uint32_t i;

    for (i = 0; i < ulNum; i++){                                        /* ����ָ�����ֽ�����           */
        uartSendByte (*pucStr++);
    }
}
/*********************************************************************************************************
* Function Name:        UART_IRQHandler
* Description:          UART�жϷ�����
* Input:                ��
* Output:               ��
* Return:               ��
*********************************************************************************************************/
void UART_IRQHandler (void)
{
    GulNum = 0;

    while ((LPC_UART->IIR & 0x01) == 0){                                /*  �ж��Ƿ����жϹ���          */
        switch (LPC_UART->IIR & 0x0E){                                  /*  �ж��жϱ�־                */
        
            case 0x04:                                                  /*  ���������ж�                */
                GucRcvNew = 1;                                          /*  �ý��������ݱ�־            */
				GucRcvBuf[0] = 8;
                for (GulNum = 1; GulNum < 9; GulNum++){                 /*  ��������8���ֽ�             */
					GucRcvBuf[GulNum] = LPC_UART->RBR;
					put_byte_buffer(&rcv_byte_buffer, &GucRcvBuf[GulNum]);
                }
                break;
            
            case 0x0C:                                                  /*  �ַ���ʱ�ж�                */
                GucRcvNew = 1;
				GulNum = 1; 
                while ((LPC_UART->LSR & 0x01) == 0x01){                 /*  �ж������Ƿ�������        */
                    GucRcvBuf[GulNum] = LPC_UART->RBR;
					put_byte_buffer(&rcv_byte_buffer, &GucRcvBuf[GulNum]);
                    GulNum++;
                }
                break;

            default:
                break;

        }
    }
		
}

int senduartframe(struct uartframe uartframesendvar)
{
	uint8_t* p_temp = (uint8_t*)uartframesendvar;
	int i = 0;
	uartSendByte(0x7E);
	//uartframe�ṹ���ܳ�����24��ȥ����β��7E
	for(i=1;i<23;i++)
	{
		if((*(p_temp+i)>=0x00)&&(*(p_temp+i)<=0x20))
		{
			uartSendByte(0x7D);
			uartSendByte(*(p_temp+i));
		}
		else
			switch(*(p_temp+i))
			{
				case (0x7E):
				{
					uartSendByte(0x7D);
					uartSendByte(0x5E);
					break;
				}
				case (0x7D):
				{
					uartSendByte(0x7D);
					uartSendByte(0x5D);
					break;
				}
				default:
				{
					uartSendByte(*(p_temp+i));
					break;
				}
			}
	}
	uartSendByte(0x7E);
}

int receiveuartframe(uint8_t uartreceive, struct uartframe * puartframereceive, uint8_t valid_7E, uint8_t convert_7D)
{
	int i = 0;
	int static j = 1;
	uint8_t check[2] = 0;
	
	uint32_t id = 0;
	uint32_t can_speed = 0;
	uint8_t  ucErr = 0;
	uint8_t  data[256],temp[256];
	
	uint8_t *p_temp = (uint8_t*)puartframereceive;
	
	switch(uartreceive)
	{
		case (7E):
		{
			if((j==23)&&(p_temp[0]==0x7E)&&(p_temp[1]==0xFF)&&(p_temp[2]==0x03)&&(p_temp[3]==0xA0)&&(p_temp[4]==0x21)&&(p_temp[5]==0x00)&&(p_temp[6]==0x0D))
			{
				for(i=1,i<21,i++)
				{
					check[0] ^= p_temp[i];
					check[1] += p_temp[i];
				}
				if((check[0]==p_temp[21])&&(check[1]==p_temp[22]))
				{
					//У����ȷ���ҷ���Э�飬��ʼ��������
					switch(p_temp[7])
					{
						case (1):
						{
							id = 0;
							for(i=0;i<4;i++)
							{
								id = (id<<8) | p_temp[8+i];
							}
							//���͵�ID�Ƿ����֡��Ϣ
							//id = (id&0x1FFFFFFF)|(TID_STD_EXT<<29)|(TID_DAT_RTR<<30);
							//msg_obj.msgobj = 2;
							msg_obj.mode_id = id;
							msg_obj.dlc = p_temp[12];
							for(i=0;i<msg_obj.dlc;i++)
							{
								msg_obj.data[i] = p_temp[13+i];
							}
							(*rom)->pCANAPI->can_transmit(&msg_obj);
						}
						case (2):
						{
							for(i=0;i<4;i++)
							{
								can_speed = (can_speed<<8) | p_temp[8+i];
								data[i] = p_temp[8+i];
								temp[i] = 0;
							}
							for(i=4;i<256;i++)
							{
								data[i] = 0;
								temp[i] = 0;
							}
							
							ucErr = eepromWrite(0, data);
							eepromRead(0, temp, 256);
							if (ucErr == 0)
							{
								for (i = 0; i < 256; i++)
								{
									if (data[i] != temp[i])
								    {
										ucErr = 1;
										//uartSendStr("abcde",3)
								   	}
								}
							}
							if(ucErr == 0)
							{
								uartSendByte(0x7E);
								//uartframe�ṹ���ܳ�����24��ȥ����β��7E
								for(i=1;i<23;i++)
								{
									if((*(success_config+i)>=0x00)&&(*(success_config+i)<=0x20))
									{
										uartSendByte(0x7D);
										uartSendByte(*(success_config+i));
									}
									else
										switch(*(success_config+i))
										{
											case (0x7E):
											{
												uartSendByte(0x7D);
												uartSendByte(0x5E);
												break;
											}
											case (0x7D):
											{
												uartSendByte(0x7D);
												uartSendByte(0x5D);
												break;
											}
											default:
											{
												uartSendByte(*(success_config+i));
												break;
											}
										}
								}
								uartSendByte(0x7E);
							}
							else
							{
								uartSendByte(0x7E);
								//uartframe�ṹ���ܳ�����24��ȥ����β��7E
								for(i=1;i<23;i++)
								{
									if((*(fail_config+i)>=0x00)&&(*(fail_config+i)<=0x20))
									{
										uartSendByte(0x7D);
										uartSendByte(*(fail_config+i));
									}
									else
										switch(*(fail_config+i))
										{
											case (0x7E):
											{
												uartSendByte(0x7D);
												uartSendByte(0x5E);
												break;
											}
											case (0x7D):
											{
												uartSendByte(0x7D);
												uartSendByte(0x5D);
												break;
											}
											default:
											{
												uartSendByte(*(fail_config+i));
												break;
											}
										}
								}
								uartSendByte(0x7E);
							}
							break;
						}
						default:
						{
							break;
						}
					}
				}
			}
			valid_7E = 1;
			j = 1;
			*p_temp = 0x7E;
			for(i=1;i<24;i++)
			{
				*(p_temp+i) = 0x00;
			}
			break;
		}
		
		case (7D):
		{
			if(valid_7E)
				convert_7D = 0x01;
			else
				convert_7D = 0x00;
		}

		default:
		{
			if(valid_7E)
			{
				if(convert_7D)
				{
					convert_7D = 0;
					if(uartreceive>=0 && uartreceive<=0x20)
						*(p_temp+(j++)) = uartreceive;
					else
						switch(uartreceive)
						{
							case (0x5E):
							{
								*(p_temp+(j++)) = 0x7E;
								break;
							}
							case (0x5D):
							{
								*(p_temp+(j++)) = 0x7D;
								break;
							}
							default:
							{
								valid_7E = 0;
								break;
							}
						}
				}
				else
				{
					convert_7D = 0;
					if((uartreceive>=0x21&&uartreceive<=0x7C)||(uartreceive>=0x7F&&uartreceive<=0xFF))
						*(p_temp+(j++)) = uartreceive;
					else
						valid_7E = 0;
				}
			}
			else
			{
				j = 1;
				convert_7D = 0;
			}
		}
	}
	if(j>=24)
	{
		valid_7E = 0;
		j = 1;
	}
}


/*********************************************************************************************************
** Function name:       main
** Descriptions:        CAN�����շ�,ֱ�ӵ���LPC11C14Ƭ��CAN �̼�API������Ĭ�ϲ�����Ϊ500kbps��
**                      
**                      
**                      
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
int main(void)
{
	int i= 0;
	uint32_t id = 0, msg_id = 0;
	uint8_t	byte_send;
	uint8_t	valid_7E = 0;
	uint8_t	convert_7D = 0;
	uint8_t	senddata[13];
	
	SystemInit();                                                     //��ʼ��Ŀ��壬����ɾ��
    GPIOInit();                                                       //��ʼ��P1_10Ϊ����
	timer0Init();
	
	uartInit();
	CANInit();

	ccpInit();
	
	/*while (1)
	{
		delay_ms(500);
		(LPC_GPIO2->DATA |= (1ul << 7));			  //�ø�
		delay_ms(500);
		(LPC_GPIO2->DATA &= ~(1ul << 7)); 		 //�õ�
	}*/

	while (1) 
	{

//CCP���
		//�յ�CCP����֡
		if(ccp_flag)
		{
			ccpCommand(receive_buffer);
			ccpBackground();
			ccp_flag = 0;
		}
		ccpSendCallBack();

		
		if(get_can_buffer(&rcv_can_buffer,&msg_obj))
		{
			uartframesend.start_symbol = 0x7E;
			uartframesend.fixation = {0xFF,0x03};
			uartframesend.protocol = {0xA0,0x21};
			uartframesend.leng = {0x00,0x0D};
			uartframesend.cmd = 0x01;
			//uartframesend.data = {};
			//uartframesend.check = {};
			uartframesend.end_symbol = 0x7E;

			
			//msg_id = (msg_obj.mode_id&0x1fffffff)|(RID_STD_EXT<<29)|(RID_DAT_RTR<<30);
			msg_id = (msg_obj.mode_id&0x1fffffff)|((msg_obj.msgobj)<<29);
			for(i=0;i<4;i++)
			{
				senddata[i] = (char)(msg_id >> 8*(3-i));
			}
			senddata[4] = msg_obj.dlc;
			
			for(i=0;i<msg_obj.dlc;i++)
			{
				senddata[5+i] = msg_obj.data[i];
			}
			//��Ӻ�8λ
			if(msg_obj.dlc<8)
			{
				for(i=0;i<=8-msg_obj.dlc;i++)
				{
					senddata[5+msg_obj.dlc+i] = i;
				}
			}
			uartframesend.data = &senddata[0];

			uartframesend.check[0] = 0xFF^0x03^0xA0^0x21^0x00^0x0D^0x01;
			uartframesend.check[1] = 0xFF+0x03+0xA0+0x21+0x00+0x0D+0x01;
			for(i=0;i<13;i++)
			{
				uartframesend.check[0] ^= uartframesend.data[i];
				uartframesend.check[1] += uartframesend.data[i];
			}
			
			senduartframe(uartframesend);
		}
/*
		if(((rcv_byte_buffer.next+rcv_byte_buffer.size-rcv_byte_buffer.oldest)%rcv_byte_buffer.size) >= 13)
		{
			id = 0;
			timer_ms_cont = 0;
			for(i=0;i<4;i++)
			{
				get_byte_buffer(&rcv_byte_buffer,&byte_send);
				id = (id<<8) | (uint32_t)byte_send;
			}
			//���͵�ID�Ƿ����֡��Ϣ
			//id = (id&0x1FFFFFFF)|(TID_STD_EXT<<29)|(TID_DAT_RTR<<30);
			msg_obj.msgobj = 2;
			msg_obj.mode_id = id;
			get_byte_buffer(&rcv_byte_buffer,&byte_send);
			msg_obj.dlc = byte_send;
			for(i=0;i<8;i++)
			{
				get_byte_buffer(&rcv_byte_buffer,&byte_send);
				msg_obj.data[i] = byte_send;
			}
			(*rom)->pCANAPI->can_transmit(&msg_obj);
		}
		else if(timer_ms_cont > 500)
		{
			//���buffer
			rcv_byte_buffer.oldest = rcv_byte_buffer.next;
			timer_ms_cont = 0;
		}
*/
		if(is_byte_buffer_empty(&rcv_byte_buffer)==0)
		{
			get_byte_buffer(&rcv_byte_buffer,&byte_send);
			//uartSendByte(byte_send);
			receiveuartframe(byte_send, &uartframereceive, valid_7E, convert_7D);
		}
	}
}
/*********************************************************************************************************
**                                      End Of File
*********************************************************************************************************/
