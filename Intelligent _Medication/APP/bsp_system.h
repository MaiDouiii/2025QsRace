#ifndef __bsp_system_H__
#define __bsp_system_H__

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "freertos.h"
#include "semphr.h"
#include "rtc.h"
#include "usart.h"
#include "queue.h"
#include "task.h"
#include "tim.h"

#include "SDCARD.h"
#include "ui.h"
#include "delay.h"
#include "touch.h"
#include "key.h"
#include "motor.h"
#include "driver_ring.h"
#include "my_lvgl_task.h"
#include "my_key_task.h"
#include "my_rtc_task.h"
#include "my_motor_task.h"
#include "my_task_init.h"
#include "iic.h"

extern SemaphoreHandle_t lvgl_semphr;
extern SemaphoreHandle_t motor_semphr;

extern RTC_DateTypeDef GetData;
extern RTC_TimeTypeDef GetTime;

extern uint8_t uart_buffer[50];
extern uint8_t uart_buffer_6[10];
extern uint8_t motorBottom_turn, motorTop_turn;
extern uint16_t Pulse_count;

#endif
