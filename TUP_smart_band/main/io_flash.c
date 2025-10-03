#include "io_flash.h"
#include "lcd_touch.h"
#include <stdio.h>
#include <string.h>

void save_data_to_nvs(const char *key, const char *value) {
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open NVS storage (namespace "storage")
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error opening NVS: %s\n", esp_err_to_name(err));
        return;
    }

    // Write string value
    err = nvs_set_str(my_handle, key, value);
    if (err == ESP_OK) {
        nvs_commit(my_handle);   // commit is required!
    }

    nvs_close(my_handle);
}

void read_data_from_nvs(const char *key, char *value, size_t size) {
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
        printf("Error opening NVS: %s\n", esp_err_to_name(err));
        return;
    }

    // First get required buffer size
    size_t required_size;
    err = nvs_get_str(my_handle, key, NULL, &required_size);
    if (err == ESP_OK) {
        char *tmp = malloc(required_size);
        nvs_get_str(my_handle, key, tmp, &required_size);
        printf("Read from NVS: %s = %s\n", key, tmp);
        snprintf(value, size, "%s", tmp);
        free(tmp);
    } else {
        printf("No value found for %s\n", key);
    }

    nvs_close(my_handle);
}

void parse_and_save(const char *input) {
    if (strncmp(input, "USER:", 5) == 0) {
        // Key is "USER", value after ':'
        save_data_to_nvs("USER", input + 5);
        lvgl_msg_t msg = {};
	    msg.type = MSG_TYPE_USERNAME;
	    if(lvgl_update_queue){
			xQueueSend(lvgl_update_queue, &msg, 0);
		}
    }
    else if (strncmp(input, "WIFI:", 5) == 0) {
        // Key is "WIFI", value after ':'
        save_data_to_nvs("WIFI", input + 5);
        init_variables();
    }
    else {
        printf("Ignored input: %s\n", input);
    }
}
