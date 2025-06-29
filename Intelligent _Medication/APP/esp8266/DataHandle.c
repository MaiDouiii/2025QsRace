#include "DataHandle.h"
#include "cJSON.h"

void TimeGet(Users *user, char *str,unsigned char user_id)
{
		char show_time[6] = {0};
    for (int i = 2; i < strlen(str); i++)
    {
        if (str[i] == 'M' || str[i] == 'N' || str[i] == 'E')
        {
            int index = (str[i] == 'M') ? 0 : (str[i] == 'N') ? 1
                                                              : 2;
            user->Time[index][0] = (str[i + 2] - '0') * 10 + (str[i + 3] - '0'); // 时
            user->Time[index][1] = (str[i + 5] - '0') * 10 + (str[i + 6] - '0'); // 分
						sprintf(show_time,"%02d:%02d",user->Time[index][0],user->Time[index][1]);
						if(index == 0)
						{
							xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
							if(user_id == 0)
							{
								lv_textarea_set_text(ui_User1Time1,(const char *)show_time);
							}
							else if(user_id == 1)
							{
								lv_textarea_set_text(ui_User2Time1,(const char *)show_time);
							}
							else
							{
								lv_textarea_set_text(ui_User3Time1,(const char *)show_time);
							}
							xSemaphoreGive(lvgl_semphr);
						}
						else if(index == 1)
						{
							xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
							if(user_id == 0)
							{
								lv_textarea_set_text(ui_User1Time2,(const char *)show_time);
							}
							else if(user_id == 1)
							{
								lv_textarea_set_text(ui_User2Time2,(const char *)show_time);
							}
							else
							{
								lv_textarea_set_text(ui_User3Time2,(const char *)show_time);
							}
							xSemaphoreGive(lvgl_semphr);
						}
						else
						{
							xSemaphoreTake(lvgl_semphr, portMAX_DELAY);
							if(user_id == 0)
							{
								lv_textarea_set_text(ui_User1Time3,(const char *)show_time);
							}
							else if(user_id == 1)
							{
								lv_textarea_set_text(ui_User2Time3,(const char *)show_time);
							}
							else
							{
								lv_textarea_set_text(ui_User3Time3,(const char *)show_time);
							}
							xSemaphoreGive(lvgl_semphr);
						}
        }
    }
}

void parse_mqtt_command(const char *data)
{
	
    // 找到 JSON 数据的起始位置（第一个 '{' 字符）
    const char *json_start = strchr(data, '{');
    if (json_start == NULL)
    {
        printf("Invalid data format: JSON not found\n");
        return;
    }

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(json_start);
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("JSON parse error before: %s\n", error_ptr);
        }
        return;
    }

    // 解析 paras.value
    cJSON *paras = cJSON_GetObjectItemCaseSensitive(root, "paras");
    if (cJSON_IsObject(paras))
    {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(paras, "value");
        if (cJSON_IsString(value) && (value->valuestring != NULL))
        {
						printf("%s\n", value->valuestring);
						if(value->valuestring[0] == 'T')		//时间设置
						{
							if(value->valuestring[2] == '0')
							{
								TimeGet(&user_messages[0],value->valuestring,0);
							}
							else if(value->valuestring[2] == '1')
							{
								TimeGet(&user_messages[1],value->valuestring,1);
							}
							else
							{
								TimeGet(&user_messages[2],value->valuestring,2);
							}
						}
            
        }
    }

    //    // 解析 service_id
    //    cJSON *service_id = cJSON_GetObjectItemCaseSensitive(root, "service_id");
    //    if (cJSON_IsString(service_id) && service_id->valuestring != NULL) {
    //        printf("Service ID: %s\n", service_id->valuestring);
    //    } else if (cJSON_IsNull(service_id)) {
    //        printf("Service ID: null\n");
    //    }

    //    // 解析 command_name
    //    cJSON *command_name = cJSON_GetObjectItemCaseSensitive(root, "command_name");
    //    if (cJSON_IsString(command_name) && command_name->valuestring != NULL) {
    //        printf("Command name: %s\n", command_name->valuestring);
    //    } else if (cJSON_IsNull(command_name)) {
    //        printf("Command name: null\n");
    //    }

    // 清理资源
    cJSON_Delete(root);
}
