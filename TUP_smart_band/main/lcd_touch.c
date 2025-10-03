/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include "lcd_touch.h"
#include "constant.h"
#include "core/lv_obj_pos.h"
#include "http_server.h"
#include "misc/lv_anim.h"
#include "widgets/lv_arc.h"
#include "widgets/lv_label.h"
#include <string.h>
#include "ui/images.h"
#include "io_flash.h"
/* LCD settings */
#define LCD_SPI_NUM (SPI2_HOST)
#define LCD_PIXEL_CLK_HZ (40 * 1000 * 1000)
#define LCD_CMD_BITS (8)
#define LCD_PARAM_BITS (8)
#define LCD_COLOR_SPACE (ESP_LCD_COLOR_SPACE_RGB)
#define LCD_BITS_PER_PIXEL (16)
#define LCD_DRAW_BUFF_DOUBLE (1)
#define LCD_DRAW_BUFF_HEIGHT (50)
#define LCD_BL_ON_LEVEL (1)

/* LCD pins */
#define LCD_GPIO_SCLK (GPIO_NUM_6)
#define LCD_GPIO_MOSI (GPIO_NUM_7)
#define LCD_GPIO_RST (GPIO_NUM_8)
#define LCD_GPIO_DC (GPIO_NUM_4)
#define LCD_GPIO_CS (GPIO_NUM_5)
#define LCD_GPIO_BL (GPIO_NUM_15)

#define USE_TOUCH 1

#define TOUCH_HOST I2C_NUM_0

#if USE_TOUCH
#define PIN_NUM_TOUCH_SCL (GPIO_NUM_10)
#define PIN_NUM_TOUCH_SDA (GPIO_NUM_11)
#define PIN_NUM_TOUCH_RST (GPIO_NUM_13)
#define PIN_NUM_TOUCH_INT (GPIO_NUM_14)

esp_lcd_touch_handle_t tp = NULL;
#endif

static const char *TAG = "LCD_TP";

/* LCD IO and panel */
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;

static lv_display_t *lvgl_disp = NULL;

/* LVGL display and touch */

esp_err_t app_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    /* LCD backlight */
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_GPIO_BL};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    /* LCD initialization */
    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = LCD_GPIO_SCLK,
        .mosi_io_num = LCD_GPIO_MOSI,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = LCD_H_RES * LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");

    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_GPIO_DC,
        .cs_gpio_num = LCD_GPIO_CS,
        .pclk_hz = LCD_PIXEL_CLK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_NUM, &io_config, &lcd_io), err, TAG, "New panel IO failed");

    ESP_LOGD(TAG, "Install LCD driver");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_GPIO_RST,
        .color_space = LCD_COLOR_SPACE,
        .bits_per_pixel = LCD_BITS_PER_PIXEL,
    };
    
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7789(lcd_io, &panel_config, &lcd_panel), err, TAG, "New panel failed");

    esp_lcd_panel_reset(lcd_panel);
    esp_lcd_panel_init(lcd_panel);
    esp_lcd_panel_mirror(lcd_panel, true, true);
    esp_lcd_panel_disp_on_off(lcd_panel, true);

    /* LCD backlight on */
    ESP_ERROR_CHECK(gpio_set_level(LCD_GPIO_BL, LCD_BL_ON_LEVEL));

    esp_lcd_panel_set_gap(lcd_panel, 0, 20);
    esp_lcd_panel_invert_color(lcd_panel, true);

    return ret;

err:
    if (lcd_panel)
    {
        esp_lcd_panel_del(lcd_panel);
    }
    if (lcd_io)
    {
        esp_lcd_panel_io_del(lcd_io);
    }
    spi_bus_free(LCD_SPI_NUM);
    return ret;
}

#if USE_TOUCH
static void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t)drv->user_data;
    assert(tp);

    uint16_t tp_x;
    uint16_t tp_y;
    uint8_t tp_cnt = 0;
    /* Read data from touch controller into memory */
    esp_lcd_touch_read_data(tp);
    /* Read data from touch controller */
    bool tp_pressed = esp_lcd_touch_get_coordinates(tp, &tp_x, &tp_y, NULL, &tp_cnt, 1);
    if (tp_pressed && tp_cnt > 0)
    {
        data->point.x = tp_x;
        data->point.y = tp_y;
        data->state = LV_INDEV_STATE_PRESSED;
        ESP_LOGD(TAG, "Touch position: %d,%d", tp_x, tp_y);
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
#endif

esp_err_t app_lvgl_init(void)
{
    /* Initialize LVGL */
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,       /* LVGL task priority */
        .task_stack = 4096,       /* LVGL task stack size */
        .task_affinity = -1,      /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500, /* Maximum sleep in LVGL task */
        .timer_period_ms = 5      /* LVGL timer tick period in ms */
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = lcd_io,
        .panel_handle = lcd_panel,
        .buffer_size = LCD_H_RES * LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
        .double_buffer = LCD_DRAW_BUFF_DOUBLE,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
        }};
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);
    lvgl_disp2cast = lvgl_disp;

    return ESP_OK;
}

