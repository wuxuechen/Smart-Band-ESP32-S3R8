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


static const char *TAG_WiFi = "WIFI";

#define WIFI_SSID      "HelloRice"
#define WIFI_PASS      "12345679"

/* The event group allows multiple bits for different events */
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

static bool sntp_inited = false;

void initialize_sntp(void)
{
	if (sntp_inited) return;   
    ESP_LOGI("NTP", "Initializing SNTP");

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org"); // You can use other NTP servers
    esp_sntp_init();
    
    sntp_inited = true;
}

void obtain_time(void)
{
    initialize_sntp();

    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2020 - 1900) && ++retry < retry_count) {
        ESP_LOGI("NTP", "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(pdMS_TO_TICKS(2000));
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year >= (2020 - 1900)) {
        ESP_LOGI("NTP", "Time is set!");
    } else {
        ESP_LOGW("NTP", "Failed to get time from NTP");
    }
}




static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG_WiFi, "Disconnected, reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WiFi, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        obtain_time();
        // Example: Singapore Time (UTC+8)
		setenv("TZ", "SGT-8", 1); // TZ string format: <STD><OFFSET>
		tzset();
        time_t now;
		struct tm timeinfo;
		time(&now);
		localtime_r(&now, &timeinfo);
		
		ESP_LOGI("TIME", "Current time: %04d-%02d-%02d %02d:%02d:%02d",
		         timeinfo.tm_year + 1900,
		         timeinfo.tm_mon + 1,
		         timeinfo.tm_mday,
		         timeinfo.tm_hour,
		         timeinfo.tm_min,
		         timeinfo.tm_sec);
		esp_err_t  err = rtc_set_time(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday,timeinfo.tm_wday, timeinfo.tm_mon, timeinfo.tm_year + 1900 - 2000);
	    if (err == ESP_OK) {
	        ESP_LOGI(TAGRTC, "Time set successfully");
	    } else {
	        ESP_LOGE(TAGRTC, "Failed to set time");
	    }
    }
}

void wifi_init_sta(void)
{
    // Init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG_WiFi, "Waiting for IP address...");
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG_WiFi, "WiFi connection established.");
}

void wifi_connect_task(void *pvParameters)
{
    wifi_init_sta();
    // The task should not be deleted. It can suspend or just end.
    // If other parts of the application might need to manage reconnections
    // or perform other network-related tasks, it's better to keep it running.
    vTaskSuspend(NULL);
}

void init_wifi(void)
{
    xTaskCreate(wifi_connect_task, "wifi_connect", 4096, NULL, 4, NULL);
}
