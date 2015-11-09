/********************************************************************************************************
--Design name:          CAN_API.h
--Target devices:       TinyM0-CAN-T
--Create data:          2014-3-31
--Revision:             v1.0
--Copyright(c):         AngDe
--Additional comments:  在官方发布的例程中进行修改
*********************************************************************************************************/
#ifndef __CAN_API_H__
#define __CAN_API_H__

#include "stdint.h"
#include "LPC11xx.h"

#define INT8U	uint8_t   
#define INT16U	uint16_t  
#define INT32U	uint32_t  

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif

#ifndef NULL
#define NULL 0
#endif
/*********************************************************************************************************
  CAN 报文结构与变量定义
*********************************************************************************************************/
#define CAN_MSGOBJ_STD 0x00000000UL                         // CAN 2.0a 11-bit ID
#define CAN_MSGOBJ_EXT 0x20000000UL                         // CAN 2.0b 29-bit ID
#define CAN_MSGOBJ_DAT 0x00000000UL                         // data frame
#define CAN_MSGOBJ_RTR 0x40000000UL                         // rtr frame
/*********************************************************************************************************
  CAN_ODENTRY.entrytype_len的高位半字节值
*********************************************************************************************************/
#define OD_NONE   0x00                                      // Object Dictionary entry doesn't exist
#define OD_EXP_RO 0x10                                      // Object Dictionary entry expedited, read-only
#define OD_EXP_WO 0x20                                      // Object Dictionary entry expedited, write-only
#define OD_EXP_RW 0x30                                      // Object Dictionary entry expedited, read-write
#define OD_SEG_RO 0x40                                      // Object Dictionary entry segmented, read-only 
#define OD_SEG_WO 0x50                                      // Object Dictionary entry segmented, write-only
#define OD_SEG_RW 0x60                                      // Object Dictionary entry segmented, read-write
/*********************************************************************************************************
  错误状态位
*********************************************************************************************************/
#define CAN_ERROR_NONE 0x00000000UL
#define CAN_ERROR_PASS 0x00000001UL
#define CAN_ERROR_WARN 0x00000002UL
#define CAN_ERROR_BOFF 0x00000004UL
#define CAN_ERROR_STUF 0x00000008UL
#define CAN_ERROR_FORM 0x00000010UL
#define CAN_ERROR_ACK  0x00000020UL
#define CAN_ERROR_BIT1 0x00000040UL
#define CAN_ERROR_BIT0 0x00000080UL
#define CAN_ERROR_CRC  0x00000100UL
/**************************************************************************
SDO Abort Codes
**************************************************************************/
#define SDO_ABORT_TOGGLE          0x05030000UL  // Toggle bit not alternated
#define SDO_ABORT_SDOTIMEOUT      0x05040000UL  // SDO protocol timed out
#define SDO_ABORT_UNKNOWN_COMMAND 0x05040001UL  // Client/server command specifier not valid or unknown
#define SDO_ABORT_UNSUPPORTED     0x06010000UL  // Unsupported access to an object
#define SDO_ABORT_WRITEONLY       0x06010001UL  // Attempt to read a write only object
#define SDO_ABORT_READONLY        0x06010002UL  // Attempt to write a read only object
#define SDO_ABORT_NOT_EXISTS      0x06020000UL  // Object does not exist in the object dictionary
#define SDO_ABORT_PARAINCOMP      0x06040043UL  // General parameter incompatibility reason
#define SDO_ABORT_ACCINCOMP       0x06040047UL  // General internal incompatibility in the device
#define SDO_ABORT_TYPEMISMATCH    0x06070010UL  // Data type does not match, length of service parameter does not match
#define SDO_ABORT_UNKNOWNSUB      0x06090011UL  // Sub-index does not exist
#define SDO_ABORT_VALUE_RANGE     0x06090030UL  // Value range of parameter exceeded (only for write access)
#define SDO_ABORT_TRANSFER        0x08000020UL  // Data cannot be transferred or stored to the application
#define SDO_ABORT_LOCAL           0x08000021UL  // Data cannot be transferred or stored to the application because of local control
#define SDO_ABORT_DEVSTAT         0x08000022UL  // Data cannot be transferred or stored to the application because of the present device state


/*********************************************************************************************************
  CAN报文结构体定义，报文对象
*********************************************************************************************************/
typedef struct _CAN_MSG_OBJ {
    INT32U mode_id;
    INT32U mask;
    INT8U data[8];
    INT8U dlc;
    INT8U msgobj;
}CAN_MSG_OBJ;

/*********************************************************************************************************
  CANOpen 报文结构与变量定义
*********************************************************************************************************/
typedef struct _CAN_ODCONSTENTRY {
    INT16U index;
    INT8U subindex;
    INT8U len;
    INT32U val;
}CAN_ODCONSTENTRY;
/*********************************************************************************************************
  CANOD 对象字典
*********************************************************************************************************/
typedef struct _CAN_ODENTRY {
    INT16U index;
    INT8U subindex;
    INT8U entrytype_len;
    INT8U *val;
}CAN_ODENTRY;
/*********************************************************************************************************
  CANopen配置结构体
*********************************************************************************************************/
typedef struct _CAN_CANOPENCFG {
    INT8U node_id;											    // node_id							
    INT8U msgobj_rx;										    // 接收的报文对象
    INT8U msgobj_tx;										    // 发送的报文对象
	INT8U isr_handled;
    INT32U od_const_num;									    // 固定的对象字典个数
    CAN_ODCONSTENTRY *od_const_table;						    // 固定的、只读对象字典（OD）
                                                                // 入口的列表
    INT32U od_num;											    // 对象字典个数
    CAN_ODENTRY *od_table;									    // 对象字典入口列表
}CAN_CANOPENCFG;

