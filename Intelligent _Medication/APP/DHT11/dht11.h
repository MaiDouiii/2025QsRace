#ifndef __DHT11_H
#define __DHT11_H 

#include "bsp_system.h"


/******************************************************************************************/
/* DHT11 ���� ���� */

#define DHT11_DQ_GPIO_PORT                  GPIOB
#define DHT11_DQ_GPIO_PIN                   GPIO_PIN_3
#define DHT11_DQ_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)   /* PG��ʱ��ʹ�� */

/******************************************************************************************/

/* IO�������� */
                                       #define DHT11_DQ_OUT(x)     do{ x ? \
                                HAL_GPIO_WritePin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN, GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)                                                /* ���ݶ˿���� */
#define DHT11_DQ_IN         HAL_GPIO_ReadPin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN)  /* ���ݶ˿����� */


uint8_t dht11_init(void);   /* ��ʼ��DHT11 */
uint8_t dht11_check(void);  /* ����Ƿ����DHT11 */
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);   /* ��ȡ��ʪ�� */

#endif















