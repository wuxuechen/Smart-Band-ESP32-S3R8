#include "wifi_connect.h"


static const char *TAG_WiFi = "WIFI";

/* The event group allows multiple bits for different events */
static EventGroupHandle_t wifi_event_group;

static bool sntp_inited = false;

QueueHandle_t wifi_cmd_queue = NULL;

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

void set_local_time()
{
    // 1. Obtain SNTP / network time
    obtain_time();

    // 2. Set timezone (Singapore Time UTC+8)
    setenv("TZ", "SGT-8", 1);
    tzset();

    // 3. Get current time
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    ESP_LOGI("TIME", "Current time: %04d-%02d-%02d %d %02d:%02d:%02d",
             timeinfo.tm_year + 1900,
             timeinfo.tm_mon + 1,
             timeinfo.tm_mday,
             timeinfo.tm_wday,
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec);

    // 4. Set RTC
    esp_err_t err = rtc_set_time(
        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
        timeinfo.tm_mday, timeinfo.tm_wday,
        timeinfo.tm_mon, timeinfo.tm_year + 1900 - 2000
    );

    if (err == ESP_OK) {
        ESP_LOGI(TAGRTC, "Time set successfully");
    } else {
        ESP_LOGE(TAGRTC, "Failed to set time");
    }
}

void wifi_task(void *arg) {
    wifi_cmd_t cmd;
    while (1) {
        if (xQueueReceive(wifi_cmd_queue, &cmd, portMAX_DELAY)) {
            if (cmd == WIFI_CMD_ENABLE) {
                wifi_switch(true);
            } else {
                wifi_switch(false);
            }
        }
    }
}


static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		
		wifi_config_t wifi_config = {0};
        // Set authmode
		wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
		
		// Copy SSID and password safely
		strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid) - 1);
		wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = 0;  // null-terminate
		
		strncpy((char*)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password) - 1);
		wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = 0;  // null-terminate
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG_WiFi, "Disconnected, reconnecting...");
        	wifi_config_t wifi_config = {0};
        // Set authmode
		wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
		
		// Copy SSID and password safely
		strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid) - 1);
		wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = 0;  // null-terminate
		
		strncpy((char*)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password) - 1);
		wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = 0;  // null-terminate
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WiFi, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        // Start post-WiFi tasks here (only once)
        init_weather(); // creates weather_task
        set_local_time();

    	    // start async weather per minutes
/*    	vTaskDelay(pdMS_TO_TICKS(5000));
    	start_http_server();*/

    }
}

void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    cfg.static_rx_buf_num   = 4;   // default 10
	cfg.dynamic_rx_buf_num  = 8;   // default 32
	cfg.static_tx_buf_num   = 4;   // default 16
	cfg.cache_tx_buf_num    = 4;   // (if present in your IDF version)
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

    wifi_config_t wifi_config = {0};  // zero-initialize

	// Set authmode
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	
	// Copy SSID and password safely
	strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid) - 1);
	wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = 0;  // null-terminate
	
	strncpy((char*)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password) - 1);
	wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = 0;  // null-terminate

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG_WiFi, "Waiting for IP address...");
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG_WiFi, "WiFi connection established.");
}

static bool wifi_initialized = false;

void wifi_switch(bool enable)
{
	if (!wifi_initialized) {
        wifi_init_sta();   // run your full init (or esp_wifi_init + esp_wifi_set_mode)
        wifi_initialized = true;
    }
	
    if (enable) {
        ESP_LOGI(TAG_WiFi, "Enabling WiFi...");
        esp_err_t ret = esp_wifi_start();
        if (ret == ESP_OK) {
            ESP_LOGI(TAG_WiFi, "WiFi started");
        } else if (ret == ESP_ERR_WIFI_CONN) {
            ESP_LOGW(TAG_WiFi, "WiFi already started");
        } else {
            ESP_LOGE(TAG_WiFi, "Failed to start WiFi: %s", esp_err_to_name(ret));
        }
    } else {
        ESP_LOGI(TAG_WiFi, "Disabling WiFi...");
        esp_err_t ret = esp_wifi_stop();
        if (ret == ESP_OK) {
            ESP_LOGI(TAG_WiFi, "WiFi stopped");
        } else if (ret == ESP_ERR_WIFI_NOT_STOPPED) {
            ESP_LOGW(TAG_WiFi, "WiFi already stopped");
        } else {
            ESP_LOGE(TAG_WiFi, "Failed to stop WiFi: %s", esp_err_to_name(ret));
        }
    }
}


void wifi_connect_task(void *pvParameters)
{
    wifi_init_sta();
    // The task should not be deleted. It can suspend or just end.
    // If other parts of the application might need to manage reconnections
    // or perform other network-related tasks, it's better to keep it running.
    vTaskSuspend(NULL);
}


