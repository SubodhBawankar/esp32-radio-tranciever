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

#include "softAP.h"

void start_tuning_http_server();
