#include "buffer.h"

/*********************************************************************************************************
** Function name:       new_can_buffer
** Descriptions:        �½�can buffer
** input parameters:    size--buffer�Ĵ�С
** output parameters:   ��
** Returned value:      ���ص�����Ϊ1λ�ɹ���0Ϊʧ��
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
** Descriptions:        ��bufferȡ��
** input parameters:    T_can_BUFFER
** output parameters:   pframe
** Returned value:      ���ص�����Ϊ1�ɹ���0ʧ��
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
** Descriptions:        ��buffer����
** input parameters:    pframe
** output parameters:   T_can_BUFFER
** Returned value:      ���ص�����Ϊ1�ɹ���0Ϊʧ��
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
** Descriptions:        �ж�buffer�Ƿ�Ϊ��
** input parameters:    T_can_BUFFER
** output parameters:   
** Returned value:      ���ص�����Ϊ1 �գ�0 ����
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
** Descriptions:        �ж�buffer�Ƿ�Ϊ��
** input parameters:    T_can_BUFFER
** output parameters:   
** Returned value:      ���ص�����Ϊ1 ����0����
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
** Descriptions:        �½�byte buffer
** input parameters:    size--buffer�Ĵ�С
** output parameters:   ��
** Returned value:      ���ص�����Ϊ1λ�ɹ���0Ϊʧ��
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
** Descriptions:        ��bufferȡ��
** input parameters:    T_BYTE_BUFFER
** output parameters:   pbyte
** Returned value:      ���ص�����Ϊ1�ɹ���0ʧ��
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
** Descriptions:        ��buffer����
** input parameters:    pbyte
** output parameters:   T_BYTE_BUFFER
** Returned value:      ���ص�����Ϊ1�ɹ���0Ϊʧ��
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
** Descriptions:        �ж�buffer�Ƿ�Ϊ��
** input parameters:    T_BYTE_BUFFER
** output parameters:   
** Returned value:      ���ص�����Ϊ1 �գ�0 ����
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
** Descriptions:        �ж�buffer�Ƿ�Ϊ��
** input parameters:    T_CAN_BUFFER
** output parameters:   
** Returned value:      ���ص�����Ϊ1 ����0����
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
** Descriptions:        �½�arry buffer
** input parameters:    size--buffer�Ĵ�С
** output parameters:   ��
** Returned value:      ���ص�����Ϊ1λ�ɹ���0Ϊʧ��
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
** Descriptions:        ��bufferȡ��
** input parameters:    T_ARRY_BUFFER
** output parameters:   parry[9]
** Returned value:      ���ص�����Ϊ1�ɹ���0ʧ��
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
** Descriptions:        ��buffer����
** input parameters:    parry
** output parameters:   T_ARRY_BUFFER
** Returned value:      ���ص�����Ϊ1�ɹ���0Ϊʧ��
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
** Descriptions:        �ж�buffer�Ƿ�Ϊ��
** input parameters:    T_ARRY_BUFFER
** output parameters:   
** Returned value:      ���ص�����Ϊ1 �գ�0 ����
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
** Descriptions:        �ж�buffer�Ƿ�Ϊ��
** input parameters:    T_arry_BUFFER
** output parameters:   
** Returned value:      ���ص�����Ϊ1 ����0����
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
