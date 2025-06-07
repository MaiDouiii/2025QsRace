#include "my_lvgl_task.h"

void timer_cb(lv_timer_t *timer)
{
	HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
	lv_label_set_text_fmt(ui_TimeLabel, "%d:%d:%d", GetTime.Hours, GetTime.Minutes, GetTime.Seconds);
}

////lv_obj_t *bg_image;
////const char *p1 ="S:/images/dy.bin";
//void safe_switch_image(lv_obj_t *img, const void *new_src) {
//    const void *old_src = lv_img_get_src(img);
//    
//    // 1. 解除图片对象与旧资源的关联
//    lv_img_set_src(img, NULL);  // 重要：先解除绑定
//    
//    // 2. 清除缓存
//    if (old_src) {
//        lv_img_cache_invalidate_src(old_src);
//    }
//    
////    // 3. 获取旧资源的解码器描述符并关闭
////    lv_img_decoder_dsc_t old_dsc;
////    lv_color_t bg_color = lv_color_white(); // 根据背景色调整
////    if (lv_img_decoder_open(&old_dsc, old_src, bg_color,0) == LV_RES_OK) {
////        lv_img_decoder_close(&old_dsc); // 正确关闭解码器
////    }
////    
//    // 4. 设置新资源
//    lv_img_set_src(img, new_src);
//    lv_obj_center(img);
////	lv_label_set_text_fmt(ui_Label11, "%d/12",image_num);
//    // 5. 强制刷新显示
//    lv_refr_now(NULL);
//	
//}
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
