#include "pcf85063.h"
#include "gatt_server_service_table.h"
#include "lcd_touch.h"
#include "misc/lv_timer.h"
#include "ui/screens.h"


void app_main(void)
{
	init_rtc();
    init_bluetooth();
    ESP_ERROR_CHECK(app_lcd_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());
	app_touch_init();
	    /* Show LVGL objects */
    app_main_display();

}