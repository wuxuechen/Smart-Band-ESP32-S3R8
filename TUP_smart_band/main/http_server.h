#ifndef HTTP_SERVER_HEADER_H
#define HTTP_SERVER_HEADER_H

#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_lcd_touch_cst816s.h"
#include <lvgl.h>

#ifndef DISP2CAST
#define DISP2CAST
	extern lv_disp_t *lvgl_disp2cast;  // declaration only
#endif
void http_server_task(void *pvParameter);
void start_http_server(void);
esp_err_t stop_http_server(void);
esp_err_t restart_http_server(void);

#endif // HTTP_SERVER_HEADER_H