#include "http_request.h"

#include "esp_crt_bundle.h" 

#define BUFFER_SIZE 4096

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (evt->data_len > 0) {
                // evt->data is NOT null-terminated, so use fwrite or printf with length
                printf("%.*s", evt->data_len, (char *)evt->data);
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}


void fetch_weather(char *tempC, char *weatherDesc, size_t desc_len)
{
    char buffer[BUFFER_SIZE] = {0};

    esp_http_client_config_t config = {
	    .url = "http://192.168.0.122:5000/weather",
	    .event_handler = http_event_handler,
    };
    printf("url:%s\n", config.url);

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int content_length = esp_http_client_get_content_length(client);
        printf("content_length:%d\n",content_length);
        int read_len = esp_http_client_read(client, buffer, BUFFER_SIZE - 1);
        printf("read_len:%d\n",read_len);
        if (read_len > 0) buffer[read_len] = '\0';
		printf("weather info:%s\n",buffer);
        // Parse JSON
/*        cJSON *root = cJSON_Parse(buffer);
        if (root) {
            cJSON *current = cJSON_GetObjectItem(root, "current_condition");
            if (cJSON_IsArray(current)) {
                cJSON *first = cJSON_GetArrayItem(current, 0);
                const char *tmp = cJSON_GetObjectItem(first, "temp_C")->valuestring;
                const char *desc = cJSON_GetObjectItem(first, "weatherDesc")->child->valuestring;

                strncpy(tempC, tmp, 8);
                strncpy(weatherDesc, desc, desc_len);
            }
            cJSON_Delete(root);
        }*/
    } else {
        printf("HTTP request failed: %s\n", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void weather_task(void *pvParameter)
{
    char tempC[8] = {0};
    char weatherDesc[64] = {0};

    while (1) {
        fetch_weather(tempC, weatherDesc, sizeof(weatherDesc));
        printf("tempc:%s\nweatherDesc:%s\n",tempC, weatherDesc);
        vTaskDelay(pdMS_TO_TICKS(60000)); // update every 60 seconds
    }
}

void init_weather(){
	xTaskCreate(weather_task, "weather_task", 8192, NULL, 5, NULL);
}

