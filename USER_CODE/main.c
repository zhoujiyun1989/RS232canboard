//
//


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

/*********************************************************************************************************
  宏定义
*********************************************************************************************************/

//define Transparent  透明传输
//define Identification   带标识传输
//define Modbus   modbus协议传输
//串口到CAN，会传输ID和dlc
//发出去的ID实际只有29位,第30位为1时为扩展帧为0时为标准帧，第31位为1时为远程帧为0时为数据帧
//CAN到串口，会传输ID和dlc

#define UART_BPS       					115200                                        /* 串口通信波特率               */
#define CAN_SPEED						250
#define RID 							0x00000000
#define	RID_STD_EXT						1																							//0时为标准帧,1时为扩展帧
#define	RID_DAT_RTR						0																							//0时为数据帧,1时为远程帧
#define RID_CHECK						(RID&0x1fffffff)|(RID_STD_EXT<<29)|(RID_DAT_RTR<<30)
#define RMASK 							0x00																		//接收0x00-0xFF的ID帧
struct uartframe {
	uint8_t header[2];
	uint8_t cmd;
	uint8_t leng[2];
	uint8_t retain;
	uint8_t *data;
	uint8_t check;
};
enum State {
	findhear = 0,
	cmdhandle,
	lenhandle,
	dataread,
	checkhandle
}enum_UartHandleState;

/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
int timer_ms_cont = 0;
volatile   		uint8_t          GucRcvNew;                                  /* 串口接收新数据的标志         */
uint8_t      	GucRcvBuf[9] ;                                            /* 串口接收数据缓冲区           */
uint32_t     	GulNum;                                                    /* 串口接收数据的个数           */
struct uartframe uartframesend;
struct uartframe uartframereceive;
uint8_t	receivedata[13];

/*********************************************************************************************************
** Function name:       delay_ms
** Descriptions:        软件延时时间为1ms
** input parameters:    无
** output parameters:   无
** Returned value:      无
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
* Description:          TIMER16_0 中断处理函数
* Input:                无
* Output:               无
* Return:               无
*********************************************************************************************************/
void TIMER16_0_IRQHandler (void)
{
    LPC_TMR16B0->IR = 0x01;                                             /* 清除中断标志                 */
    if (timer_ms_cont++ == 1000) {
				timer_ms_cont = 0;
				if(LPC_GPIO2->DATA & (1ul << 7))
					LPC_GPIO2->DATA &= ~(1ul << 7);          //置低
				else
					LPC_GPIO2->DATA |= (1ul << 7);            //置高
    } 
}

/*********************************************************************************************************
** Function name:       timer0Init
** Descriptions:        16位定时器0初始化函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void timer0Init (void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);                              /* 打开定时器模块               */

    LPC_TMR16B0->IR      = 1;
    LPC_TMR16B0->PR      = 100;                                        /* 设置分频系数                 */
    LPC_TMR16B0->MCR     = 3;                                           /* 设置MR0匹配后复位TC并产生中断*/
    //LPC_TMR16B0->MR0     = SystemFrequency/1000;                        /* 设置中断时间1S               */
	LPC_TMR16B0->MR0     = SystemFrequency/100000;                        /* 设置中断时间1mS               */
                                                                        /* 16位定时器计数最大值65535    */
    LPC_TMR16B0->TCR     = 0x01;                                        /* 启动定时器                   */
    NVIC_EnableIRQ(TIMER_16_0_IRQn);                                    /* 设置中断并使能               */
    NVIC_SetPriority(TIMER_16_0_IRQn, 2);
}

