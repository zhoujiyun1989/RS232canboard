#include "buffer.h"

/*********************************************************************************************************
** Function name:       new_can_buffer
** Descriptions:        新建can buffer
** input parameters:    size--buffer的大小
** output parameters:   无
** Returned value:      返回的数据为1位成功，0为失败
*********************************************************************************************************/
T_CAN_BUFFER new_can_buffer(size_t_can size)
{
	//CAN_MSG_OBJ frame_buffer[BUFFER_SIZE];
	T_CAN_BUFFER buffer;
	buffer.size = 1000;
	buffer.over = 0;
	buffer.next = 0;
	buffer.oldest = 0;
	//buffer.pframe = (CAN_MSG_OBJ *)malloc(sizeof(CAN_MSG_OBJ) * 100);
	return buffer;
}


/*********************************************************************************************************
** Function name:       get_can_buffer
** Descriptions:        从buffer取数
** input parameters:    T_can_BUFFER
** output parameters:   pframe
** Returned value:      返回的数据为1成功，0失败
*********************************************************************************************************/
int get_can_buffer(T_CAN_BUFFER* pbuffer, CAN_MSG_OBJ* pframe)
{
	if(is_can_buffer_empty(pbuffer))
		return 0;
	else
	{
		*pframe = *(pbuffer->pframe + pbuffer->oldest);
		pbuffer->oldest = (pbuffer->oldest + 1) % pbuffer->size;
		return 1;
	}
}

/*********************************************************************************************************
** Function name:       put_can_buffer
** Descriptions:        向buffer存数
** input parameters:    pframe
** output parameters:   T_can_BUFFER
** Returned value:      返回的数据为1成功，0为失败
*********************************************************************************************************/
int put_can_buffer(T_CAN_BUFFER* pbuffer, CAN_MSG_OBJ* pframe)
{
	if(is_can_buffer_full(pbuffer))
	{
		(pbuffer->over)++;
		return 0;
	}
	else
	{
		*(pbuffer->pframe + pbuffer->next) = *pframe;
		pbuffer->next = (pbuffer->next + 1) % pbuffer->size;
		return 1;
	}
}

/*********************************************************************************************************
** Function name:       is_can_buffer_empty
** Descriptions:        判断buffer是否为空
** input parameters:    T_can_BUFFER
** output parameters:   
** Returned value:      返回的数据为1 空，0 不空
*********************************************************************************************************/
int is_can_buffer_empty(T_CAN_BUFFER* buffer)
{
	if(buffer->oldest==buffer->next)
		return 1;
	else
		return 0;
}

/*********************************************************************************************************
** Function name:       is_can_buffer_full
** Descriptions:        判断buffer是否为满
** input parameters:    T_can_BUFFER
** output parameters:   
** Returned value:      返回的数据为1 满，0不满
*********************************************************************************************************/
int is_can_buffer_full(T_CAN_BUFFER* buffer)
{
	if((buffer->next + 1)%buffer->size==buffer->oldest)
		return 1;
	else
		return 0; 
}

T_CAN_BUFFER  rcv_can_buffer = {
	CAN_BUFFER_SIZE,
	0,
	0,
	0,
	0x00,
};


/*********************************************************************************************************
** Function name:       new_byte_buffer
** Descriptions:        新建byte buffer
** input parameters:    size--buffer的大小
** output parameters:   无
** Returned value:      返回的数据为1位成功，0为失败
*********************************************************************************************************/
T_BYTE_BUFFER new_byte_buffer(size_t_byte size)
{
	T_BYTE_BUFFER buffer;
	buffer.size = size;
	buffer.over = 0;
	buffer.next = 0;
	buffer.oldest = 0;
	return buffer;
}

/*********************************************************************************************************
** Function name:       get_byte_buffer
** Descriptions:        从buffer取数
** input parameters:    T_BYTE_BUFFER
** output parameters:   pbyte
** Returned value:      返回的数据为1成功，0失败
*********************************************************************************************************/
int get_byte_buffer(T_BYTE_BUFFER* pbuffer, char* pbyte)
{
	if(is_byte_buffer_empty(pbuffer))
		return 0;
	else
	{
		*pbyte = *(pbuffer->byte + pbuffer->oldest);
		pbuffer->oldest = (pbuffer->oldest + 1) % pbuffer->size;
		return 1;
	}
}

