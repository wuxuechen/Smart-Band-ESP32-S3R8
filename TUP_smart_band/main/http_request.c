#include "http_request.h"
#include "cJSON.h"
#include "esp_crt_bundle.h" 
#include "esp_wifi.h"
#include "lcd_touch.h"
#include "widgets/lv_label.h"
#include "ui/screens.h"
#include "ui/images.h"
#include <stdio.h>
#include <string.h>
#include "esp_ota_ops.h"
#include "esp_partition.h"


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

char version_from_server[32] = {0};

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    static char buffer[1536]; // accumulate data if multiple chunks
    static int offset = 0;

    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (evt->data_len > 0) {
                // Append chunk to buffer
                if (offset + evt->data_len < sizeof(buffer)) {
                    memcpy(buffer + offset, evt->data, evt->data_len);
                    offset += evt->data_len;
                }

                // Null-terminate the buffer
                buffer[offset] = '\0';
                
                // Parse JSON once
                cJSON *root = cJSON_Parse(buffer);
                if (root) {
                    // Parse status
                    cJSON *status = cJSON_GetObjectItem(root, "status");
                    if (status && cJSON_IsString(status)) {
                        ESP_LOGI("RESPONSE", "Status: %s", status->valuestring);
                    }
                    
                    cJSON *version = cJSON_GetObjectItem(root, "version");
                    if (version && cJSON_IsString(version)) {
                        ESP_LOGI("RESPONSE", "Status: %s", version->valuestring);
                    }
					snprintf(version_from_server, sizeof(version_from_server), "%s",version->valuestring);
                    // Parse top9 data
                    lvgl_msg_t msg;
                    cJSON *top9 = cJSON_GetObjectItem(root, "top9");
                    if (top9 && cJSON_IsArray(top9)) {
                        int top9_size = cJSON_GetArraySize(top9);
                        ESP_LOGI("TOP9", "Found %d entries in top9", top9_size);
                        
                        // Process up to 9 entries (or as many as available)

                        msg.type = MSG_TYPE_TOP9;
                        for (int i = 0; i < top9_size && i < 9; i++) {
                            cJSON *entry = cJSON_GetArrayItem(top9, i);
                            if (entry) {
                                // Get username
                                cJSON *username = cJSON_GetObjectItem(entry, "username");
                                const char *user = username && cJSON_IsString(username) ? 
                                                  username->valuestring : "Unknown";
                                
                                // Get steps
                                cJSON *steps = cJSON_GetObjectItem(entry, "steps");
                                int step_count = steps && (cJSON_IsNumber(steps) || cJSON_IsString(steps)) ? 
                                                steps->valueint : 0;
                                
                                ESP_LOGI("TOP9", "Rank %d: %s - %d steps", i+1, user, step_count);
                                snprintf(msg.score[i].name, sizeof(msg.score[i].name), "%s",user);                           
                                msg.score[i].steps = step_count;
                                
                                // Update your UI elements here
                                // Example assuming you have objects.top9_rankX_label for each rank
                                /*
                                char rank_text[64];
                                snprintf(rank_text, sizeof(rank_text), "%d. %s: %d steps", i+1, user, step_count);
                                
                                switch(i) {
                                    case 0: lv_label_set_text(objects.top9_rank1_label, rank_text); break;
                                    case 1: lv_label_set_text(objects.top9_rank2_label, rank_text); break;
                                    case 2: lv_label_set_text(objects.top9_rank3_label, rank_text); break;
                                    // Add cases for ranks 4-9 as needed
                                    default: break;
                                }
                                */
                            }
                        }
                        if (top9_size < 9) {
								snprintf(msg.score[top9_size].name, sizeof(msg.score[top9_size].name), "%s","-1");
								msg.score[top9_size].steps = 0;
							}
                        if(lvgl_update_queue){
				        	xQueueSend(lvgl_update_queue, &msg, 0);
				        }
                    } else {
                        ESP_LOGW("TOP9", "No valid top9 data found");
                    }

                    // Parse weather data if available
                    cJSON *weather = cJSON_GetObjectItem(root, "weather");
                    if (weather) {
                        // Check if weather is "NA" (string) or a full object
                        if (cJSON_IsString(weather) && strcmp(weather->valuestring, "NA") == 0) {
                            ESP_LOGI("WEATHER", "Weather data not available (NA)");
                            // Update UI to show weather is unavailable
                            /*
                            lv_label_set_text(objects.weather_current_label, "N/A");
                            lv_label_set_text(objects.home_weather, "N/A");
                            lv_label_set_text(objects.weather_current_tmp_label, "N/A");
                            */
                        } 
                        // Check if weather is a JSON object with data
                        else if (cJSON_IsObject(weather)) {
							lvgl_msg_t msg;
							msg.type = MSG_TYPE_WEATHER;
                            // Current condition
                            cJSON *current = cJSON_GetObjectItem(weather, "current_condition");
                            if (current) {
                                const char *temp = cJSON_GetObjectItem(current, "temp_C")->valuestring;
                                const char *desc = cJSON_GetObjectItem(current, "weatherDesc")->valuestring;
                                ESP_LOGI("WEATHER", "Current:%s Desc:%s", temp, desc);
                                msg.weather.index_desc = get_img(desc);
                                snprintf(msg.weather.current_tmp, sizeof(msg.weather.current_tmp), "%s°C", temp);
                                
                            }

                            // Today forecast
                            cJSON *today = cJSON_GetObjectItem(weather, "today");
                            if (today) {
                                const char *maxC = cJSON_GetObjectItem(today, "max_temp_C")->valuestring;
                                const char *minC = cJSON_GetObjectItem(today, "min_temp_C")->valuestring;
                                const char *desc = cJSON_GetObjectItem(today, "weatherDesc")->valuestring;
                                ESP_LOGI("WEATHER", "Today Max:%s Min:%s Desc:%s", maxC, minC, desc);
                                
                                const char *max2show = maxC ? maxC : "N/A";
                                const char *min2show = minC ? minC : "N/A";
                                const char *desc2show = desc ? desc : "N/A";					
                                snprintf(msg.weather.today_desc, sizeof(msg.weather.today_desc), "Today: %s°C-%s°C\n%s", 
                                         max2show, min2show, desc2show);
                            }

                            // Future days
                            cJSON *future_days = cJSON_GetObjectItem(weather, "future_days");
                            if (future_days && cJSON_IsArray(future_days)) {
                                int size = cJSON_GetArraySize(future_days);
                                for (int i = 0; i < size; i++) {
                                    cJSON *day = cJSON_GetArrayItem(future_days, i);
                                    const char *date = cJSON_GetObjectItem(day, "date")->valuestring;
                                    const char *maxC = cJSON_GetObjectItem(day, "max_temp_C")->valuestring;
                                    const char *minC = cJSON_GetObjectItem(day, "min_temp_C")->valuestring;
                                    const char *desc = cJSON_GetObjectItem(day, "weatherDesc")->valuestring;
                                    ESP_LOGI("WEATHER", "Day %s Max:%s Min:%s Desc:%s", date, maxC, minC, desc);
                                    
                                    const char *date2show = date ? date : "N/A";
                                    const char *max2show = maxC ? maxC : "N/A";
                                    const char *min2show = minC ? minC : "N/A";
                                    const char *desc2show = desc ? desc : "N/A";
                                    
                                    if (i == 0) {
										snprintf(msg.weather.tomorrow_desc, sizeof(msg.weather.tomorrow_desc), "%s: %s°C-%s°C\n%s", 
                                             date2show, max2show, min2show, desc2show);                                        
                                    }
                                    if (i == 1) {
										snprintf(msg.weather.data_after_desc, sizeof(msg.weather.data_after_desc), "%s: %s°C-%s°C\n%s", 
                                             date2show, max2show, min2show, desc2show);
                                    }
                                }
                            }
                            xQueueSend(lvgl_update_queue, &msg, 0);
                        }
                    } else {
                        ESP_LOGW("WEATHER", "No weather data found in response");
                    }
					
                    cJSON_Delete(root);
                    offset = 0; // Reset for next request
                }
            }
            break;

        case HTTP_EVENT_ON_FINISH:
        case HTTP_EVENT_DISCONNECTED:
            offset = 0; // Reset in case of error
            break;

        default:
            break;
    }
    return ESP_OK;
}



