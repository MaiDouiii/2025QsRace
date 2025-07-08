#include "my_check_task.h"

SemaphoreHandle_t check_semphr;
uint8_t log_num = 0;
char log_message[50] = {0};
char send_str[12];
UINT write_num;
/**
 * @brief    检查状态任务(红外对管检测药仓)
 * @param    params  ：任务参数无
 * @return   无
 * @note     该任务用于检测用户是否取药以及服药状态等。
 *           以防止上一个用户还没服完药就执行下一次的取药操作。
 */
void check_task(void *params)
{
	FIL file;
	while (1) // 检测用户是否服药
	{
		if (xSemaphoreTake(check_semphr, portMAX_DELAY) == pdTRUE)
		{
			uint8_t fail_flag = 0,now_time; //漏服标志
			//播报送药完毕,提醒吃药语音
			HAL_UART_Transmit(&huart6, "E", 1, 10);	
			
			HAL_Delay(4000);
			//人脸识别开始
			Usart_SendString(&huart4,"do",2);
			HAL_Delay(500);
			while(Race_Flag == 0)
			{
					if(Race_Id != 3)
					{
						HAL_UART_Transmit(&huart6, "Q", 1, 10);	//验证身份失败
						HAL_Delay(3000);
						if(Race_Id == 0)
							HAL_UART_Transmit(&huart6, "H", 1, 10);
						else if(Race_Id == 1)
							HAL_UART_Transmit(&huart6, "I", 1, 10);
						else if(Race_Id == 2)
							HAL_UART_Transmit(&huart6, "P", 1, 10);
						HAL_Delay(4000);
					}
					Usart_SendString(&huart4,"do",2);
					HAL_Delay(3000);
			}
			Race_Flag = 0;
			HAL_UART_Transmit(&huart6, "R", 1, 10);	//成功检测到用户
			HAL_Delay(2000);
			Race_Id = 3;
			
			// 红外对管检测药仓被取出
			while (HAL_GPIO_ReadPin(BOTTOM_FORWARD_GPIO_Port, BOTTOM_FORWARD_Pin) == GPIO_PIN_RESET)
			{
					HAL_Delay(5);
			}
			// 红外对管检测药仓被放回,服药完成
			while (HAL_GPIO_ReadPin(BOTTOM_FORWARD_GPIO_Port, BOTTOM_FORWARD_Pin) == GPIO_PIN_SET)
			{
					HAL_Delay(5);
			}
			eat_flag = 1; // 设置吃药标志位为1，表示已经服药
			//播报已经服完药物语音
			HAL_UART_Transmit(&huart6, "D", 1, 10);
			sprintf(send_str,"%02d:%02d:%02d %d",GetTime.Hours,GetTime.Minutes,GetTime.Seconds,now_user);
			ESP8266_UPDataStr(esp8266_Data, ESP8266_Send2, "log",send_str);
			
			sprintf(log_message,"2025-%02d-%02d     %02d:%02d:%02d     用户%d\r\n", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
			if(now_user == 0)
			{
				f_open(&file, "0:/user1/log.txt", FA_OPEN_ALWAYS | FA_WRITE);
				f_lseek(&file, f_size(&file));  // 定位到文件末尾
				f_write(&file, log_message, strlen(log_message), &write_num);
				f_close(&file);
			}
			else if(now_user == 1)
			{
				f_open(&file, "0:/user2/log.txt", FA_OPEN_ALWAYS | FA_WRITE);
				f_lseek(&file, f_size(&file));  // 定位到文件末尾
				f_write(&file, log_message, strlen(log_message), &write_num);
				f_close(&file);
			}
			else
			{
				f_open(&file, "0:/user3/log.txt", FA_OPEN_ALWAYS | FA_WRITE);
				f_lseek(&file, f_size(&file));  // 定位到文件末尾
				f_write(&file, log_message, strlen(log_message), &write_num);
				f_close(&file);
			}
			if(log_num == 0)			//服药日志存储
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log1, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);							
			}
			else if(log_num == 1)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log2, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);	
			}
			else if(log_num == 2)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log3, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);
			}
			else if(log_num == 3)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log4, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);	
			}
			else if(log_num == 4)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log5, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);	
			}
			else if(log_num == 5)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log6, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);	
			}
			else if(log_num == 6)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log7, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);	
			}
			else if(log_num == 7)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log8, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);	
			}
			else if(log_num == 8)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log9, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);	
			}
			if(log_num == 9)
			{
				xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
				lv_label_set_text_fmt(ui_Log10, "2025-%02d-%02d     %02d:%02d:%02d     用户%d", GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds, now_user+1);
				xSemaphoreGive(lvgl_semphr);
				log_num = 0;
			}
			else
			{
				log_num++;
			}
		}
	}
}
