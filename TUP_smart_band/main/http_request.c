#include "http_request.h"
#include "cJSON.h"
#include "esp_crt_bundle.h" 
#include "widgets/lv_label.h"
#include "ui/screens.h"
#include "ui/images.h"

#define BUFFER_SIZE 4096

int get_img(const char* desc){
	if (!desc || !desc[0]) return -1;

    char desc_mod[64];  // adjust size as needed
    strncpy(desc_mod, desc, sizeof(desc_mod)-1);
    desc_mod[sizeof(desc_mod)-1] = '\0';

    // Make only the first character lowercase
    desc_mod[0] = tolower((unsigned char)desc_mod[0]);

    for (int i = 0; i < 18; i++) {
        if (strstr(desc_mod, images[i].name)) {
            return i;
        }
    }

    return -1; // not found
}

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    static char buffer[2048]; // accumulate data if multiple chunks
    static int offset = 0;

    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (evt->data_len > 0) {
                // append chunk to buffer
                if (offset + evt->data_len < sizeof(buffer)) {
                    memcpy(buffer + offset, evt->data, evt->data_len);
                    offset += evt->data_len;
                }

                // null-terminate
                buffer[offset] = '\0';
                // Parse JSON once
                cJSON *root = cJSON_Parse(buffer);
                if (root) {
                    // Current condition
                    cJSON *current = cJSON_GetObjectItem(root, "current_condition");
                    if (current) {
                        const char *temp = cJSON_GetObjectItem(current, "temp_C")->valuestring;
                        const char *desc = cJSON_GetObjectItem(current, "weatherDesc")->valuestring;
                        ESP_LOGI("WEATHER", "Current:%s Desc:%s", temp, desc);
                        int index = get_img(desc);
                        if (index==-1){
							lv_label_set_text(objects.weather_current_label, "sunny");
							lv_label_set_text(objects.home_weather, "sunny");
							lv_img_set_src(objects.weather_current, &img_sun);
							lv_img_set_src(objects.home_weather_img, &img_sun);
						}else{
							lv_label_set_text(objects.weather_current_label, images[index].name);
							lv_label_set_text(objects.home_weather, images[index].name);
							lv_img_set_src(objects.weather_current, images[index].img_dsc);
							lv_img_set_src(objects.home_weather_img, images[index].img_dsc);
						}
                        if(temp){
							lv_label_set_text_fmt(objects.weather_current_tmp_label, "%s°C", temp);
						}
                    }

                    // Today forecast
                    cJSON *today = cJSON_GetObjectItem(root, "today");
                    if (today) {
                        const char *maxC = cJSON_GetObjectItem(today, "max_temp_C")->valuestring;
                        const char *minC = cJSON_GetObjectItem(today, "min_temp_C")->valuestring;
                        const char *desc = cJSON_GetObjectItem(today, "weatherDesc")->valuestring;
                        ESP_LOGI("WEATHER", "Today Max:%s Min:%s Desc:%s", maxC, minC, desc);
                        const char *max2show = maxC ? maxC: "N/A";
					    const char *min2show = minC ? minC : "N/A";
					    const char *desc2show = desc ? desc : "N/A";					
					    char temp_buf[64];
					    snprintf(temp_buf, sizeof(temp_buf), "Today: %s°C-%s°C\n%s", max2show, min2show, desc2show);
					    lv_label_set_text(objects.weather_today, temp_buf);
                        
                    }

                    // Future days
                    cJSON *future_days = cJSON_GetObjectItem(root, "future_days");
                    if (future_days && cJSON_IsArray(future_days)) {
                        int size = cJSON_GetArraySize(future_days);
                        for (int i = 0; i < size; i++) {
                            cJSON *day = cJSON_GetArrayItem(future_days, i);
                            const char *date = cJSON_GetObjectItem(day, "date")->valuestring;
                            const char *maxC = cJSON_GetObjectItem(day, "max_temp_C")->valuestring;
                            const char *minC = cJSON_GetObjectItem(day, "min_temp_C")->valuestring;
                            const char *desc = cJSON_GetObjectItem(day, "weatherDesc")->valuestring;
                            ESP_LOGI("WEATHER", "Day %s Max:%s Min:%s Desc:%s", date, maxC, minC, desc);
                            const char *date2show = date ? date: "N/A";
                            const char *max2show = maxC ? maxC: "N/A";
						    const char *min2show = minC ? minC : "N/A";
						    const char *desc2show = desc ? desc : "N/A";					
						    char temp_buf[64];
						    snprintf(temp_buf, sizeof(temp_buf), "%s: %s°C-%s°C\n%s", date2show, max2show, min2show, desc2show);
						    if (i==0){
								lv_label_set_text(objects.weather_tomorrow, temp_buf);
							}
							if(i==1){
								lv_label_set_text(objects.weather_after, temp_buf);
							}
						    
                        }
                    }

                    cJSON_Delete(root);
                    offset = 0; // reset for next request
                }
            }
            break;

        case HTTP_EVENT_ON_FINISH:
        case HTTP_EVENT_DISCONNECTED:
            offset = 0; // reset in case of error
            break;

        default:
            break;
    }
    return ESP_OK;
}


void fetch_weather()
{
    esp_http_client_config_t config = {
	    .url = "http://192.168.0.122:5000/weather",
	    .event_handler = http_event_handler,
    };
	ESP_LOGI("HTTP", "url:%s\n", config.url);
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int content_length = esp_http_client_get_content_length(client);
        ESP_LOGI("HTTP", "content_length:%d\n",content_length);
    } else {
        ESP_LOGW("HTTP", "HTTP request failed: %s\n", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void weather_task(void *pvParameter)
{
    while (1) {
        fetch_weather();
        vTaskDelay(pdMS_TO_TICKS(10000)); // update every 60 seconds
    }
}

void init_weather(){
	BaseType_t ret = xTaskCreate(weather_task, "weather_task", 4096, NULL, 4, NULL);
	if(ret != pdPASS) {
    ESP_LOGE("TASK", "Failed to create weather_task!");
}
}

