#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_err.h"


#define SWITCH_1 4
#define SWITCH_2 2


esp_err_t enable_switches();


int read_switch(int switch_id);