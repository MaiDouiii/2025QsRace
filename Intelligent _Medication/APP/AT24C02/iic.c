#include "iic.h"

#define RCC_IIC_SCL RCC_AHB1Periph_GPIOB // 端口时钟
#define IIC_SCL_PORT GPIOB               // 端口
#define IIC_SCL GPIO_PIN_8               // 引脚

#define RCC_IIC_SDA RCC_AHB1Periph_GPIOB // 端口时钟
#define IIC_SDA_PORT GPIOB               // 端口
#define IIC_SDA GPIO_PIN_9               // 引脚

// IO操作函数
#define IIC_SCL_H IIC_SCL_PORT->BSRR |= IIC_SCL                 // SCL置1
#define IIC_SCL_L IIC_SCL_PORT->BSRR |= (uint32_t)IIC_SCL << 16 // SCL置0
#define IIC_SDA_H IIC_SDA_PORT->BSRR |= IIC_SDA                 // SDA置1
#define IIC_SDA_L IIC_SDA_PORT->BSRR |= (uint32_t)IIC_SDA << 16 // SDA置0
#define READ_SDA (IIC_SDA_PORT->IDR & IIC_SDA) ? 1 : 0          // 读取SDA输入（IIC_SDA_PORT->IDR & IIC_SDA表示IIC_SDA口的IIC_SDA引脚）
#define SDA_IN()                                \
    {                                           \
        IIC_SDA_PORT->MODER &= ~(3 << (9 * 2)); \
        IIC_SDA_PORT->MODER |= (0 << (9 * 2));  \
    } // SDA口设置为输入

#define SDA_OUT()                               \
    {                                           \
        IIC_SDA_PORT->MODER &= ~(3 << (9 * 2)); \
        IIC_SDA_PORT->MODER |= (1 << (9 * 2));  \
    } // SDA口设置为输出

void IIC_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = IIC_SCL | IIC_SDA;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // 设置为推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;         // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // 初始化IIC_SCL和IIC_SDA引脚
    IIC_SDA_H;                              // 设置SDA为高电平
    IIC_SCL_H;                              // 设置SCL为高电平
}

void IIC_Start(void)
{
    // 输出
    SDA_OUT();
    IIC_SDA_H;
    IIC_SCL_H;
    delay_us(4);
    IIC_SDA_L; // 拉低SDA
    delay_us(4);
    IIC_SCL_L; // 钳住IIC总线，准备发送或接收数据
}

void IIC_Stop(void)
{
    SDA_OUT();
    IIC_SCL_L;
    IIC_SDA_L;
    delay_us(4);
    IIC_SCL_H;
    IIC_SDA_H;
    delay_us(4);
}

void IIC_ACK(void)
{
    IIC_SCL_L;
    SDA_OUT();
    IIC_SDA_L;
    delay_us(1);
    IIC_SCL_H;
    delay_us(2);
    IIC_SCL_L;
}
void IIC_NACK(void)
{
    IIC_SCL_L;
    SDA_OUT();
    IIC_SDA_H;
    delay_us(1);
    IIC_SCL_H;
    delay_us(1);
    IIC_SCL_L;
}

uint8_t IIC_waitACK(void)
{
    uint8_t ucErrTime = 0;
    SDA_IN();
    IIC_SDA_H;
    delay_us(1);
    IIC_SCL_H;
    delay_us(1);
    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL_L;
    // delay_us(20);
    return 0;
}

void IIC_SendByte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL_L;
    for (t = 0; t < 8; t++)
    {
        if ((txd & 0x80) >> 7)
            IIC_SDA_H;
        else
            IIC_SDA_L;
        txd <<= 1;
        delay_us(1);
        IIC_SCL_H;
        delay_us(1);
        IIC_SCL_L;
        delay_us(1);
    }
}

uint8_t IIC_ReadByte(uint8_t ack)
{
    uint8_t i, receive = 0;
    SDA_IN();
    for (i = 0; i < 8; i++)
    {
        IIC_SCL_L;
        delay_us(1);
        IIC_SCL_H;
        receive <<= 1;
        if (READ_SDA)
            receive++;
        delay_us(1);
    }
    if (!ack)
		IIC_NACK();
	else
		IIC_ACK();
    return receive;
}

void at24c02_write(uint8_t addr, uint8_t data)
{
    IIC_Start();
    IIC_SendByte(0xA0); // 设备地址+写
    IIC_waitACK();

    IIC_SendByte(addr); // 写入地址

    IIC_waitACK();

    IIC_SendByte(data); // 写入数据
    IIC_waitACK();
    IIC_Stop();
    HAL_Delay(5);
}

uint8_t at24c02_read(uint8_t addr)
{
    uint8_t data = 0;
    IIC_Start();
    IIC_SendByte(0xA0); // 设备地址+写
    IIC_waitACK();

    IIC_SendByte(addr); // 写入地址
    IIC_waitACK();

    IIC_Start();
    IIC_SendByte(0xA1); // 设备地址+读
    IIC_waitACK();

    data = IIC_ReadByte(0); // 读取数据
    
    IIC_Stop();
    return data;
}
