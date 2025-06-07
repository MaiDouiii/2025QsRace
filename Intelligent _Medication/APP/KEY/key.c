#include "key.h"

uint8_t key_read()
{
	uint8_t val=0;
	if(KEY3 == 0)val=KEY3_DOWN;
	return val;
}

