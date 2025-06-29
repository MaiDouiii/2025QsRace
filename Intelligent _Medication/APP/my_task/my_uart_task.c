#include "my_uart_task.h"

SemaphoreHandle_t uart_semphr;
uint8_t uart_flag = 0;

void uart_task(void *params)
{
	while(1)
	{
		if(xSemaphoreTake(uart_semphr, portMAX_DELAY) == pdTRUE)
		{
			switch(uart_flag)
			{
				case 2:			//收到来自串口2发送的数据 步进电机
					if(uart_buffer_2[3] == 0 && uart_buffer_2[4] == 0 && uart_buffer_2[5] == 0x6B)
					{
						//printf("stop\r\n");
						motor_stop_flag = 1;
					}
					memset(uart_buffer_2,0,sizeof(uart_buffer_2));
					break;
				case 3:			//收到来自串口3发送的数据 ESP8266
					parse_mqtt_command(esp8266_buf);
					ESP8266_Clear();
					break;
				case 6:			//收到来自串口6发送的数据 ASR-PRO
					if(uart_buffer_6[0] == 0xFF && uart_buffer_6[3] == 0xAF)
					{
						if(uart_buffer_6[1] == 0x11 && uart_buffer_6[2] == 0x22)			//用户1取药
						{
							for (uint8_t j = 0; j < 3; j++) // 三个时间段 早中晚
							{
								if (user_messages[0].send_flag[j] == 1) // 如果该用户的该时间段已经送过药了
								{
									continue; // 跳过，检查下一个时间段
								}
								now_user = 0;					   // 记录当前用户
								now_time_period = j;			   // 记录当前时间段
								user_messages[0].send_flag[j] = 1; // 设置该时间段已送药标志位
								xSemaphoreGive(motor_semphr);	   // 给电机任务发送信号量,开始送药
								break;
							}
						}
						else if(uart_buffer_6[1] == 0x33 && uart_buffer_6[2] == 0x44)	//用户2取药
						{
							for (uint8_t j = 0; j < 3; j++) // 三个时间段 早中晚
							{
								if (user_messages[1].send_flag[j] == 1) // 如果该用户的该时间段已经送过药了
								{
									continue; // 跳过，检查下一个时间段
								}
								now_user = 1;					   // 记录当前用户
								now_time_period = j;			   // 记录当前时间段
								user_messages[1].send_flag[j] = 1; // 设置该时间段已送药标志位
								xSemaphoreGive(motor_semphr);	   // 给电机任务发送信号量,开始送药

								break;
							}
						}
						else if(uart_buffer_6[1] == 0x55 && uart_buffer_6[2] == 0x66)	//用户3取药
						{
							for (uint8_t j = 0; j < 3; j++) // 三个时间段 早中晚
							{
								if (user_messages[2].send_flag[j] == 1) // 如果该用户的该时间段已经送过药了
								{
									continue; // 跳过，检查下一个时间段
								}
								now_user = 2;					  						 // 记录当前用户
								now_time_period = j;			 				  // 记录当前时间段
								user_messages[2].send_flag[j] = 1; 	// 设置该时间段已送药标志位
								xSemaphoreGive(motor_semphr);	 		  // 给电机任务发送信号量,开始送药
								break;
							}
						}
					}
					memset(uart_buffer_6,0,sizeof(uart_buffer_6));
					break;
			}
		}
	}
}

