#include "gatt_server_service_table.h"
#include "lcd_touch.h"

void app_main(void)
{
    init_bluetooth();
    
    ESP_ERROR_CHECK(app_lcd_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());
	app_touch_init();

    /* Show LVGL objects */
    app_main_display();
}