void app_touch_init(){
	#if USE_TOUCH
    ESP_LOGI(TAG, "Initialize I2C bus");
    esp_log_level_set("lcd_panel.io.i2c", ESP_LOG_NONE);
    esp_log_level_set("CST816S", ESP_LOG_NONE);
/*    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = PIN_NUM_TOUCH_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = PIN_NUM_TOUCH_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100 * 1000,
    };
    i2c_param_config(TOUCH_HOST, &i2c_conf);

    i2c_driver_install(TOUCH_HOST, i2c_conf.mode, 0, 0, 0);*/

    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    // Attach the TOUCH to the I2C bus
    esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)TOUCH_HOST, &tp_io_config, &tp_io_handle);

    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_H_RES,
        .y_max = LCD_V_RES,
        .rst_gpio_num = PIN_NUM_TOUCH_RST,
        .int_gpio_num = PIN_NUM_TOUCH_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    ESP_LOGI(TAG, "Initialize touch controller");
    esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &tp);
#endif


#if USE_TOUCH
    static lv_indev_drv_t indev_drv; // Input device driver (Touch)
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = lvgl_disp;
    indev_drv.read_cb = lvgl_touch_cb;
    indev_drv.user_data = tp;
    lv_indev_drv_register(&indev_drv);
#endif
}

const char *weekdays[7] = {
	"Su",
    "Mo", 
    "Tu", 
    "We", 
    "Th", 
    "Fr", 
    "Sa"
};

const char *months[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};




QueueHandle_t lvgl_update_queue = NULL;

