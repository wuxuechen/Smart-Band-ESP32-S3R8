#include "http_server.h"

// --- Global server handle ---
static httpd_handle_t server = NULL;
static const char *TAG_HTTP = "HTTP_SERVER";

lv_disp_t *lvgl_disp2cast = NULL;
// --- URI handler example ---
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char resp[] = "Hello from ESP32-S3!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


static esp_err_t cast_get_handler(httpd_req_t *req)
{
	const char resp[] = "it is difficult, because it the hardware is not strong enough!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	
    /*lv_disp_draw_buf_t *draw_buf = lv_disp_get_draw_buf(lvgl_disp2cast);

    if (!draw_buf) return ESP_FAIL;

    // BMP header
    uint32_t file_size = 54 + 240 * 280 * 2;
    uint8_t header[54] = {0};
    header[0] = 'B'; header[1] = 'M';
    header[2] = file_size & 0xFF;
    header[3] = (file_size >> 8) & 0xFF;
    header[4] = (file_size >> 16) & 0xFF;
    header[5] = (file_size >> 24) & 0xFF;
    header[10] = 54; header[14] = 40;
    header[18] = 240 & 0xFF; header[19] = (240 >> 8) & 0xFF;
    header[22] = 280 & 0xFF; header[23] = (280 >> 8) & 0xFF;
    header[26] = 1; header[28] = 16;
    
    

    httpd_resp_set_type(req, "image/bmp");
    httpd_resp_send_chunk(req, (const char*)header, sizeof(header));
     
     
    lv_color_t *buf = draw_buf->buf1;  // single buffer, already in RAM
    httpd_resp_send_chunk(req, (const char*)buf, 240*280*2);
    // Stream line by line
    #define CHUNK_LINES 200
	for (int y = 0; y < 280; y += CHUNK_LINES) {
	    int lines = (y + CHUNK_LINES > 280) ? (280 - y) : CHUNK_LINES;
	    lv_color_t *block = &draw_buf->buf1[y * 240];
	    httpd_resp_send_chunk(req, (const char*)block, 240 * lines * 2);
	}*/
    httpd_resp_send_chunk(req, NULL, 0); // end
    return ESP_OK;
}


// --- Server operations ---
typedef enum {
    SERVER_OP_START,
    SERVER_OP_STOP,
    SERVER_OP_RESTART
} server_operation_t;

typedef struct {
    server_operation_t op;
} server_task_param_t;

// --- Internal server task ---
static void server_task(void *pvParameters)
{
    server_task_param_t *param = (server_task_param_t *)pvParameters;
    if (!param) {
        vTaskDelete(NULL);
        return;
    }

    if (param->op == SERVER_OP_START) {
        if (server == NULL) {
            httpd_config_t config = HTTPD_DEFAULT_CONFIG();
            if (httpd_start(&server, &config) == ESP_OK) {
                httpd_uri_t hello_uri = {
                    .uri = "/",
                    .method = HTTP_GET,
                    .handler = hello_get_handler,
                    .user_ctx = NULL
                };
                httpd_register_uri_handler(server, &hello_uri);
                
                httpd_uri_t cast_uri = {
                    .uri = "/cast",
                    .method = HTTP_GET,
                    .handler = cast_get_handler,
                    .user_ctx = NULL
                };
                httpd_register_uri_handler(server, &cast_uri);
                
                ESP_LOGI(TAG_HTTP, "Server started on port %d", config.server_port);
            } else {
                ESP_LOGE(TAG_HTTP, "Failed to start server");
                server = NULL;
            }
        } else {
            ESP_LOGW(TAG_HTTP, "Server already running");
        }
    }
    else if (param->op == SERVER_OP_STOP) {
        if (server != NULL) {
            if (httpd_stop(server) == ESP_OK) {
                ESP_LOGI(TAG_HTTP, "Server stopped successfully");
                server = NULL;
            } else {
                ESP_LOGE(TAG_HTTP, "Failed to stop server");
            }
        } else {
            ESP_LOGW(TAG_HTTP, "Server not running");
        }
    }
    else if (param->op == SERVER_OP_RESTART) {
        if (server != NULL) {
            httpd_stop(server);
            server = NULL;
        }
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        if (httpd_start(&server, &config) == ESP_OK) {
            httpd_uri_t hello_uri = {
                .uri = "/",
                .method = HTTP_GET,
                .handler = hello_get_handler,
                .user_ctx = NULL
            };
            httpd_register_uri_handler(server, &hello_uri);
            ESP_LOGI(TAG_HTTP, "Server restarted on port %d", config.server_port);
        } else {
            ESP_LOGE(TAG_HTTP, "Failed to restart server");
            server = NULL;
        }
    }

    free(param);
    vTaskDelete(NULL);
}

// --- Public API ---
// Non-blocking, safe for Wi-Fi callbacks
void start_http_server(void)
{
    server_task_param_t *param = malloc(sizeof(server_task_param_t));
    if (!param) {
        ESP_LOGE(TAG_HTTP, "Memory allocation failed for server task!");
        return;
    }
    param->op = SERVER_OP_START;

    if (xTaskCreate(server_task, "server_task_start", 4096, param, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG_HTTP, "Failed to create server start task!");
        free(param);
    }
}

esp_err_t stop_http_server(void)
{
    server_task_param_t *param = malloc(sizeof(server_task_param_t));
    if (!param) {
        ESP_LOGE(TAG_HTTP, "Memory allocation failed for server task!");
        return ESP_ERR_NO_MEM;
    }
    param->op = SERVER_OP_STOP;

    if (xTaskCreate(server_task, "server_task_stop", 4096, param, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG_HTTP, "Failed to create server stop task!");
        free(param);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t restart_http_server(void)
{
    server_task_param_t *param = malloc(sizeof(server_task_param_t));
    if (!param) {
        ESP_LOGE(TAG_HTTP, "Memory allocation failed for server task!");
        return ESP_ERR_NO_MEM;
    }
    param->op = SERVER_OP_RESTART;

    if (xTaskCreate(server_task, "server_task_restart", 4096, param, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG_HTTP, "Failed to create server restart task!");
        free(param);
        return ESP_FAIL;
    }

    return ESP_OK;
}