void fetch_weather_gait()
{
	wifi_ap_record_t ap_info;  // declare a struct
    esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);  // pass its address

    if (ret != ESP_OK) {
		ESP_LOGW("WIFI", "Not connected to any AP, http is not available");
        return;
    }
    char post_data[256];  // Buffer to hold the JSON string
    snprintf(post_data, sizeof(post_data), 
             "{\"username\": \"%s\", \"stride\": %.2f, \"cadence\": %.2f, \"pace\": %.2f, \"steps\": %d}",
             USERNAME,
             msg.gait.stride_length,   // Use stride from msg.gait
             msg.gait.cadence,  // Use cadence from msg.gait
             msg.gait.pace,     // Use pace from msg.gait
             msg.gait.steps);   // Use steps from msg.gait
	
    esp_http_client_config_t config = {
	    .url = "http://52.74.153.124:18711/gait",
	    .event_handler = http_event_handler,
	    .timeout_ms = 15000
    };
	ESP_LOGI("HTTP", "url:%s\n", config.url);
    esp_http_client_handle_t client = esp_http_client_init(&config);
     // Set HTTP method to POST
    // Set HTTP POST method and headers
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    char content_length_str[16];  // Buffer for length string (sufficient for 64-bit integers)
    snprintf(content_length_str, sizeof(content_length_str), "%d", strlen(post_data));  // %zu for size_t
    esp_http_client_set_header(client, "Content-Length", content_length_str);  // Pass string here

    // Set POST payload (use strlen(post_data) for length, correct type: size_t)
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int content_length = esp_http_client_get_content_length(client);
        ESP_LOGI("HTTP", "content_length:%d\n",content_length);
    } else {
        ESP_LOGW("HTTP", "HTTP request failed: %s\n", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

static int total_firmware_size = 0;
static int downloaded_size = 0;
static bool ota_in_progress = false;


// Background OTA task
void ota_background_task(void *pvParameters) {
    ESP_LOGI("OTA", "Starting background OTA update...");
    ota_in_progress = true;
    downloaded_size = 0;
    total_firmware_size = 0;
    // WiFi connection check (simplified)
    wifi_ap_record_t ap_info;
    esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
    if (ret != ESP_OK) {

        ota_in_progress = false;
        vTaskDelete(NULL);
    }

    // HTTP client configuration
    esp_http_client_config_t config = {
        .url = "http://52.74.153.124:18711/TUPSmartBand.bin",
        .timeout_ms = 15000,
        .keep_alive_enable = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {

        ota_in_progress = false;
        vTaskDelete(NULL);
    }

    // Open HTTP connection
    ret = esp_http_client_open(client, 0);
    if (ret != ESP_OK) {
        char error_msg[64];
        printf(error_msg, "HTTP open failed: %s", esp_err_to_name(ret));
        esp_http_client_cleanup(client);
        ota_in_progress = false;
        vTaskDelete(NULL);
    }

    // Get content length
    total_firmware_size = esp_http_client_fetch_headers(client);
    if (total_firmware_size <= 0) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        ota_in_progress = false;
        vTaskDelete(NULL);
    }

    // Initialize OTA
    esp_ota_handle_t ota_handle;
    const esp_partition_t* update_partition = esp_ota_get_next_update_partition(NULL);
    if (!update_partition) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        ota_in_progress = false;
        vTaskDelete(NULL);
    }

    ret = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (ret != ESP_OK) {
        char error_msg[64];
        printf(error_msg, "OTA begin failed: %s", esp_err_to_name(ret));

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        ota_in_progress = false;
        vTaskDelete(NULL);
    }

    // Read and write firmware in chunks
    const int buffer_size = 8192;
    uint8_t* buffer = (uint8_t*)malloc(buffer_size);
    if (!buffer) {

        esp_ota_abort(ota_handle);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        ota_in_progress = false;
        vTaskDelete(NULL);
    }

    // Download loop with progress updates
    while (downloaded_size < total_firmware_size) {
        int read_length = esp_http_client_read(client, (char*)buffer, buffer_size);
        if (read_length <= 0) {
            char error_msg[64];
            printf(error_msg, "Download failed: %s\n", esp_err_to_name(read_length));

            free(buffer);
            esp_ota_abort(ota_handle);
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            ota_in_progress = false;
            vTaskDelete(NULL);
        }

        // Write to OTA partition
        ret = esp_ota_write(ota_handle, buffer, read_length);
        if (ret != ESP_OK) {
            char error_msg[64];
            printf(error_msg, "OTA write failed: %s\n", esp_err_to_name(ret));

            free(buffer);
            esp_ota_abort(ota_handle);
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            ota_in_progress = false;
            vTaskDelete(NULL);
        }

        // Update progress
        downloaded_size += read_length;
        
        // Update UI periodically (not too often to avoid flooding)
        if (downloaded_size % (buffer_size * 2) == 0) {
            printf("downloaded_size:%d/%d\n", downloaded_size, total_firmware_size);
            lvgl_msg_t msg = {};
		    msg.type = MSG_TYPE_UPDATE;
		    msg.update_percent = (int)(100*downloaded_size/total_firmware_size);
		    if(lvgl_update_queue){
				xQueueSend(lvgl_update_queue, &msg, 0);
			}
        }

        // Small delay to allow other tasks to run
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    // Cleanup
    free(buffer);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    // Finalize OTA
    ret = esp_ota_end(ota_handle);
    if (ret != ESP_OK) {

        ota_in_progress = false;
        vTaskDelete(NULL);
    }

    // Set boot partition
    ret = esp_ota_set_boot_partition(update_partition);
    if (ret != ESP_OK) {

        ota_in_progress = false;
        vTaskDelete(NULL);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    esp_restart();
}

// UI update function (runs in LVGL context)

void weather_task(void *pvParameter)
{
    while (1) {
        fetch_weather_gait();
        vTaskDelay(pdMS_TO_TICKS(10000)); // update every 60 seconds
    }
}

void init_weather(){
	BaseType_t ret = xTaskCreate(weather_task, "weather_task", 6144, NULL, 5, NULL);
	if(ret != pdPASS) {
	    ESP_LOGE("TASK", "Failed to create weather_task!");
	}
}

