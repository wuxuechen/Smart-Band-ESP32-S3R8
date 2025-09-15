#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include <time.h>
#include "pcf85063.h"
#include "http_request.h"
#include "http_server.h"

#define WIFI_SSID      "HelloRice"
#define WIFI_PASS      "12345679"

#define WIFI_CONNECTED_BIT BIT0

void wifi_init_sta(void);
void wifi_switch(bool enable);
void init_wifi(void);

#endif // WIFI_MANAGER_H
