#ifndef HTTP_SERVER_HEADER_H
#define HTTP_SERVER_HEADER_H

#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
esp_err_t hello_get_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
void http_server_task(void *pvParameter);
void init_http_server(void);

#endif // HTTP_SERVER_HEADER_H