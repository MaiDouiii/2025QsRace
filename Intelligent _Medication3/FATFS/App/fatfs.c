/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
//  RTC_TimeTypeDef sTime;
//  RTC_DateTypeDef sDate;
//  
//  // 从RTC获取当前时间
//  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
//  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
//  
//  // 转换为FAT时间格式
//  return ((sDate.Year) << 25) |  // 年 (2025年 = 45)
//         ((sDate.Month) << 21) |  // 月 (1-12)
//         ((sDate.Date) << 16) |  // 日 (1-31)
//         ((sTime.Hours) << 11) | // 时 (0-23)
//         ((sTime.Minutes) << 5) | // 分 (0-59)
//         ((sTime.Seconds / 2));   // 秒 (除以2)
	
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */
