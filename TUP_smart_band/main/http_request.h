#ifndef HTTP_CLIENT_HEADER_H
#define HTTP_CLIENT_HEADER_H

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "lvgl.h"

int get_img(const char* desc);

esp_err_t http_event_handler(esp_http_client_event_t *evt);

void fetch_weather();

void weather_task(void *pvParameter);

void init_weather();
#endif // HTTP_CLIENT_HEADER_H