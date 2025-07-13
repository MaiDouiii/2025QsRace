#include "my_motor_task.h"

SemaphoreHandle_t motor_semphr;

uint8_t now_location = 0;
uint8_t now_user = 0;
uint8_t now_time_period = 0;
uint8_t eat_flag = 1; // 吃药标志位,1表示已经服药,0表示还没服药

/**
 * @brief    电机任务
 * @param    params  ：任务参数
 * @return   无
 * @note     该任务用于控制电机的运动和状态更新。
 */
float real_angel,target_angle = 0,last_error = 0;
//float kp = 0.9,kd = 0.13;
float kp = 3.6,kd = 0.3;
float error_pid,out;
void motor_task(void *params)
{
	uint8_t medicine_num = 0;	
	//medicine_num为对应要服用的药物数量
	while (1)
	{
			if (xSemaphoreTake(motor_semphr, portMAX_DELAY) == pdTRUE)
			{
					for (uint8_t i = 0; i < 6; i++)
					{
							medicine_num = user_messages[now_user].medicine[now_time_period][i];
							if (medicine_num == 0) // 对应要服用的药品数量为0,不取药
							{
									continue;
							}
							target_angle = i * 60.0f;
							Pulse_count = 666; //随便给个值让它进while
							while (Pulse_count != 0)
							{
								vTaskDelay(100); // 等待电机运动完成
							}
							now_location = i; // 更新当前药品位置
							while (medicine_num--) // 取药数量为medicine_num
							{
									motor_take_control();
							}
					}
					//将当前药盒位置now_location写入flash中..
					at24c02_write(0x11,now_location);
					xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
					lv_label_set_text_fmt(ui_Location,"当前正对药盒号:%d号",now_location + 1);
					xSemaphoreGive(lvgl_semphr);
					/*****/
					xSemaphoreGive(check_semphr); // 给检查任务发送信号量，表示电机任务已完成
			}
	}
}
