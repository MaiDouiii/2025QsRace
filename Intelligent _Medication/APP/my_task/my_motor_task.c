#include "my_motor_task.h"

SemaphoreHandle_t motor_semphr;

user_medicine user_medicine_data[3] = {0};

uint8_t now_location = 0;

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
    for (uint8_t i = 0; i < 6; i++)
    {
        user_medicine_data[0].medicine[i][0] = 0; // 初始化药品数量为0
    }
    user_medicine_data[0].medicine[0][0] = 2;
    // user_medicine_data[0].medicine[4][0] = 1;
    // user_medicine_data[0].medicine[5][0] = 1;
    // user_medicine_data[0].medicine[3][0] = 1;
    // user_medicine_data[0].medicine[2][0] = 1;
    while (1)
    {
        if (xSemaphoreTake(motor_semphr, portMAX_DELAY) == pdTRUE)
        {
            for (uint8_t i = 0; i < 6; i++)
            {
                medicine_num = user_medicine_data[0].medicine[i][0];
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
        }
    }
}
