#include "my_rtc_task.h"

typedef struct
{
    uint8_t Hours;
    uint8_t Minutes;
    uint8_t Seconds;
} User_Time;

User_Time user_timer[3] = {0};

/**
 * @brief 判断是否到达用户服药时间前5分钟，到了就给电机任务发送信号量开始取药。
 *
 * 该函数的主要功能是检查当前时间是否接近用户设定的服药时间（提前5分钟）。
 * 如果满足条件，则向电机任务发送信号量，通知其开始执行取药操作。
 *
 * @note 确保系统时间和用户设定的服药时间已正确同步。
 */
void rtc_task(void *params)
{
    uint8_t hour, minute, second;
    while (1)
    {
        hour = GetTime.Hours;
        minute = GetTime.Minutes;
        second = GetTime.Seconds;

        vTaskDelay(1000);
    }
}
