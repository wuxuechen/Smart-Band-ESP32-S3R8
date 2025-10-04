#ifndef IO_FLASH_H
#define IO_FLASH_H

#include "nvs_flash.h"
#include "nvs.h"
#include "lcd_touch.h"
#include "esp_log.h"

void save_data_to_nvs(const char *key, const char *value);

void read_data_from_nvs(const char *key, char *value, size_t size);

void parse_and_save(const char *input);

#endif // IO_FLASH_H