/*********************************************************************************************************
** Function name:       put_byte_buffer
** Descriptions:        向buffer存数
** input parameters:    pbyte
** output parameters:   T_BYTE_BUFFER
** Returned value:      返回的数据为1成功，0为失败
*********************************************************************************************************/
int put_byte_buffer(T_BYTE_BUFFER* pbuffer, char* pbyte)
{
	if(is_byte_buffer_full(pbuffer))
	{
		(pbuffer->over)++;
		return 0;
	}
	else
	{
		*(pbuffer->byte + pbuffer->next) = *pbyte;
		pbuffer->next = (pbuffer->next + 1) % pbuffer->size;
		return 1;
	}
}

/*********************************************************************************************************
** Function name:       is_byte_buffer_empty
** Descriptions:        判断buffer是否为空
** input parameters:    T_BYTE_BUFFER
** output parameters:   
** Returned value:      返回的数据为1 空，0 不空
*********************************************************************************************************/
int is_byte_buffer_empty(T_BYTE_BUFFER* buffer)
{
	if(buffer->oldest==buffer->next)
		return 1;
	else
		return 0;
}

/*********************************************************************************************************
** Function name:       is_byte_buffer_full
** Descriptions:        判断buffer是否为满
** input parameters:    T_CAN_BUFFER
** output parameters:   
** Returned value:      返回的数据为1 满，0不满
*********************************************************************************************************/
int is_byte_buffer_full(T_BYTE_BUFFER* buffer)
{
	if((buffer->next + 1)%buffer->size==buffer->oldest)
		return 1;
	else
		return 0; 
}

T_BYTE_BUFFER  rcv_byte_buffer = {
	BYTE_BUFFER_SIZE,
	0,
	0,
	0,
	//{0x00},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};



/*********************************************************************************************************
** Function name:       new_arry_buffer
** Descriptions:        新建arry buffer
** input parameters:    size--buffer的大小
** output parameters:   无
** Returned value:      返回的数据为1位成功，0为失败
*********************************************************************************************************/
T_ARRY_BUFFER new_arry_buffer(size_t_arry size)
{
	T_ARRY_BUFFER buffer;
	buffer.size = size;
	buffer.over = 0;
	buffer.next = 0;
	buffer.oldest = 0;
	return buffer;
}

/*********************************************************************************************************
** Function name:       get_arry_buffer
** Descriptions:        从buffer取数
** input parameters:    T_ARRY_BUFFER
** output parameters:   parry[9]
** Returned value:      返回的数据为1成功，0失败
*********************************************************************************************************/
int get_arry_buffer(T_ARRY_BUFFER* pbuffer, char* parry)
{
	int i = 0;
	if(is_arry_buffer_empty(pbuffer))
		return 0;
	else
	{
		for(i=0;i<9;i++)
		{
		*(parry+i) = *(*(pbuffer->arry + pbuffer->oldest)+i);
		}
		pbuffer->oldest = (pbuffer->oldest + 1) % pbuffer->size;
		return 1;
	}
}

/*********************************************************************************************************
** Function name:       put_arry_buffer
** Descriptions:        向buffer存数
** input parameters:    parry
** output parameters:   T_ARRY_BUFFER
** Returned value:      返回的数据为1成功，0为失败
*********************************************************************************************************/
int put_arry_buffer(T_ARRY_BUFFER* pbuffer, char* parry)
{
	int i = 0;
	if(is_arry_buffer_full(pbuffer))
	{
		(pbuffer->over)++;
		return 0;
	}
	else
	{
		for(i=0;i<9;i++)
		{
			*(*(pbuffer->arry + pbuffer->oldest)+i) = *(parry+i);
		}
		pbuffer->next = (pbuffer->next + 1) % pbuffer->size;
		return 1;
	}
}

/*********************************************************************************************************
** Function name:       is_arry_buffer_empty
** Descriptions:        判断buffer是否为空
** input parameters:    T_ARRY_BUFFER
** output parameters:   
** Returned value:      返回的数据为1 空，0 不空
*********************************************************************************************************/
int is_arry_buffer_empty(T_ARRY_BUFFER* buffer)
{
	if(buffer->oldest==buffer->next)
		return 1;
	else
		return 0;
}

/*********************************************************************************************************
** Function name:       is_arry_buffer_full
** Descriptions:        判断buffer是否为满
** input parameters:    T_arry_BUFFER
** output parameters:   
** Returned value:      返回的数据为1 满，0不满
*********************************************************************************************************/
int is_arry_buffer_full(T_ARRY_BUFFER* buffer)
{
	if((buffer->next + 1)%buffer->size==buffer->oldest)
		return 1;
	else
		return 0; 
}

T_ARRY_BUFFER  rcv_arry_buffer = {
	ARRY_BUFFER_SIZE,
	0,
	0,
	0,
	0x00,
};
