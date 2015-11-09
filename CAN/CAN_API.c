/********************************************************************************************************
--Design name:          CAN_API.c
--Target devices:       TinyM0-CAN-T
--Create data:          2014-3-31
--Revision:             v1.0
--Copyright(c):         AngDe
--Additional comments:  在官方发布的例程中进行修改，当收到ID为55，数据位0x34，长度为1时，返回当前的报警状态
                        ，当报警返回0x34  0x31  0x0a,没有报警返回0x34  0x30  0x0a，标志位：0x34。
*********************************************************************************************************/

#include "CAN_API.h"
#include "stdio.h"
#include "buffer.h"

/*********************************************************************************************************
CAN  ISP 升级 升级标志 波特率改变标志 重新配置波特率
*********************************************************************************************************/
extern char GucBtResetFlag;
extern char GucUpFlag  ;
extern char GucBtcFlag ;

/*********************************************************************************************************
  注册回调函数
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
  变量定义与初始化
*********************************************************************************************************/
ROM **rom = (ROM**)0x1fff1ff8;                                  // 指向固件地址的指针
CAN_MSG_OBJ  msg_obj;                                           // 定义CAN报文变量
CAN_MSG_OBJ  msg_obj_t;                                         //定义CAN报文变量
CAN_MSG_OBJ  msg_cof1;                                           //定义CAN接收配置
CAN_MSG_OBJ  msg_cof2;
CAN_MSG_OBJ  msg_cof3;
CAN_MSG_OBJ  msg_cof0;



//CAN_MSG_OBJ  msg_reback;                                        //定义CAN请求数据

/*********************************************************************************************************
  用于CANOPEN_sdo_seg_read/write() callback 'openclose'参数的值
*********************************************************************************************************/
#define CAN_SDOSEG_SEGMENT  0                                   //分段读/写
#define CAN_SDOSEG_OPEN     1                                   //通道打开
#define CAN_SDOSEG_CLOSE    2                                   //通道关闭

/*********************************************************************************************************
  波特率与时钟初始化
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
 *CANCLKDIV设置值0  CAN_CLK为48M
 *0000: CAN_CLK = PCLK divided by 1.
 *0001: CAN_CLK = PCLK divided by 2.
 *0010: CAN_CLK = PCLK divided by 3
 *0010: CAN_CLK = PCLK divided by 4.
 *...
 *1111: CAN_CLK = PCLK divided by 16.
*/
/* CANBTR参考设计值
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
  CANOpen 固定对象字典 只用于加速写访问
*********************************************************************************************************/
CAN_ODCONSTENTRY myConstOD [] = {
    /* 索引, 子索引,    长度,    值 */
    { 0x1000, 0x00,     4,         0x3143504CUL },              // Device Type  "Test"
    { 0x1018, 0x00,     1,         0x00000003UL },              // Number of entries
    { 0x1018, 0x01,     4,         0x00000003UL },              // Vendor ID
    { 0x2000, 0x00,     1,         (INT32U)'M' },
};

