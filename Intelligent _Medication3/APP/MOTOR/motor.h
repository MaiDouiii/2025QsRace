#ifndef __motor_H__
#define __motor_H__

#include "bsp_system.h"

#define Clockwise 1        // 顺时针
#define CounterClockwise 0 // 逆时针

#define motor_bottom 0x02
#define motor_bottom_back 0
#define motor_bottom_forward 1

#define motor_top 0x01
#define motor_top_down 1
#define motor_top_up 0

/*-----------使能信号控制----------*/

void motor_En_Control(uint8_t addr, uint8_t state, uint8_t snF);

/*-------------位置模式------------*/

void motor_pos_control(uint8_t addr, uint8_t dir, uint16_t speed, uint8_t acceleration, uint32_t clk, uint8_t mode, uint8_t duoji_tongbu);

/*-------------立即停止------------*/

void motor_Stop_Now(uint8_t addr, uint8_t snF);

/*-------------读取转速------------*/

void motor_get_speed(uint8_t addr);

/*---------------校准--------------*/

void motor_calibration(uint8_t addr);

void motor_init(void);
void motor_take_control(void);
void motor_down_control(uint8_t dir, uint32_t num);
void motor_down_en(uint8_t en);

void motor_move_step(int16_t num);		//转盘转动步数

#endif
