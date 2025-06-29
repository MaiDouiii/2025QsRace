#include "my_check_task.h"

SemaphoreHandle_t check_semphr;
uint8_t log_num = 0;

/**
 * @brief    检查状态任务(红外对管检测药仓)
 * @param    params  ：任务参数无
 * @return   无
 * @note     该任务用于检测用户是否取药以及服药状态等。
 *           以防止上一个用户还没服完药就执行下一次的取药操作。
 */
void check_task(void *params)
{
    while (1) // 检测用户是否服药
    {
        if (xSemaphoreTake(check_semphr, portMAX_DELAY) == pdTRUE)
        {
						HAL_UART_Transmit(&huart6, "E", 1, 10);
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
            HAL_UART_Transmit(&huart6, "D", 1, 10);
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
