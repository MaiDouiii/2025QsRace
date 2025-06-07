//#include "SDcard.h"
//#include "lvgl/lvgl.h"
//#include "ff.h"  // FATFS 头文件

//// 文件系统打开回调函数
//static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
//    FIL *fil = lv_mem_alloc(sizeof(FIL));
//    BYTE fatfs_mode = (mode == LV_FS_MODE_WR) ? FA_WRITE | FA_READ : FA_READ;
//    FRESULT res = f_open(fil, path, fatfs_mode);
//    if (res != FR_OK) {
//        lv_mem_free(fil);
//        return NULL;
//    }
//    return fil;
//}

//// 文件系统读取回调函数
//static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br) {
//    FIL *fil = (FIL *)file_p;
//    FRESULT res = f_read(fil, buf, btr, (UINT *)br);
//    return (res == FR_OK) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
//}

//// 文件系统关闭回调函数
//static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p) {
//    FIL *fil = (FIL *)file_p;
//    f_close(fil);
//    lv_mem_free(file_p);
//    return LV_FS_RES_OK;
//}

//// 文件系统定位回调函数
//static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos) {
//    FIL *fil = (FIL *)file_p;
//    f_lseek(fil, pos);
//    return LV_FS_RES_OK;
//}

//void lvgl_fs_init(void) {
//    lv_fs_drv_t fs_drv;
//    lv_fs_drv_init(&fs_drv);

//    // 配置驱动参数
//    fs_drv.letter = 'S';         // 驱动器标识符（例如 "S:/image.bin"）
//    fs_drv.open_cb = fs_open;
//    fs_drv.read_cb = fs_read;
//    fs_drv.seek_cb = fs_seek;
//    fs_drv.close_cb = fs_close;

//    lv_fs_drv_register(&fs_drv); // 注册驱动
//}

#include "lvgl/lvgl.h"
#include "ff.h"
#include "semphr.h"
#include "stdio.h"

static FATFS fs;  // FATFS文件系统对象

// 文件系统互斥锁（FreeRTOS）
static SemaphoreHandle_t fs_mutex;

/***********************
 *  STATIC PROTOTYPES
 **********************/
static bool fs_ready(lv_fs_drv_t * drv);
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_fs_drv_t fs_drv;

/*---------------------------------------------------
 * 文件系统初始化
 *--------------------------------------------------*/
void lv_fs_if_init(void)
{
    /* 初始化FATFS */
    FRESULT res = f_mount(&fs, "", 1);  // ""表示默认驱动器
    if(res != FR_OK) {
        printf("FATFS mount error: %d\n", res);
        return;
    }
	
    /* 创建互斥锁 */
    fs_mutex = xSemaphoreCreateMutex();

    /* 注册LVGL驱动 */
    lv_fs_drv_init(&fs_drv);

    /* 设置驱动参数 */
    fs_drv.letter = 'S';                // 驱动器字母（例如"S:/picture.bin"）
    fs_drv.cache_size = 64;            // 缓存大小（KB）
    fs_drv.ready_cb = fs_ready;        // 设备就绪回调
    fs_drv.open_cb = fs_open;          // 打开文件
    fs_drv.close_cb = fs_close;        // 关闭文件
    fs_drv.read_cb = fs_read;          // 读取文件
    fs_drv.seek_cb = fs_seek;          // 文件定位
    fs_drv.tell_cb = fs_tell;          // 获取位置
	//printf("Success! \r\n");
    lv_fs_drv_register(&fs_drv);
	
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* 检查文件系统是否就绪 */
static bool fs_ready(lv_fs_drv_t * drv)
{
    return (f_mount(&fs, "", 0) == FR_OK); // 检查挂载状态
}

/* 打开文件 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
//	printf("[FS] Opening: %s\n", path); // 添加路径打印
    xSemaphoreTake(fs_mutex, portMAX_DELAY);

    FIL * fp = lv_mem_alloc(sizeof(FIL));
    if(fp == NULL) return NULL;

    BYTE fatfs_mode = 0;
    if(mode == LV_FS_MODE_RD) 
        fatfs_mode = FA_READ;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) 
        fatfs_mode = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;

    FRESULT res = f_open(fp, path, fatfs_mode);
    if(res != FR_OK) {
        lv_mem_free(fp);
        xSemaphoreGive(fs_mutex);
        return NULL;
    }
//printf("[FS] Open success! File handle: %p\n", fp);
    xSemaphoreGive(fs_mutex);
    return fp;
}

/* 关闭文件 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    xSemaphoreTake(fs_mutex, portMAX_DELAY);
    
    FIL * fp = (FIL *)file_p;
    FRESULT res = f_close(fp);
    lv_mem_free(fp);  // 释放内存
    
    xSemaphoreGive(fs_mutex);
    return (res == FR_OK) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

/* 读取文件 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
//	    FIL * fp = (FIL *)file_p;
//    UINT br_tmp;
//    FRESULT res = f_read(fp, buf, btr, &br_tmp);
//    
//    // 添加数据头打印
//    uint8_t * p = (uint8_t *)buf;
//    printf("[FS] Read %u bytes. Header: 0x%02X 0x%02X 0x%02X 0x%02X\n", 
//          br_tmp, p[0], p[1], p[2], p[3]);
//    
//    *br = br_tmp;
//    return (res == FR_OK) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
    FIL * fp = (FIL *)file_p;
    UINT br_tmp = 0;
    FRESULT res = f_read(fp, buf, btr, &br_tmp);
    *br = br_tmp;
    return (res == FR_OK) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

/* 文件定位 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    FIL * fp = (FIL *)file_p;
    DWORD offset = pos;
    switch(whence) {
    case LV_FS_SEEK_SET: break;
    case LV_FS_SEEK_CUR: offset += f_tell(fp); break;
    case LV_FS_SEEK_END: offset = f_size(fp) - pos; break;
    }
    
    FRESULT res = f_lseek(fp, offset);
    return (res == FR_OK) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

/* 获取当前文件位置 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    FIL * fp = (FIL *)file_p;
    *pos_p = f_tell(fp);
    return LV_FS_RES_OK;
}

void test_sd_card(void) {
    FATFS fs;
    FIL fil;
    UINT bytesread;
    char buffer[100];
    
    // 挂载文件系统
    if (f_mount(&fs, "", 1) != FR_OK) { // "" 表示默认驱动噿
        printf("Mount failed!\n");
        return;
    }
    
    // 尝试打开文件
    if (f_open(&fil, "test.txt", FA_READ) != FR_OK) {
        printf("Open file failed!\n");
        f_mount(NULL, "", 0); // 卸载
        return;
    }
    
    // 读取文件内容
    f_read(&fil, buffer, sizeof(buffer), &bytesread);
    printf("Read %d bytes: %s\n", bytesread, buffer);
    f_close(&fil);
    f_mount(NULL, "", 0); // 卸载
} 
