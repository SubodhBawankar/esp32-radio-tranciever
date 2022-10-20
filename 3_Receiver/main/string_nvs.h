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
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"


#include "esp_http_server.h"

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)
#define WEB_MOUNT_POINT "/www"
#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

// #include "softAP.h"

typedef struct str_message
{
    char *str;
}str_message_t;

void start_tuning_http_server();
str_message_t read_message();