#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

void config_motorA();

void set_MotorA();