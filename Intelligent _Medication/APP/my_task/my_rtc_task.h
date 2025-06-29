#ifndef __my_rtc_task_H__
#define __my_rtc_task_H__

#include "main.h"
typedef struct
{
    int medicine[3][6]; // 三个时间段对应每种药物数量 (6个药盒)
    uint8_t send_flag[3]; // 用户送药标志位,防止重复送药
    int Time[3][2];   // 三个时间段对应的时-分 09:30  13:30  19:30(早上,中午,晚上的时分秒)
} Users;
#include "bsp_system.h"





#endif
