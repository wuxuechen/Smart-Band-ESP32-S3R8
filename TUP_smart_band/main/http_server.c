#include "http_server.h"

esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char resp[] = "Hello from ESP32-S3!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // default port 80
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t hello_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = hello_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &hello_uri);
        ESP_LOGI("HTTP_SERVER", "Server started on port %d", config.server_port);
    }
    else
    {
        ESP_LOGE("HTTP_SERVER", "Failed to start server");
    }

    return server;
}

void http_server_task(void *pvParameter)
{
    start_webserver();
    // server runs in background tasks, this task can optionally do other things
    vTaskDelete(NULL); // delete this task if nothing else is needed
}

void init_http_server(void){
	    xTaskCreate(
        http_server_task,    // task function
        "HTTP_Server_Task",  // task name
        4096,                // stack size (adjust if needed)
        NULL,                // task parameter
        5,                   // priority
        NULL                 // task handle
    );
	
}
