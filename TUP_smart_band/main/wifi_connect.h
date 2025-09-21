#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include <time.h>
#include "pcf85063.h"
#include "http_request.h"
#include "http_server.h"

#define WIFI_SSID      "HelloRice"
#define WIFI_PASS      "12345679"

#define WIFI_CONNECTED_BIT BIT0

typedef enum {
    WIFI_CMD_ENABLE,
    WIFI_CMD_DISABLE
} wifi_cmd_t;

extern QueueHandle_t wifi_cmd_queue;
void wifi_task(void *arg);
void wifi_init_sta(void);
void wifi_switch(bool enable);
void init_wifi_reconnect_system();

#endif // WIFI_MANAGER_H
