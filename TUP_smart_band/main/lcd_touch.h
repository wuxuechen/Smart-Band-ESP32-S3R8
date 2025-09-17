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

typedef enum { QMI_RESULT_OK = 0, QMI_RESULT_ERROR } qmi_result_t;
typedef struct {
    float stride_length;   // meters
    float cadence;         // steps/min
    float pace;            // min/km
    float ground_contact;  // ms
    int steps;
    float temperature;
    char info[128];
} gait_metrics_t;

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t weekday;
    uint8_t month;
    uint8_t year;
} rtc_time_t;

typedef enum { MSG_TYPE_GAIT = 0, MSG_TYPE_RTC } lvgl_mgs_type_t;

typedef struct{
	lvgl_mgs_type_t type;
	union {
        gait_metrics_t gait;
        rtc_time_t rtc;
    };
} lvgl_msg_t;


extern QueueHandle_t lvgl_update_queue;

esp_err_t app_lvgl_init(void);

esp_err_t app_lcd_init(void);

void app_touch_init();

void app_main_display(void);

void lvgl_update_task(void *arg);

#endif //LCD_TP_MANAGER_H
