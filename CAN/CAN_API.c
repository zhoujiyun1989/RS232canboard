/********************************************************************************************************
--Design name:          CAN_API.c
--Target devices:       TinyM0-CAN-T
--Create data:          2014-3-31
--Revision:             v1.0
--Copyright(c):         AngDe
--Additional comments:  �ڹٷ������������н����޸ģ����յ�IDΪ55������λ0x34������Ϊ1ʱ�����ص�ǰ�ı���״̬
                        ������������0x34  0x31  0x0a,û�б�������0x34  0x30  0x0a����־λ��0x34��
*********************************************************************************************************/

#include "CAN_API.h"
#include "stdio.h"
#include "buffer.h"

/*********************************************************************************************************
CAN  ISP ���� ������־ �����ʸı��־ �������ò�����
*********************************************************************************************************/
extern char GucBtResetFlag;
extern char GucUpFlag  ;
extern char GucBtcFlag ;

/*********************************************************************************************************
  ע��ص�����
*********************************************************************************************************/
const CAN_CALLBACKS callbacks = {
    CAN_rx,
    CAN_tx,
    CAN_error,
    CANOPEN_sdo_exp_read,
    CANOPEN_sdo_exp_write,
    CANOPEN_sdo_seg_read,
    CANOPEN_sdo_seg_write,
    CANOPEN_sdo_req
};
/*********************************************************************************************************
  �����������ʼ��
*********************************************************************************************************/
ROM **rom = (ROM**)0x1fff1ff8;                                  // ָ��̼���ַ��ָ��
CAN_MSG_OBJ  msg_obj;                                           // ����CAN���ı���
CAN_MSG_OBJ  msg_obj_t;                                         //����CAN���ı���
CAN_MSG_OBJ  msg_cof1;                                           //����CAN��������
CAN_MSG_OBJ  msg_cof2;
CAN_MSG_OBJ  msg_cof3;
CAN_MSG_OBJ  msg_cof0;



//CAN_MSG_OBJ  msg_reback;                                        //����CAN��������

/*********************************************************************************************************
  ����CANOPEN_sdo_seg_read/write() callback 'openclose'������ֵ
*********************************************************************************************************/
#define CAN_SDOSEG_SEGMENT  0                                   //�ֶζ�/д
#define CAN_SDOSEG_OPEN     1                                   //ͨ����
#define CAN_SDOSEG_CLOSE    2                                   //ͨ���ر�

/*********************************************************************************************************
  ��������ʱ�ӳ�ʼ��
*********************************************************************************************************/
INT32U CanApiClkInitTable_250[2] = {
    0x00000000UL,                                               //CANCLKDIV
    0x00007eC7UL                                                //CANBTR
};
INT32U CanApiClkInitTable_500[2] = {
    0x00000000UL,                                               //CANCLKDIV
    0x00007eC3UL                                                //CANBTR
};

/*
 *CANCLKDIV����ֵ0  CAN_CLKΪ48M
 *0000: CAN_CLK = PCLK divided by 1.
 *0001: CAN_CLK = PCLK divided by 2.
 *0010: CAN_CLK = PCLK divided by 3
 *0010: CAN_CLK = PCLK divided by 4.
 *...
 *1111: CAN_CLK = PCLK divided by 16.
*/
/* CANBTR�ο����ֵ
** 48M
** 500K  0x00007eC3
** 100K  0x00007eD3
** 250k  0x00007eC7
** 24M   
** 500K  0x00007eC1
** 100K  0x00007eC9
** 20K   0x00007eF1
*/    
/*********************************************************************************************************
  CANOpen �̶������ֵ� ֻ���ڼ���д����
*********************************************************************************************************/
CAN_ODCONSTENTRY myConstOD [] = {
    /* ����, ������,    ����,    ֵ */
    { 0x1000, 0x00,     4,         0x3143504CUL },              // Device Type  "Test"
    { 0x1018, 0x00,     1,         0x00000003UL },              // Number of entries
    { 0x1018, 0x01,     4,         0x00000003UL },              // Vendor ID
    { 0x2000, 0x00,     1,         (INT32U)'M' },
};

