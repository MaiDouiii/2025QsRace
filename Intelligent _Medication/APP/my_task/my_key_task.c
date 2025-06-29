#include "my_key_task.h"

extern lv_obj_t *login_test;

void key_task(void *params)
{
	static uint8_t key_val, key_down, key_old;
	uint16_t timeOut = 8000;
	char wifi_buf[50];
	while (1)
	{
		key_val = key_read();
		key_down = key_val & (key_val ^ key_old);
		key_old = key_val;

		if (key_down == KEY3_DOWN)
		{
				ESP8266_Init();
				//vTaskDelete(NULL);
		}
		if(login_flag == 1)
		{
			
			sprintf(wifi_buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_name,wifi_pass);
			Usart_SendString(&huart3, (unsigned char *)wifi_buf, strlen((const char *)wifi_buf));
			while (timeOut--)
			{
				if(ESP8266_WaitRecive() == REV_OK)
				{
					if(strstr((const char *)esp8266_buf, "OK") != NULL)
					{
						ESP8266_Clear();
						login_flag = 0;
						printf("WIFI CONNECTED!\r\n");
						break;
					}
				}
				delay_us(100);
			}
			if(login_flag == 0)
			{
					
					xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
					lv_label_set_text(login_test,"\n        Login Success!");
					xSemaphoreGive(lvgl_semphr);				
			}
			else
			{
					xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
					lv_label_set_text(login_test,"\n        Login Fail!");
					xSemaphoreGive(lvgl_semphr);	
			}
			login_flag = 0;
		}
		vTaskDelay(10);
	}
}
