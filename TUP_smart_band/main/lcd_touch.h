/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef LCD_TP_MANAGER_H
#define LCD_TP_MANAGER_H
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lvgl_port.h"
#include "lv_demos.h"
#include "esp_lcd_touch_cst816s.h"
#include "ui/screens.h"
#include "ui/ui.h"
#include <lvgl.h>

/* LCD size */
#define LCD_H_RES (240)
#define LCD_V_RES (280)

esp_err_t app_lvgl_init(void);

esp_err_t app_lcd_init(void);

void app_touch_init();

void app_main_display(void);

#endif //LCD_TP_MANAGER_H
