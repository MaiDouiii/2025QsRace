#include "my_rtc_task.h"

Users user_messages[3] = {0};

/**
 * @brief 判断是否到达用户服药时间前3分钟，到了就给电机任务发送信号量开始取药。
 *
 * 该函数的主要功能是检查当前时间是否接近用户设定的服药时间（提前5分钟）。
 * 如果满足条件，则向电机任务发送信号量，通知其开始执行取药操作。
 *
 * @note 确保系统时间和用户设定的服药时间已正确同步。
 */
void rtc_task(void *params)
{
	uint8_t now_hour, now_minute;	 // second;
	uint8_t now_date, last_date = 0; // 用于记录上一次的日期
	while (1)
	{

		now_hour = GetTime.Hours;
		now_minute = GetTime.Minutes;
		now_date = GetData.Date; // 获取当前日期
		if (now_date != last_date)
		{
			last_date = now_date;			// 更新上一次日期
			for (uint8_t i = 0; i < 3; i++) // 重置用户送药标志位
			{
				for (uint8_t j = 0; j < 3; j++)
				{
					user_messages[i].send_flag[j] = 0;
				}
			}
		}
		// 判断是否到达服药时间,到达就给电机任务发送信号量开始取药
		for (uint8_t i = 0; i < 3; i++) // 用户号 0-1-2
		{
			if (eat_flag == 0) // 送完药后,吃药标志位为0,表示还没服药,这时候不能继续判断/送药
				break;
			for (uint8_t j = 0; j < 3; j++) // 三个时间段 早中晚
			{
				if (user_messages[i].send_flag[j] == 1) // 如果该用户的该时间段已经送过药了
				{
					continue; // 跳过，检查下一个时间段
				}
				if ((user_messages[i].Time[j][0] == now_hour && user_messages[i].Time[j][1] == now_minute + 3) || (user_messages[i].Time[j][0] == now_hour + 1 && now_minute - user_messages[i].Time[j][1] == 57)) // 到达指定服药时间前3分钟
				{
					HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
					now_user = i;					   // 记录当前用户
					now_time_period = j;			   // 记录当前时间段
					user_messages[i].send_flag[j] = 1; // 设置该时间段已送药标志位
					xSemaphoreGive(motor_semphr);	   // 给电机任务发送信号量,开始送药
					if (now_user == 0)
					{
						HAL_UART_Transmit(&huart6, "A", 1, 10);
					}
					else if (now_user == 1)
					{
						HAL_UART_Transmit(&huart6, "B", 1, 10);
					}
					else
					{
						HAL_UART_Transmit(&huart6, "C", 1, 10);
					}
					eat_flag = 0; // 已经送药完毕,设置吃药标志位为0，表示还没服药
					break;
				}
			}
		}
		vTaskDelay(1000);
	}
}
