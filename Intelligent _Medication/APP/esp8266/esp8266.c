#include "esp8266.h"

char esp8266_buf[300];
unsigned short esp8266_cnt = 0, esp8266_cntPre =0;

char esp8266_Data[200];

uint8_t esp8266_flag = 0;

const char *ESP8266_WIFI = "AT+CWJAP=\"nier\",\"12345678\"\r\n";
const char *ESP8266_MQTT_Password = "AT+MQTTUSERCFG=0,1,\"NULL\",\"67c3cdd924d772325521cd72_maidou\",\"88f0b33681fd87928e5b083cb5948f06494b40c5097157fa28d4a9d38fecf405\",0,0,\"\"\r\n";
const char *ESP8266_MQTT_Setting = "AT+MQTTCLIENTID=0,\"67c3cdd924d772325521cd72_maidou_0_0_2025042609\"\r\n";
const char *ESP8266_MQTT_IP = "AT+MQTTCONN=0,\"117.78.5.125\",1883,1\r\n";
const char *ESP8266_MQTT_DingYue = "AT+MQTTSUB=0,\"$oc/devices/67c3cdd924d772325521cd72_maidou_0_0_2025042708/sys/commands/#\",1";
const char *ESP8266_Send1 = "AT+MQTTPUB=0,\"$oc/devices/67c3cdd924d772325521cd72_maidou_0_0_2025041508/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"text\\\"\\\,\\\"properties\\\":{\\\"%s\\\":%d}}]}\",0,0\r\n";
const char *ESP8266_Send2 = "AT+MQTTPUB=0,\"$oc/devices/67c3cdd924d772325521cd72_maidou_0_0_2025041508/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"text\\\"\\\,\\\"properties\\\":{\\\"%s\\\":\\\"%s\\\"}}]}\",0,0\r\n";

void Usart_SendString(UART_HandleTypeDef *huart,  unsigned char *str,uint16_t Size)
{
    for(int count = 0; count < Size; count++)
		{
				HAL_UART_Transmit(huart, (uint8_t *)str++ ,1, HAL_MAX_DELAY);
		}
}

void ESP8266_Clear(void)
{
	
	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
}

_Bool ESP8266_WaitRecive(void)
{

	if (esp8266_cnt == 0) // 如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;

	if (esp8266_cnt == esp8266_cntPre) // 如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0; // 清0接收计数

		return REV_OK; // 返回接收完成标志
	}

	esp8266_cntPre = esp8266_cnt; // 置为相同

	return REV_WAIT; // 返回接收未完成标志
}

_Bool ESP8266_SendCmd(const char *cmd, char *res)
{
	unsigned char timeOut = 200;

	Usart_SendString(&huart3, (unsigned char *)cmd, strlen((const char *)cmd));

	while (timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)
			{
				ESP8266_Clear();
				return 0;
			}
		}
		delay_us(100);
		
	}
	return 1;
}

void ESP8266_Init(void)
{
	ESP8266_Clear();
    

    
	printf("1 ATE0\r\n");
	while(ESP8266_SendCmd("ATE0\r\n", "OK"))
	delay_ms(500);

	
	printf("2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
	delay_ms(500);
	
	printf("3. AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))

	delay_ms(500);
	
//	printf("4. CWJAP\r\n");
//	while(ESP8266_SendCmd(ESP8266_WIFI, "OK"))
//	delay_ms(500);
	
	printf("5. PassWord_Connect\r\n");
	while(ESP8266_SendCmd(ESP8266_MQTT_Password, "OK"))
	delay_ms(1500);
    
	printf("6. Set_Connect\r\n");
	while(ESP8266_SendCmd(ESP8266_MQTT_Setting, "OK"))
	delay_ms(1500);
    
	printf("7. IP_Connect\r\n");
	while(ESP8266_SendCmd(ESP8266_MQTT_IP, "OK"))
	delay_ms(1500);
    
	printf("8. DingYue_Connect\r\n");
	while(ESP8266_SendCmd(ESP8266_MQTT_DingYue, "OK"))
	delay_ms(500);
	
  HAL_UART_Transmit(&huart6, "F", 1, 10);			//鎾姤鎴愬姛涓婁簯
	
	//printf("9. ESP8266 Init OK\r\n");
	ESP8266_Clear();
	esp8266_flag = 1;
}

void ESP8266_UPDataNum(char *buf, const char *pack, const char *Device, int num) // 路垄藵式讝
{
	memset(buf,0,sizeof((const char *)buf));
	sprintf(buf, pack, Device, num);
	Usart_SendString(&huart3, (unsigned char *)buf, strlen(buf));
	delay_ms(200);
}

void ESP8266_UPDataStr(char *buf, const char *pack, const char *Device, char *str) // 路垄藵讝路没
{
	memset(buf,0,sizeof((const char *)buf));
	sprintf(buf, pack, Device, str);
	Usart_SendString(&huart3, (unsigned char *)buf, strlen(buf));
	delay_ms(200);
}
