#include "my_key_task.h"
extern uint8_t t;

void key_task(void *params)
{
	static uint8_t key_val, key_down, key_old;
//	uint8_t i = 50;
	while (1)
	{
		key_val = key_read();
		key_down = key_val & (key_val ^ key_old);
		key_old = key_val;

		if (key_down == KEY3_DOWN)
		{
//			at24c02_write(0x00, i);
//			printf("write %d\r\n", i);
//			t = at24c02_read(0x00);
//			printf("t %d\r\n", t);
//			i++;

			xSemaphoreGive(motor_semphr);
		}
		vTaskDelay(10);
	}
}
