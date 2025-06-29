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
void motor_task(void *params)
{
    int8_t move_num = 0;
    uint8_t move_dir = 0, medicine_num = 0;
    //		user_messages[0].medicine[0][1] = 1;
    //		user_messages[0].medicine[0][2] = 1;
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
                move_num = (now_location - i); // 计算当前药品位置与目标药品位置之间的差值
                if (move_num < 0)
                {
                    if (move_num < -3)
                    {
                        move_dir = Clockwise;
                        move_num = 6 + move_num; // 取绝对值
                    }
                    else
                    {
                        move_dir = CounterClockwise;
                        move_num = -move_num; // 取绝对值
                    }
                }
                else
                {
                    if (move_num > 3)
                    {
                        move_dir = CounterClockwise;
                        move_num = 6 - move_num; // 取绝对值
                    }
                    else
                    {
                        move_dir = Clockwise;
                    }
                }
                now_location = i; // 更新当前药品位置

                if (move_num > 0)
                {
                    motor_down_control(move_dir, move_num); // 控制电机运动到指定药盘位置
                    while (Pulse_count != 0)
                    {
                        vTaskDelay(100); // 等待电机运动完成
                    }
                }

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
