#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_mac.h"
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
#include "cJSON.h"



void connect();