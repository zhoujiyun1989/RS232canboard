/********************************************************************************************************
--Design name:          buffer.h
--Target devices:       TinyM0-CAN-T
--Create data:          2014-3-31
--Revision:             v1.0
--Copyright(c):         raja
--Additional comments:  
*********************************************************************************************************/
#ifndef __BUFFER_H 
#define __BUFFER_H
#include "CAN_API.h"
#define		CAN_BUFFER_SIZE		100
#define		BYTE_BUFFER_SIZE	200
#define		ARRY_BUFFER_SIZE	200


typedef		unsigned int		size_t_can;
typedef		unsigned int		size_t_byte;
typedef		unsigned int		size_t_arry;

typedef 	struct{
	size_t_can size;
	size_t_can over;
	size_t_byte next;
	size_t_byte oldest;
	CAN_MSG_OBJ pframe[CAN_BUFFER_SIZE];
}T_CAN_BUFFER;
typedef 	struct{
	size_t_byte size;
	size_t_byte over;
	size_t_byte next;
	size_t_byte oldest;
	uint8_t byte[BYTE_BUFFER_SIZE];
}T_BYTE_BUFFER;
typedef 	struct{
	size_t_arry size;
	size_t_arry over;
	size_t_arry next;
	size_t_arry oldest;
	uint8_t arry[BYTE_BUFFER_SIZE][9];
}T_ARRY_BUFFER;

extern T_CAN_BUFFER new_can_buffer(size_t_can size);
extern int get_can_buffer(T_CAN_BUFFER* pbuffer, CAN_MSG_OBJ* pframe);
extern int put_can_buffer(T_CAN_BUFFER* pbuffer, CAN_MSG_OBJ* pframe);
extern int is_can_buffer_empty(T_CAN_BUFFER* pbuffer);
extern int is_can_buffer_full(T_CAN_BUFFER* pbuffer);
extern T_CAN_BUFFER  rcv_can_buffer;

extern T_BYTE_BUFFER new_byte_buffer(size_t_byte size);
extern int get_byte_buffer(T_BYTE_BUFFER* pbuffer, char* pbyte);
extern int put_byte_buffer(T_BYTE_BUFFER* pbuffer, char* pbyte);
extern int is_byte_buffer_empty(T_BYTE_BUFFER* pbuffer);
extern int is_byte_buffer_full(T_BYTE_BUFFER* pbuffer);
extern T_BYTE_BUFFER  rcv_byte_buffer;

extern T_ARRY_BUFFER new_arry_buffer(size_t_arry size);
extern int get_arry_buffer(T_ARRY_BUFFER* pbuffer, char* parry);
extern int put_arry_buffer(T_ARRY_BUFFER* pbuffer, char* parry);
extern int is_arry_buffer_empty(T_ARRY_BUFFER* pbuffer);
extern int is_arry_buffer_full(T_ARRY_BUFFER* pbuffer);
extern T_ARRY_BUFFER  rcv_arry_buffer;
#endif