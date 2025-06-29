#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "bsp_system.h"

#define REV_OK		0	//½ÓÊÕÍê³É±êÖ¾
#define REV_WAIT	1	//½ÓÊÕÎ´Íê³É±êÖ¾

#define USART_DEBUG		 &huart1		//µ÷ÊÔ´òÓ¡ËùÊ¹ÓÃµÄ´®¿Ú×é

extern int DB_Flag;//ä¸‹å‘å‘½ä»¤çš„æ ‡å¿—

void ESP8266_Init(void);//8266åˆå§‹åŒ–
void ESP8266_Clear(void);//æ¸…ç©ºæ¥å—æ•°ç»„

_Bool ESP8266_SendCmd(const char *cmd, char *res);

void ESP8266_UPDataNum(char * buf,const char * pack,const char * Device,int num);//ä¸ŠæŠ¥æ•°å­—æ•°æ®
void ESP8266_UPDataStr(char * buf,const char * pack,const char * Device,char * str);//ä¸ŠæŠ¥å­—ç¬¦æ•°æ®

void Usart_SendString(UART_HandleTypeDef *huart,  unsigned char *str,uint16_t Size);
_Bool ESP8266_WaitRecive(void);

#endif
