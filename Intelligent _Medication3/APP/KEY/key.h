#ifndef __key_H__
#define __key_H__

#include "bsp_system.h"

#define KEY3 HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)

#define KEY3_DOWN 1<<1

uint8_t key_read(void);

#endif
