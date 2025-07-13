#ifndef DATAHANDLE_H_
#define DATAHANDLE_H_

#define MAX_TOPIC_LEN 256
#define MAX_UUID_LEN  64
#define MAX_VALUE_LEN 32


#include "bsp_system.h"

//void getJsonData(void);
void parse_mqtt_command(const char *data);

#endif
