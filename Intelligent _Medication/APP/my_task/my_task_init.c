#include "my_task_init.h"

void timer_cb(lv_timer_t *timer); // lvgl定时器回调函数,更新显示的时间和日期

void lvgl_task(void *params);
TaskHandle_t lvgl_handler;
#define lvgl_priority 8
#define lvgl_stack_size 2048

void key_task(void *params);
TaskHandle_t key_handler;
#define key_priority 7
#define key_stack_size 128

void rtc_task(void *params);
TaskHandle_t rtc_handler;
#define rtc_priority 10
#define rtc_stack_size 128

void motor_task(void *params);
TaskHandle_t motor_handler;
#define motor_priority 11
#define motor_stack_size 256

void uart_task(void *params);
TaskHandle_t uart_handler;
#define uart_priority 12
#define uart_stack_size 256	//384

void check_task(void *params);
TaskHandle_t check_handler;
#define check_priority 6
#define check_stack_size 512

void all_task_init()
{
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();

	ui_init();
	HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
	now_location = at24c02_read(0x11);
	lv_label_set_text_fmt(ui_Location,"当前正对药盒号:%d号",now_location + 1);
	lv_label_set_text_fmt(ui_DataLabel, "%d/%d/%d   Day%d", GetData.Year, GetData.Month, GetData.Date, GetData.WeekDay);
	lv_label_set_text_fmt(ui_TimeLabel, "%02d:%02d:%02d", GetTime.Hours, GetTime.Minutes, GetTime.Seconds);
	lv_timer_create(timer_cb, 200, NULL);
	

	// 更新一次时间防止首次上电显示错误

	xTaskCreate(lvgl_task,
				"LVGL_Task",
				lvgl_stack_size,
				NULL,
				lvgl_priority,
				&lvgl_handler);

	xTaskCreate(key_task,
				"Key_Task",
				key_stack_size,
				NULL,
				key_priority,
				&key_handler);

	xTaskCreate(rtc_task,
				"RTC_Task",
				rtc_stack_size,
				NULL,
				rtc_priority,
				&rtc_handler);

	xTaskCreate(motor_task,
				"Motor_Task",
				motor_stack_size,
				NULL,
				motor_priority,
				&motor_handler);

	xTaskCreate(uart_task,
				"Uart_Task",
				uart_stack_size,
				NULL,
				uart_priority,
				&uart_handler);

	xTaskCreate(check_task,
				"Check_Task",
				check_stack_size,
				NULL,
				check_priority,
				&check_handler);
}