/*********************************************************************************************************
  对象字典中使用的变量 
*********************************************************************************************************/
INT8U  error_register;
INT32U device_id;
INT32U fw_ver;
INT32U param;
/*********************************************************************************************************
  对象字典中 
*********************************************************************************************************/
CAN_ODENTRY myOD [] = {
    // 索引, 子索引,     访问类型 | 长度,         值指针
    { 0x1001, 0x00,     OD_EXP_RO | 1,            (INT8U *)&error_register },
    { 0x1018, 0x02,     OD_EXP_RO | 4,            (INT8U *)&device_id },        // Product code
    { 0x1018, 0x03,     OD_EXP_RO | 4,            (INT8U *)&fw_ver },           //Revision number
    { 0x2001, 0x00,     OD_EXP_RW | 4,            (INT8U *)&param },
    { 0x2200, 0x00,     OD_SEG_RW,                 (INT8U *)NULL },    
};
/*********************************************************************************************************
  CANopen 配置结构体 用户定义
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
** 广播帧 自身ID定义 CANopen节点ID
*********************************************************************************************************/
extern INT32U GulNetWorkFrameID ;
extern INT32U GulSelfFrameID    ;
extern INT8U  GucCanOpenNodeID  ;
/*********************************************************************************************************
** Function name:       canIsr
** Descriptions:        CAN中断，为了能处理CAN事件和调用回调函数，应用程序必须直接从中断处理程序服务
**                      中调用CAN API中断处理程序。CAN API中断处理程序根据CAN总线上接收到的数据和检测到
**                      的状态来采取相关的操作
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CAN_IRQHandler(void)
{
    (*rom)->pCANAPI->isr();
}
/*********************************************************************************************************
** Function name:       GetCheckNum()
** Descriptions:        校验码计算
** input parameters:    
** output parameters:   无
** Returned value:      无
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
** Descriptions:        数据接收处理，将接收到的报文再发送出去
** input parameters:    msg_obj_num  待读取报文编号
** output parameters:   无
** Returned value:      无
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
	(*rom)->pCANAPI->can_transmit(&msg_obj);                            /* 将接收到的数据回发出去       */
	#endif
}
/*********************************************************************************************************
** Function name:       CAN_tx
** Descriptions:        回调函数中，发送数据处理
** input parameters:    msg_obj_num  待发送报文编号
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CAN_tx(INT8U msg_obj_num)
{
    /* 
     *  用户程序添加
     */
    msg_obj_num = msg_obj_num;
}
/*********************************************************************************************************
** Function name:       CAN_error
** Descriptions:        总线错误处理
** input parameters:    error_info  错误状态
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CAN_error(INT32U error_info)
{
    /*
     * 如果用户进入总线关闭状态，可以告诉应用程序重新初始化CAN控制器
     */
    return;
}
/*********************************************************************************************************
** Function name:       CANOPEN_sdo_exp_read
** Descriptions:        用于加速读访问的CANopen回调
** input parameters:    index：索引  subindex：子索引
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
INT32U CANOPEN_sdo_exp_read(INT16U index, INT8U subindex)
{
    /*
     *    用户程序添加
     */
    return 0;                                                       // 返回0表示成功
}

/*********************************************************************************************************
** Function name:       CANOPEN_sdo_exp_write
** Descriptions:        用于加速写访问的CANopen回调
** input parameters:    index：索引  subindex：子索引
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
INT32U CANOPEN_sdo_exp_write(INT16U index, INT8U subindex, INT8U *dat_ptr)
{

    /*
     *    用户程序添加
     */

    return 0;   
}
/*********************************************************************************************************
** Function name:       CANOPEN_sdo_seg_read
** Descriptions:        用于分段读访问的CANopen回调
** input parameters:    index：索引  subindex：子索引
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
INT32U CANOPEN_sdo_seg_read(INT16U index, INT8U subindex, INT8U openclose, INT8U *length, INT8U *data, INT8U *last)
{
    /*
     * 用户程序添加
     */
   return 0;                                                        // 返回0表示成功
}
/*********************************************************************************************************
** Function name:       CANOPEN_sdo_seg_write
** Descriptions:        用于分段写访问的CANopen回调
** input parameters:    index：索引  subindex：子索引
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
INT32U CANOPEN_sdo_seg_write(INT16U index, INT8U subindex, INT8U openclose, INT8U length, INT8U *data, INT8U *fast_resp)
{
    /*
     * 用户程序添加
     */
    return 0;                                                       // 返回0表示成功
}
/*********************************************************************************************************
** Function name:       CANOPEN_sdo_req
** Descriptions:        CANOPEN SDO请求
** input parameters:    length_req：命令长度，req_ptr：request命令的指针，
**                        length_resp：回应长度的指针，resp_ptr：response命令的指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
INT8U CANOPEN_sdo_req(INT8U length_req, INT8U *req_ptr, INT8U *length_resp, INT8U *resp_ptr)
{
    /*
     * 用户程序添加
     */
    return 0;                                                       // 返回0表示成功
}
/*********************************************************************************************************
**                                      End Of File
*********************************************************************************************************/
