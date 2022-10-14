#include "tuning_http_server.h"
#include <string.h>

#include <math.h>
// #include "main.c"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "wifi_handler.h"

static const char *TAG = "tuning_http_server";



/* Simple handler for on button press handling */
static esp_err_t click_post_handler(httpd_req_t *req)
{
    
     if (req->content_len == 0)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid json");
        return ESP_OK;
    }
    char *buffer = malloc(req->content_len) + 1;
    memset(buffer, 0, req->content_len + 1);

    httpd_req_recv(req, buffer, req->content_len);

    
    cJSON *json = cJSON_Parse(buffer);
    // for extracting data
    if (json)
   {
        cJSON *received_message = cJSON_GetObjectItemCaseSensitive(json, "motion");
        if (received_message)
        {


            esp_err_t err1 = nvs_flash_init();
            nvs_handle set_str_handle;
            err1 = nvs_open("storage", NVS_READWRITE, &set_str_handle);
            char *response_string = malloc(strlen(received_message->valuestring) + 200);
            sprintf(response_string, "Motion is : %s", received_message->valuestring);

            char *string_buffer = received_message->valuestring;
           
            err1 = nvs_set_str(set_str_handle, "string_buffer", (const char*)string_buffer);
            err1 = nvs_commit(set_str_handle);
            // nvs_close(set_str_handle);
            
            size_t required_size = 100;
            char* message = malloc(required_size);
            nvs_get_str(set_str_handle, "string_buffer",message,&required_size);
            nvs_close(set_str_handle);
            // nvs_get_str(set_str_handle, "string_buffer", bluetooth_name, &required_size);
            ESP_LOGE(TAG, "NVS_DATA %s", message);
            
            return ESP_OK;
        }
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "message missing");
        cJSON_Delete(json);
        
        return ESP_OK;
    }
   
   httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid json");
    return ESP_OK;
}

static esp_err_t start_tuning_http_server_private()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP Server");
    if (httpd_start(&server, &config) != ESP_OK)
    {
        ESP_LOGE(TAG, "start server failed");
        return ESP_FAIL;
    }
    
    httpd_uri_t click_post_uri = {
        .uri = "/api/motion",
        .method = HTTP_POST,
        .handler = click_post_handler,
        .user_ctx = NULL
    };
 
   if (httpd_register_uri_handler(server, &click_post_uri) != ESP_OK)
    {
        ESP_LOGE(TAG, "register post uri failed");
        return ESP_FAIL;
    }

    return ESP_OK;
}





void start_tuning_http_server()
{
    
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(connect_to_wifi());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(start_tuning_http_server_private());

    vTaskDelete(NULL);

}