/*********************************************************************************************************
  CAN回调函数声明
*********************************************************************************************************/
typedef struct _CAN_CALLBACKS {
    void (*CAN_rx)(INT8U msg_obj);							    //can接收处理函数		 
    void (*CAN_tx)(INT8U msg_obj);
    void (*CAN_error)(INT32U error_info);
    INT32U (*CANOPEN_sdo_read)(INT16U index, INT8U subindex);
    INT32U (*CANOPEN_sdo_write)(INT16U index, INT8U subindex, INT8U *dat_ptr);
    INT32U (*CANOPEN_sdo_seg_read)(INT16U index, INT8U subindex, INT8U openclose,
								  INT8U *length, INT8U *data, INT8U *last);
    INT32U (*CANOPEN_sdo_seg_write)(INT16U index, INT8U subindex, INT8U openclose,
								   INT8U length, INT8U *data, INT8U *fast_resp);
    INT8U (*CANOPEN_sdo_req)(INT8U length_req, INT8U *req_ptr, INT8U *length_resp,
							INT8U *resp_ptr);
}CAN_CALLBACKS;

/*********************************************************************************************************
  CAN固件函数结构体
*********************************************************************************************************/
typedef struct _CAND {
    void (*init_can) (INT32U * can_cfg);						        // 初始化CAN配置
    void (*isr) (void);													// CAN 中断
    void (*config_rxmsgobj) (CAN_MSG_OBJ * msg_obj);					// 接收报文对象配置
    INT8U (*can_receive) (CAN_MSG_OBJ * msg_obj);						// 读取接收报文对象
    void (*can_transmit) (CAN_MSG_OBJ * msg_obj);						// 配置发送报文对象
    void (*config_canopen) (CAN_CANOPENCFG * canopen_cfg);				// CANOpen初始化函数
    void (*canopen_handler) (void);										// CANOpen SDO处理
    void (*config_calb) (CAN_CALLBACKS * callback_cfg);					// 发布回调函数
}CAND;

/*********************************************************************************************************
  ROM中CAN固件地址
*********************************************************************************************************/
typedef	struct _ROM {
   const unsigned p_usbd;
   const unsigned p_clib;
   const CAND * pCANAPI;
   const unsigned p_pwrd;
   const unsigned p_dev1;
   const unsigned p_dev2;
   const unsigned p_dev3;
   const unsigned p_dev4; 
}ROM;

/*********************************************************************************************************
  CAN API 函数声明
*********************************************************************************************************/
extern void CAN_rx(INT8U msg_obj_num);
extern void CAN_tx(INT8U msg_obj_num);
extern void CAN_error(INT32U error_info);
extern INT32U CANOPEN_sdo_exp_read(INT16U index, INT8U subindex);
extern INT32U CANOPEN_sdo_exp_write(INT16U index, INT8U subindex, INT8U *dat_ptr);
extern INT32U CANOPEN_sdo_seg_read(INT16U index, INT8U subindex, INT8U openclose, INT8U *length, INT8U *data, INT8U *last);
extern INT32U CANOPEN_sdo_seg_write(INT16U index, INT8U subindex, INT8U openclose, INT8U length, INT8U *data, INT8U *fast_resp);
extern INT8U CANOPEN_sdo_req(INT8U length_req, INT8U *req_ptr, INT8U *length_resp, INT8U *resp_ptr);


extern const CAN_CALLBACKS callbacks ;                                  // CAN回调函数声明
extern ROM **rom ;	                                                    // 指向固件地址的指针
extern CAN_MSG_OBJ  msg_obj;  	                                        // 定义CAN报文变量
extern CAN_MSG_OBJ  msg_cof1;                                            //定义CAN接收配置
extern CAN_MSG_OBJ  msg_cof2;
extern CAN_MSG_OBJ  msg_cof3;
extern CAN_MSG_OBJ  msg_cof0;

extern CAN_MSG_OBJ  msg_reback;                                         //定义CAN请求返回数据
extern CAN_MSG_OBJ  msg_obj_t;  	                                    // 定义CAN报文变量
extern CAN_CANOPENCFG  myCANopen;                                       // CANOPEN结构体
extern INT32U CanApiClkInitTable_500[];                                     // CAN 波特率配置数组
extern INT32U CanApiClkInitTable_250[];
/*********************************************************************************************************
  对象字典中使用的变量 
*********************************************************************************************************/
extern INT8U  error_register;
extern INT32U device_id;
extern INT32U fw_ver;
extern INT32U param;
#endif
/*********************************************************************************************************
**                                      End Of File
*********************************************************************************************************/
