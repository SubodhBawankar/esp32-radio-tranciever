#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <esp_system.h>
#include <sys/param.h>
#include <nvs_flash.h>
#include "esp_netif.h"
#include <esp_http_server.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "softAP.h"
#include "http_handler.h"

static const char *TAG = "http_handler";

static esp_err_t string_handler(httpd_req_t *req)
{
   esp_err_t ret;
   ESP_LOGI(TAG,"Send message");
   const char* res = (const char* ) req->user_ctx;
   ret = httpd_resp_send(req, res, strlen(res));
   if(ret != ESP_OK)
   {
    ESP_LOGI(TAG,"Error %d ",ret);
   }
   else
   {
    ESP_LOGI(TAG, "Response sent successfully");
   }
   return ret;
}

static const httpd_uri_t string = {
    .uri       = "/string",
    .method    = HTTP_GET,
    .handler   = string_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = //html or null
};

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "ERROR : 404");
    return ESP_FAIL;
}

void handler()
{
    static httpd_handle_t server = NULL;
    
   //Initialize NVS
    connect();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
    //ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
}
