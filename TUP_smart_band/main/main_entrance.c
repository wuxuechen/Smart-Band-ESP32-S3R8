#include "pcf85063.h"
#include "qmi8658c.h"
#include "gatt_server_service_table.h"
#include "lcd_touch.h"
#include "misc/lv_timer.h"
#include "ui/screens.h"
#include "wifi_connect.h"
#include "nvs_flash.h"
//#include "http_server.h"

#define I2C_MASTER_SCL 10
#define I2C_MASTER_SDA 11


void i2c_bus_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA,
        .scl_io_num = I2C_MASTER_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void app_main(void)
{
	
	i2c_bus_init();
	init_rtc();
	init_qmi8658();
	
	    /* Initialize NVS. this is very important for wifi and bluetooth*/
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
	
    init_bluetooth();
    
    wifi_cmd_queue = xQueueCreate(4, sizeof(wifi_cmd_t));
    xTaskCreate(wifi_task, "wifi_task", 3072, NULL, 5, NULL);
    
    ESP_ERROR_CHECK(app_lcd_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());
	app_touch_init();
	    /* Show LVGL objects */
    app_main_display();
    
}