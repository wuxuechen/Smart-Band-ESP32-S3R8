#ifndef HTTP_CLIENT_HEADER_H
#define HTTP_CLIENT_HEADER_H

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "lvgl.h"
#include "qmi8658c.h"
#include "lcd_touch.h"
#include "esp_https_ota.h"
#include "constant.h"

int get_img(const char* desc);

void update_check(void *pvParameters);

void ota_background_task(void *pvParameters);

esp_err_t http_event_handler(esp_http_client_event_t *evt);

void fetch_weather_gait();

void weather_task(void *pvParameter);

void init_weather();
#endif // HTTP_CLIENT_HEADER_H