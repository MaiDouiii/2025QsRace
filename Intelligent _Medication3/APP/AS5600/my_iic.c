#include "my_iic.h"

#define RCC_IIC_SCL22 RCC_AHB1Periph_GPIOG // �˿�ʱ��
#define IIC_SCL2_PORT2 GPIOG               // �˿�
#define IIC_SCL2 GPIO_PIN_4               // ����

#define RCC_IIC_SDA22 RCC_AHB1Periph_GPIOG // �˿�ʱ��
#define IIC_SDA2_PORT2 GPIOG               // �˿�
#define IIC_SDA2 GPIO_PIN_2               // ����

// IO��������
#define IIC_SCL2_H2 IIC_SCL2_PORT2->BSRR |= IIC_SCL2                 // SCL��1
#define IIC_SCL2_L2 IIC_SCL2_PORT2->BSRR |= (uint32_t)IIC_SCL2 << 16 // SCL��0
#define IIC_SDA2_H2 IIC_SDA2_PORT2->BSRR |= IIC_SDA2                 // SDA��1
#define IIC_SDA2_L2 IIC_SDA2_PORT2->BSRR |= (uint32_t)IIC_SDA2 << 16 // SDA��0
#define READ_SDA2 (IIC_SDA2_PORT2->IDR & IIC_SDA2) ? 1 : 0          // ��ȡSDA���루IIC_SDA2_PORT2->IDR & IIC_SDA2��ʾIIC_SDA2�ڵ�IIC_SDA2���ţ�
#define SDA_IN2()                                \
    {                                           \
        IIC_SDA2_PORT2->MODER &= ~(3 << (9 * 2)); \
        IIC_SDA2_PORT2->MODER |= (0 << (9 * 2));  \
    } // SDA������Ϊ����

#define SDA_OUT2()                               \
    {                                           \
        IIC_SDA2_PORT2->MODER &= ~(3 << (9 * 2)); \
        IIC_SDA2_PORT2->MODER |= (1 << (9 * 2));  \
    } // SDA������Ϊ���

void IIC_init2(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStruct.Pin = IIC_SCL2 | IIC_SDA2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // ����Ϊ�������
    GPIO_InitStruct.Pull = GPIO_NOPULL;         // ��������
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct); // ��ʼ��IIC_SCL2��IIC_SDA2����
    IIC_SDA2_H2;                              // ����SDAΪ�ߵ�ƽ
    IIC_SCL2_H2;                              // ����SCLΪ�ߵ�ƽ
}

void IIC_Start2(void)
{
    // ���
    SDA_OUT2();
    IIC_SDA2_H2;
    IIC_SCL2_H2;
    delay_us(4);
    IIC_SDA2_L2; // ����SDA
    delay_us(4);
    IIC_SCL2_L2; // ǯסIIC���ߣ�׼�����ͻ��������
}

void IIC_Stop2(void)
{
    SDA_OUT2();
    IIC_SCL2_L2;
    IIC_SDA2_L2;
    delay_us(4);
    IIC_SCL2_H2;
    IIC_SDA2_H2;
    delay_us(4);
}

void IIC_ACK2(void)
{
    IIC_SCL2_L2;
    SDA_OUT2();
    IIC_SDA2_L2;
    delay_us(1);
    IIC_SCL2_H2;
    delay_us(2);
    IIC_SCL2_L2;
}
void IIC_NACK2(void)
{
    IIC_SCL2_L2;
    SDA_OUT2();
    IIC_SDA2_H2;
    delay_us(1);
    IIC_SCL2_H2;
    delay_us(1);
    IIC_SCL2_L2;
}

uint8_t IIC_waitACK2(void)
{
    uint8_t ucErrTime = 0;
    SDA_IN2();
    IIC_SDA2_H2;
    delay_us(1);
    IIC_SCL2_H2;
    delay_us(1);
    while (READ_SDA2)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL2_L2;
    // delay_us(20);
    return 0;
}

void IIC_SendByte2(uint8_t txd)
{
    uint8_t t;
    SDA_OUT2();
    IIC_SCL2_L2;
    for (t = 0; t < 8; t++)
    {
        if ((txd & 0x80) >> 7)
            IIC_SDA2_H2;
        else
            IIC_SDA2_L2;
        txd <<= 1;
        delay_us(1);
        IIC_SCL2_H2;
        delay_us(1);
        IIC_SCL2_L2;
        delay_us(1);
    }
}

uint8_t IIC_ReadByte2(uint8_t ack)
{
    uint8_t i, receive = 0;
    SDA_IN2();
    for (i = 0; i < 8; i++)
    {
        IIC_SCL2_L2;
        delay_us(1);
        IIC_SCL2_H2;
        receive <<= 1;
        if (READ_SDA2)
            receive++;
        delay_us(1);
    }
    if (!ack)
		IIC_NACK();
	else
		IIC_ACK();
    return receive;
}

uint8_t AS5600_IIC_Read_OneByte(uint8_t deviceaddr,uint8_t readaddr)
{
  uint8_t temp;
  IIC_Start2();
  IIC_SendByte2(deviceaddr&0xfe);
  IIC_waitACK2();
  IIC_SendByte2(readaddr);
  IIC_waitACK2();
  IIC_Stop2();
	
  IIC_Start2();
  IIC_SendByte2(deviceaddr|0x01);
  IIC_waitACK2();
  temp=IIC_ReadByte2(0);
  IIC_Stop2();
  return temp;
}
uint32_t AS5600_Get_Data(void)
{
	uint32_t value=0;
	value=AS5600_IIC_Read_OneByte((0x36<<1),0x0e);
    value<<= 8;
    value|=AS5600_IIC_Read_OneByte((0x36<<1),0x0f);
	return value;
}

