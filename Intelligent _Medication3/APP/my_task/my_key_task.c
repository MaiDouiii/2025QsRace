#include "my_key_task.h"

float adc_val = 0;
extern lv_obj_t *login_test;

float adc_get_val()
{
	float adc_res = 0;
	for (uint8_t i = 0; i < 10; i++)
	{
		HAL_ADC_Start(&hadc1);
		adc_res += HAL_ADC_GetValue(&hadc1);
	}
	return adc_res / 10.0f;
}

void key_task(void *params)
{
	static uint8_t key_val, key_down, key_old;
	uint16_t timeOut = 20000;
	char wifi_buf[50];
//	uint8_t p_num = 0;	
	while (1)
	{
		key_val = key_read();
		key_down = key_val & (key_val ^ key_old);
		key_old = key_val;
		if(HAL_GPIO_ReadPin(down_en_GPIO_Port,down_en_Pin) == GPIO_PIN_SET)
			last_error = 0;
		real_angel = AS5600_Get_Data() * 360.0f / 4096;
		if(real_angel < 400 && HAL_GPIO_ReadPin(down_en_GPIO_Port,down_en_Pin) == GPIO_PIN_RESET)		//磁编码PID 运算 >400是为了防止错误数据 角度0-359
		{
			error_pid = target_angle - real_angel;
			if(error_pid > 180)
				error_pid = 360 - error_pid;
			else if(error_pid < -180)
				error_pid = error_pid + 360;
			out = kp * error_pid + kd * (error_pid - last_error);
			last_error = error_pid;
			motor_move_step((int16_t)out);
//			if(++p_num >= 50)
//			{
//				p_num = 0;
//				printf("angle:%.1f\r\n",real_angel);
//			}
			
		}
		
		if (key_down == KEY3_DOWN)
		{
			//target_angle = (target_angle >= 300) ? 0: (target_angle = target_angle +60);
			ESP8266_Init();
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