/*********************************************************************************************************
** Function name:       __nop
** Descriptions:        执行空指令，在C语言中，插入汇编语句
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
__asm void nop(void)
{
    NOP
}
/*********************************************************************************************************
** Function name:       GPIOInit
** Descriptions:        初始化P1_10为输入,连接震动模块的输出引脚，低电平报警
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void GPIOInit(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);                          // 使能GPIO外设时钟
		LPC_IOCON->PIO2_7  &= ~0x07;
    LPC_GPIO2->DIR |= (1ul << 7);  

}

/*********************************************************************************************************
** Function name:       CANInit
** Descriptions:        初始化CAN外设
**											摘自    LPC111x/LPC11Cxx User manual
**											C_CAN controller
**											The C_CAN clock control bit 17 in the SYSAHBCLKCTRL register (Table 21) and the
**											C_CAN reset control bit 3 in the PRESETCTRL register (Table 9) are only functional for
**											parts LPC11Cxx/101/201/301.
**											 
**											The SYSAHBCLKCTRL register gates the system clock to the various peripherals and
**											memories. UART, the WDT, and SPI0/1 have individual clock dividers to derive peripheral
**											clocks from the main clock. 
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CANInit(void)
{

    LPC_SYSCON->PRESETCTRL    |= (1 << 3);                            // 解除CAN外设复位
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 17);                           //初始化SystemInit中，已经使能，使能CAN外设
		
	if(CAN_SPEED == 250)
    	(*rom)->pCANAPI->init_can(&CanApiClkInitTable_250[0]);                //设置CAN的波特率为250k
	else if(CAN_SPEED == 500)
		(*rom)->pCANAPI->init_can(&CanApiClkInitTable_500[0]);                //设置CAN的波特率为500k	

    /*配置CAN中断*/
    NVIC_EnableIRQ(CAN_IRQn);                                         //使能CAN的中断
    NVIC_SetPriority(CAN_IRQn, 1);                                    //设置CAN的优先级为1
    (*rom)->pCANAPI->config_calb((CAN_CALLBACKS *)&callbacks);        //设置CAN的回调函数  

	/*配置接收帧*/
	msg_cof0.msgobj = 0;
	msg_cof0.mask = RMASK;                                        //设置接收的屏蔽码
	msg_cof0.mode_id = (RID&0x1fffffff)|(0<<29)|(0<<30); //设置接收的ID
	(*rom)->pCANAPI-> config_rxmsgobj(&msg_cof0);                      //配置接收对象

	msg_cof1.msgobj = 1;
	msg_cof1.mask = RMASK;                                        //设置接收的屏蔽码
	msg_cof1.mode_id = (RID&0x1fffffff)|(1<<29)|(0<<30); //设置接收的ID
	(*rom)->pCANAPI-> config_rxmsgobj(&msg_cof1);                      //配置接收对象

	msg_cof2.msgobj = 2;
	msg_cof2.mask = RMASK;                                        //设置接收的屏蔽码
	msg_cof2.mode_id = (RID&0x1fffffff)|(0<<29)|(1<<30); //设置接收的ID
	(*rom)->pCANAPI-> config_rxmsgobj(&msg_cof2);                      //配置接收对象

	msg_cof3.msgobj = 3;
	msg_cof3.mask = RMASK;                                        //设置接收的屏蔽码
	msg_cof3.mode_id = (RID&0x1fffffff)|(1<<29)|(1<<30); //设置接收的ID
	(*rom)->pCANAPI-> config_rxmsgobj(&msg_cof3);                      //配置接收对象

	
}

/*********************************************************************************************************
** Function name:        uartInit
** Descriptions:        串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为115200
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uartInit (void)
{
    uint16_t usFdiv;
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);                             /* 使能IOCON时钟                */
    LPC_IOCON->PIO1_6 |= 0x01;                                          /* 将P1.6 1.7配置为RXD和TXD     */
    LPC_IOCON->PIO1_7 |= 0x01;

    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);                               /* 打开UART功能部件时钟         */
    LPC_SYSCON->UARTCLKDIV       = 0x01;                                /* UART时钟分频                 */

    LPC_UART->LCR  = 0x83;                                              /* 允许设置波特率               */
    usFdiv = (SystemFrequency/LPC_SYSCON->UARTCLKDIV/16)/UART_BPS;      /* 设置波特率                   */
    LPC_UART->DLM  = usFdiv / 256;
    LPC_UART->DLL  = usFdiv % 256;
    LPC_UART->LCR  = 0x03;                                              /* 锁定波特率                   */
    LPC_UART->FCR  = 0x87;                                              /* 使能FIFO，设置8个字节触发点  */
    
    NVIC_EnableIRQ(UART_IRQn);                                          /* 使能UART中断，并配置优先级   */
    NVIC_SetPriority(UART_IRQn, 1);

    LPC_UART->IER  = 0x01;                                              /* 使能接收中断                 */
}

