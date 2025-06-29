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
#include "ff.h"
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
#include "my_uart_task.h"
#include "iic.h"
#include "dht11.h"
#include "esp8266.h"
#include "DataHandle.h"

extern SemaphoreHandle_t uart_semphr;
extern SemaphoreHandle_t lvgl_semphr;
extern SemaphoreHandle_t motor_semphr;
extern SemaphoreHandle_t check_semphr;

extern RTC_DateTypeDef GetData;
extern RTC_TimeTypeDef GetTime;

extern Users user_messages[3];
extern uint8_t uart_buffer_2[20];
extern uint8_t uart_buffer_6[20];
extern uint8_t motorBottom_turn, motorTop_turn;
extern uint16_t Pulse_count;

extern char esp8266_buf[300];  // 串口接受数组
extern char esp8266_Data[200]; // 上报数据的包

extern char wifi_name[20];
extern char wifi_pass[20];

extern const char *ESP8266_Send1;
extern unsigned short esp8266_cnt;
extern uint8_t esp8266_flag;
extern uint8_t uart_flag;
extern uint8_t temp, humi;
extern uint8_t now_location;
extern uint8_t now_user;
extern uint8_t now_time_period;
extern uint8_t motor_stop_flag;
extern uint8_t login_flag;
extern uint8_t eat_flag;

#endif