void lvgl_update_task(void *arg) {
    lvgl_msg_t msg;
    const user_objs_t label_objs[9]=
	{
		{objects.activity_rank_name_0, objects.activity_rank_steps_0},
	    {objects.activity_rank_name_1, objects.activity_rank_steps_1},
	    {objects.activity_rank_name_2, objects.activity_rank_steps_2},
	    {objects.activity_rank_name_3, objects.activity_rank_steps_3},
	    {objects.activity_rank_name_4, objects.activity_rank_steps_4},
	    {objects.activity_rank_name_5, objects.activity_rank_steps_5},
	    {objects.activity_rank_name_6, objects.activity_rank_steps_6},
	    {objects.activity_rank_name_7, objects.activity_rank_steps_7},
	    {objects.activity_rank_name_8, objects.activity_rank_steps_8}
	};
    while(1) {
        if(xQueueReceive(lvgl_update_queue, &msg, pdMS_TO_TICKS(100))) {
			lvgl_port_lock(-1); 
			if (msg.type == MSG_TYPE_GAIT){
				gait_metrics_t *g = &msg.gait;
				if(objects.activity_stride)
                lv_label_set_text_fmt(objects.activity_stride, "%.2f", g->stride_length);
	            if(objects.activity_cadence)
	                lv_label_set_text_fmt(objects.activity_cadence, "%.2f", g->cadence);
	            if(objects.activity_pace)
	                lv_label_set_text_fmt(objects.activity_pace, "%.2f", g->pace);
	            if(objects.activity_steps)
	                lv_label_set_text_fmt(objects.activity_steps, "%d", g->steps);
	            if(objects.activity_label){
					lv_label_set_text(objects.activity_label, g->info);	
				}
				if(objects.setttings_tmp_label){
					lv_label_set_text_fmt(objects.setttings_tmp_label, "%.1f", g->temperature);
				}
			}
			if (msg.type == MSG_TYPE_RTC) {
				rtc_time_t *t = &msg.rtc;
				if(objects.home_time){
					lv_label_set_text_fmt(objects.home_time, "%02d:%02d:%02d", t->hours, t->minutes, t->seconds);
				}
				if(objects.home_date){
					lv_label_set_text_fmt(objects.home_date, "%s %02d %s", months[t->month], t->day,  weekdays[t->weekday]);
					lv_calendar_set_showed_date(objects.calendar_calendar, t->year, t->month+1);
					lv_calendar_set_today_date(objects.calendar_calendar, t->year, t->month+1, t->day);
				}				
			}
			
			if (msg.type == MSG_TYPE_TOP9) {
				for(int i = 0; i < 9; i++){
					if(strcmp("-1", msg.score[i].name)==0)
					{
						break;
					}
					if(label_objs[i].label_username){
						lv_label_set_text(label_objs[i].label_username, msg.score[i].name);
					}
					if(label_objs[i].label_steps){
						lv_label_set_text_fmt(label_objs[i].label_steps, "%d", msg.score[i].steps);
					}
				}
			}
			
			if (msg.type == MSG_TYPE_UPDATE){
				if(objects.ota_update_percent){
					lv_arc_set_value(objects.ota_update_percent, msg.update_percent);
					lv_label_set_text_fmt(objects.ota_update_percent_label, "%d%%", msg.update_percent);
				}
			}
			if(msg.type == MSG_TYPE_WEATHER){
				int index = msg.weather.index_desc;
                if (index == -1) {
                    lv_label_set_text(objects.weather_current_label, "sunny");
                    lv_label_set_text(objects.home_weather, "sunny");
                    lv_img_set_src(objects.weather_current, &img_sun);
                    lv_img_set_src(objects.home_weather_img, &img_sun);
                } else {
                    lv_label_set_text(objects.weather_current_label, images[index].name);
                    lv_label_set_text(objects.home_weather, images[index].name);
                    lv_img_set_src(objects.weather_current, images[index].img_dsc);
                    lv_img_set_src(objects.home_weather_img, images[index].img_dsc);
                }
                lv_label_set_text_fmt(objects.weather_current_tmp_label, "%s°C", msg.weather.current_tmp);
                lv_label_set_text(objects.weather_today, msg.weather.today_desc);
				lv_label_set_text(objects.weather_tomorrow, msg.weather.tomorrow_desc);
				lv_label_set_text(objects.weather_after, msg.weather.data_after_desc);
			}
			
			if (msg.type == MSG_TYPE_USERNAME){
				init_ui_variables();
			}

			lvgl_port_unlock();  // unlock after LVGL update
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // LVGL task tick
    }
}

void init_ui_variables(){
	lv_label_set_text(objects.settings_version, version);
   	read_data_from_nvs("USER",USERNAME,DATA_SIZE);
   	lv_label_set_text(objects.setttings_username,USERNAME);	
	// Center text inside the label
	lv_obj_set_style_text_align(objects.setttings_username, LV_TEXT_ALIGN_CENTER, 0);
	
	// Keep Y fixed
	lv_coord_t current_y = 32;
	lv_obj_set_y(objects.setttings_username, current_y);
}

void init_variables(){
	char ssid_pswd[DATA_SIZE*2] = {0};
	read_data_from_nvs("WIFI",ssid_pswd,DATA_SIZE*2);
	char *comma_pos = strchr(ssid_pswd, ',');
    if (comma_pos != NULL) {
        // Copy first part to WIFI_SSID
        size_t ssid_len = comma_pos - ssid_pswd;
        if (ssid_len >= DATA_SIZE) ssid_len = DATA_SIZE - 1;
        strncpy(WIFI_SSID, ssid_pswd, ssid_len);
        WIFI_SSID[ssid_len] = '\0';
        // Copy second part to WIFI_PASS
        strncpy(WIFI_PASS, comma_pos + 1, DATA_SIZE - 1);
        WIFI_PASS[DATA_SIZE - 1] = '\0';
    }
    printf("SSID: %s\n", WIFI_SSID);
    printf("PASS: %s\n", WIFI_PASS);
}

void app_main_display(void)
{
    /* Task lock */
    lvgl_port_lock(-1);
    ui_init();
    init_ui_variables();
    init_variables();
    lvgl_update_queue = xQueueCreate(10, sizeof(lvgl_msg_t));
    if (!lvgl_update_queue) {
	    ESP_LOGE(TAG, "Failed to create LVGL update queue");
	}
    xTaskCreate(lvgl_update_task, "LVGL_Update", 4096, NULL, 5, NULL);
    /* Task unlock */
    lvgl_port_unlock();
}

