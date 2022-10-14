// #include <string.h>

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

#include "http_handler.h"
#include "softAP.h"

static const char *TAG = "tuning_http_server";
static str_message_t message_obj = {.str_obj = "type a message"};
static char scratch[SCRATCH_BUFSIZE];

str_message read_mess()
{
    return message_obj;
}

static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
 char filepath[FILE_PATH_MAX] = WEB_MOUNT_POINT;

    if (strlen(req->uri) > 0 && req->uri[strlen(req->uri) - 1] == '/') 
    {
        strlcat(filepath, "/index.html", sizeof(filepath));
    }
    else 
    {
        strlcat(filepath, req->uri, sizeof(filepath));
    }

    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = scratch;
    memset(scratch, '\0', SCRATCH_BUFSIZE);
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}


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
            sprintf(response_string, "Message is : %s", received_message->valuestring);


            message_obj.str_obj = received_message->valuestring;
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
        .uri = "/string",
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
    static httpd_handle_t server = NULL;
    
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(connect());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(start_tuning_http_server_private());

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));


    vTaskDelete(NULL);

}