#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "bsp_system.h"

#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־

#define USART_DEBUG		 &huart1		//���Դ�ӡ��ʹ�õĴ�����

extern int DB_Flag;//下发命令的标志

void ESP8266_Init(void);//8266初始化
void ESP8266_Clear(void);//清空接受数组

_Bool ESP8266_SendCmd(const char *cmd, char *res);

void ESP8266_UPDataNum(char * buf,const char * pack,const char * Device,int num);//上报数字数据
void ESP8266_UPDataStr(char * buf,const char * pack,const char * Device,char * str);//上报字符数据

void Usart_SendString(UART_HandleTypeDef *huart,  unsigned char *str,uint16_t Size);
_Bool ESP8266_WaitRecive(void);

#endif
