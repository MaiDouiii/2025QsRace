#ifndef __IIC_H
#define __IIC_H

#include "bsp_system.h"

void IIC_init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_ACK(void);
void IIC_NACK(void);
uint8_t IIC_waitACK(void);
void IIC_SendByte(uint8_t txd);
uint8_t IIC_ReadByte(uint8_t ack);

void at24c02_write(uint8_t addr, uint8_t data);
uint8_t at24c02_read(uint8_t addr);

#endif