/*********************************************************************************************************
** Function name:       uartSendByte
** Descriptions:        向串口发送子节数据，并等待数据发送完成，使用查询方式
** input parameters:    ucDat:   要发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uartSendByte (uint8_t ucDat)
{
    LPC_UART->THR = ucDat;                                              /*  写入数据                    */
    while ((LPC_UART->LSR & 0x40) == 0);                                /*  等待数据发送完毕            */
}

/*********************************************************************************************************
** Function name:       uartSendStr
** Descriptions:        向串口发送字符串
** input parameters:    pucStr:  要发送的字符串指针
**                      ulNum:   要发送的数据个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uartSendStr (uint8_t const *pucStr, uint32_t ulNum)
{
    uint32_t i;

    for (i = 0; i < ulNum; i++){                                        /* 发送指定个字节数据           */
        uartSendByte (*pucStr++);
    }
}
/*********************************************************************************************************
* Function Name:        UART_IRQHandler
* Description:          UART中断服务函数
* Input:                无
* Output:               无
* Return:               无
*********************************************************************************************************/
void UART_IRQHandler (void)
{
    GulNum = 0;

    while ((LPC_UART->IIR & 0x01) == 0){                                /*  判断是否有中断挂起          */
        switch (LPC_UART->IIR & 0x0E){                                  /*  判断中断标志                */
        
            case 0x04:                                                  /*  接收数据中断                */
                GucRcvNew = 1;                                          /*  置接收新数据标志            */
				GucRcvBuf[0] = 8;
                for (GulNum = 1; GulNum < 9; GulNum++){                 /*  连续接收8个字节             */
					GucRcvBuf[GulNum] = LPC_UART->RBR;
					put_byte_buffer(&rcv_byte_buffer, &GucRcvBuf[GulNum]);
                }
                break;
            
            case 0x0C:                                                  /*  字符超时中断                */
                GucRcvNew = 1;
				GulNum = 1; 
                while ((LPC_UART->LSR & 0x01) == 0x01){                 /*  判断数据是否接收完毕        */
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
	int i = 0;
	uartSendByte(uartframesendvar.header[0]);
	uartSendByte(uartframesendvar.header[1]);
	uartSendByte(uartframesendvar.cmd);
	uartSendByte(uartframesendvar.leng[0]);
	uartSendByte(uartframesendvar.leng[1]);
	uartSendByte(uartframesendvar.retain);
	for(i=0;i<13;i++)
		uartSendByte(uartframesendvar.data[i]);
	uartframesendvar.check = uartframesendvar.header[0]^\
							uartframesendvar.header[1]^\
							uartframesendvar.cmd^\
							uartframesendvar.leng[0]^\
							uartframesendvar.leng[1]^\
							uartframesendvar.retain;
	for(i=0;i<13;i++)
	uartframesendvar.check ^= uartframesendvar.data[i];
	uartSendByte(uartframesendvar.check);
}

int receiveuartframe(uint8_t uartreceive, struct uartframe* puartframereceive, enum State* penum_UartHandleState)
{
	int static i = 0;
	uint32_t id = 0;
	
	switch(*penum_UartHandleState)
	{
		case (findhear):
		{
			if(uartreceive==0xAA)
			{
				i = 1;
				(*puartframereceive).header[0] = uartreceive;
			}
			else if(i==1 && uartreceive==0x75)
			{
				*penum_UartHandleState = cmdhandle;
				(*puartframereceive).header[1] = uartreceive;
				i = 0;
			}
			else
				i = 0;
			break;
		}
		
		case (cmdhandle):
		{
			(*puartframereceive).cmd= uartreceive;
			if(((*puartframereceive).cmd>=1)||((*puartframereceive).cmd<=3))
				*penum_UartHandleState = lenhandle;
			else
				*penum_UartHandleState = findhear;
			break;
		}

		case (lenhandle):
		{
			if(i<2)
				(*puartframereceive).leng[i]= uartreceive;
			else
				(*puartframereceive).retain = uartreceive;
			i++;
			if(i>=3)
			{
				*penum_UartHandleState = dataread;
				i = 0;
			}
			break;
		}

		case (dataread):
		{
			receivedata[i]= uartreceive;
			i++;
			if(i>=((*puartframereceive).leng[0]*255 + (*puartframereceive).leng[1]))
			{
				(*puartframereceive).data = receivedata;
				*penum_UartHandleState = checkhandle;
				i = 0;
			}
			break;
		}
		case (checkhandle):
		{
			(*puartframereceive).check = (*puartframereceive).header[0]^\
							(*puartframereceive).header[1]^\
							(*puartframereceive).cmd^\
							(*puartframereceive).leng[0]^\
							(*puartframereceive).leng[1]^\
							(*puartframereceive).retain;
			for(i=0;i<(*puartframereceive).leng[0]*255 + (*puartframereceive).leng[1];i++)
			(*puartframereceive).check ^= (*puartframereceive).data[i];
			//uartSendByte((*puartframereceive).check);
			//uartSendByte(uartreceive);

			if((*puartframereceive).check ==uartreceive)
			{
				id = 0;
				for(i=0;i<4;i++)
				{
					id = (id<<8) | (*puartframereceive).data[i];
				}
				//发送的ID是否带有帧信息
				//id = (id&0x1FFFFFFF)|(TID_STD_EXT<<29)|(TID_DAT_RTR<<30);
				//msg_obj.msgobj = 2;
				msg_obj.mode_id = id;
				msg_obj.dlc = (*puartframereceive).data[4];
				for(i=0;i<msg_obj.dlc;i++)
				{
					msg_obj.data[i] = (*puartframereceive).data[5+i];
				}
				(*rom)->pCANAPI->can_transmit(&msg_obj);
			}
			*penum_UartHandleState = findhear;
			i = 0;
			break;
		}

		default: 
		{
			*penum_UartHandleState = findhear;
			i = 0;
			break;
		}
	}
}

/*********************************************************************************************************
** Function name:       main
** Descriptions:        CAN数据收发,直接调用LPC11C14片上CAN 固件API函数，默认波特率为500kbps。
**                      
**                      
**                      
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int main(void)
{
	int i= 0;
	uint32_t id = 0, msg_id = 0;
	uint8_t	byte_send;
	uint8_t	senddata[13];
	
	SystemInit();                                                     //初始化目标板，切勿删除
    GPIOInit();                                                       //初始化P1_10为输入
	timer0Init();
	
	uartInit();
	CANInit();

	/*while (1)
	{
		delay_ms(500);
		(LPC_GPIO2->DATA |= (1ul << 7));			  //置高
		delay_ms(500);
		(LPC_GPIO2->DATA &= ~(1ul << 7)); 		 //置低
	}*/

	while (1) 
	{
		if(get_can_buffer(&rcv_can_buffer,&msg_obj))
		{
			uartframesend.header[0] = 0xAA;
			uartframesend.header[1] = 0x75;
			uartframesend.cmd = 0x01;
			uartframesend.leng[0] = 0x00;
			uartframesend.leng[1] = 0x0D;
			uartframesend.retain = 0x00;
			
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
			//添加后8位
			if(msg_obj.dlc<8)
			{
				for(i=0;i<=8-msg_obj.dlc;i++)
				{
					senddata[5+msg_obj.dlc+i] = i;
				}
			}
			uartframesend.data = &senddata[0];
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
			//发送的ID是否带有帧信息
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
			//清空buffer
			rcv_byte_buffer.oldest = rcv_byte_buffer.next;
			timer_ms_cont = 0;
		}
*/
		if(is_byte_buffer_empty(&rcv_byte_buffer)==0)
		{
			get_byte_buffer(&rcv_byte_buffer,&byte_send);
			//uartSendByte(byte_send);
			receiveuartframe(byte_send, &uartframereceive, &enum_UartHandleState);
		}
	}
}
/*********************************************************************************************************
**                                      End Of File
*********************************************************************************************************/