/*********************************************************************************************************
  �����ֵ���ʹ�õı��� 
*********************************************************************************************************/
INT8U  error_register;
INT32U device_id;
INT32U fw_ver;
INT32U param;
/*********************************************************************************************************
  �����ֵ��� 
*********************************************************************************************************/
CAN_ODENTRY myOD [] = {
    // ����, ������,     �������� | ����,         ֵָ��
    { 0x1001, 0x00,     OD_EXP_RO | 1,            (INT8U *)&error_register },
    { 0x1018, 0x02,     OD_EXP_RO | 4,            (INT8U *)&device_id },        // Product code
    { 0x1018, 0x03,     OD_EXP_RO | 4,            (INT8U *)&fw_ver },           //Revision number
    { 0x2001, 0x00,     OD_EXP_RW | 4,            (INT8U *)&param },
    { 0x2200, 0x00,     OD_SEG_RW,                 (INT8U *)NULL },    
};
/*********************************************************************************************************
  CANopen ���ýṹ�� �û�����
*********************************************************************************************************/
CAN_CANOPENCFG  myCANopen = {
    0x10,                                                               // node_id
    5,                                                                  // msgobj_rx
    6,                                                                  // msgobj_tx
    1,                                                                  // enable_irq
    sizeof(myConstOD)/sizeof(myConstOD[0]),                             // od_const_num
    (CAN_ODCONSTENTRY *)myConstOD,                                      // od_const_table
    sizeof(myOD)/sizeof(myOD[0]),                                       // od_num
    (CAN_ODENTRY *)myOD,                                                // od_table
};
/*********************************************************************************************************
** �㲥֡ ����ID���� CANopen�ڵ�ID
*********************************************************************************************************/
extern INT32U GulNetWorkFrameID ;
extern INT32U GulSelfFrameID    ;
extern INT8U  GucCanOpenNodeID  ;
/*********************************************************************************************************
** Function name:       canIsr
** Descriptions:        CAN�жϣ�Ϊ���ܴ���CAN�¼��͵��ûص�������Ӧ�ó������ֱ�Ӵ��жϴ���������
**                      �е���CAN API�жϴ������CAN API�жϴ���������CAN�����Ͻ��յ������ݺͼ�⵽
**                      ��״̬����ȡ��صĲ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void CAN_IRQHandler(void)
{
    (*rom)->pCANAPI->isr();
}
/*********************************************************************************************************
** Function name:       GetCheckNum()
** Descriptions:        У�������
** input parameters:    
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
INT8U GetCheckNum (CAN_MSG_OBJ *pmsg_obj)
{
    INT8U ucCheckNum = 0;
    INT8U i          = 0;
    ucCheckNum = ((pmsg_obj->mode_id >> 8) & (0xFF)) + (pmsg_obj->mode_id & 0xFF);
    
    for (i = 0; i < pmsg_obj->dlc - 1 ; i++) {
        ucCheckNum += pmsg_obj->data[i];
    }   
    return ucCheckNum;
}
/*********************************************************************************************************
** Function name:       CAN_rx
** Descriptions:        ���ݽ��մ��������յ��ı����ٷ��ͳ�ȥ
** input parameters:    msg_obj_num  ����ȡ���ı��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void CAN_rx(INT8U msg_obj_num)
{  
	int i = 0;
	#if 1
	CAN_MSG_OBJ msg_reback;
	msg_reback.msgobj = msg_obj_num;
	(*rom)->pCANAPI->can_receive(&msg_reback);
	put_can_buffer(&rcv_can_buffer,&msg_reback);
	//uartSendByte(0x35);
	#endif
	
	#if 0
	CAN_MSG_OBJ msg_obj;
	uartSendByte(msg_obj_num);
	
	msg_obj.msgobj = msg_obj_num;
	(*rom)->pCANAPI->can_receive(&msg_obj);
	uartSendByte((char)(msg_obj.mode_id >> 8*0));
	uartSendByte((char)(msg_obj.mode_id >> 8*1));
	uartSendByte((char)(msg_obj.mode_id >> 8*2));
	uartSendByte((char)(msg_obj.mode_id >> 8*3));
	//msg_obj.msgobj = 2;
	//msg_obj.mode_id = 33;
	(*rom)->pCANAPI->can_transmit(&msg_obj);                            /* �����յ������ݻط���ȥ       */
	#endif
}
/*********************************************************************************************************
** Function name:       CAN_tx
** Descriptions:        �ص������У��������ݴ���
** input parameters:    msg_obj_num  �����ͱ��ı��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void CAN_tx(INT8U msg_obj_num)
{
    /* 
     *  �û��������
     */
    msg_obj_num = msg_obj_num;
}
/*********************************************************************************************************
** Function name:       CAN_error
** Descriptions:        ���ߴ�����
** input parameters:    error_info  ����״̬
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void CAN_error(INT32U error_info)
{
    /*
     * ����û��������߹ر�״̬�����Ը���Ӧ�ó������³�ʼ��CAN������
     */
    return;
}
/*********************************************************************************************************
** Function name:       CANOPEN_sdo_exp_read
** Descriptions:        ���ڼ��ٶ����ʵ�CANopen�ص�
** input parameters:    index������  subindex��������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
INT32U CANOPEN_sdo_exp_read(INT16U index, INT8U subindex)
{
    /*
     *    �û��������
     */
    return 0;                                                       // ����0��ʾ�ɹ�
}

/*********************************************************************************************************
** Function name:       CANOPEN_sdo_exp_write
** Descriptions:        ���ڼ���д���ʵ�CANopen�ص�
** input parameters:    index������  subindex��������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
INT32U CANOPEN_sdo_exp_write(INT16U index, INT8U subindex, INT8U *dat_ptr)
{

    /*
     *    �û��������
     */

    return 0;   
}
/*********************************************************************************************************
** Function name:       CANOPEN_sdo_seg_read
** Descriptions:        ���ڷֶζ����ʵ�CANopen�ص�
** input parameters:    index������  subindex��������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
INT32U CANOPEN_sdo_seg_read(INT16U index, INT8U subindex, INT8U openclose, INT8U *length, INT8U *data, INT8U *last)
{
    /*
     * �û��������
     */
   return 0;                                                        // ����0��ʾ�ɹ�
}
/*********************************************************************************************************
** Function name:       CANOPEN_sdo_seg_write
** Descriptions:        ���ڷֶ�д���ʵ�CANopen�ص�
** input parameters:    index������  subindex��������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
INT32U CANOPEN_sdo_seg_write(INT16U index, INT8U subindex, INT8U openclose, INT8U length, INT8U *data, INT8U *fast_resp)
{
    /*
     * �û��������
     */
    return 0;                                                       // ����0��ʾ�ɹ�
}
/*********************************************************************************************************
** Function name:       CANOPEN_sdo_req
** Descriptions:        CANOPEN SDO����
** input parameters:    length_req������ȣ�req_ptr��request�����ָ�룬
**                        length_resp����Ӧ���ȵ�ָ�룬resp_ptr��response�����ָ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
INT8U CANOPEN_sdo_req(INT8U length_req, INT8U *req_ptr, INT8U *length_resp, INT8U *resp_ptr)
{
    /*
     * �û��������
     */
    return 0;                                                       // ����0��ʾ�ɹ�
}
/*********************************************************************************************************
**                                      End Of File
*********************************************************************************************************/
