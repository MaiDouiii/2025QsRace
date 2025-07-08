#include "my_lvgl_task.h"

void timer_cb(lv_timer_t *timer)
{
	HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
	lv_label_set_text_fmt(ui_TimeLabel, "%d:%d:%d", GetTime.Hours, GetTime.Minutes, GetTime.Seconds);
}

void lvgl_task(void *params)
{
//	lv_fs_if_init();
//	bg_image = lv_img_create(ui_SDCard);
//	lv_img_set_src(bg_image, "S:/images/img1.bin");
//	safe_switch_image(bg_image,p1);

	while (1)
	{
		xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
		tp_dev.scan(0); // 此函数里有进入临界区
		lv_timer_handler();
		xSemaphoreGive(lvgl_semphr);
		vTaskDelay(5);
	}
}
