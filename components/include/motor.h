#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

esp_err_t config_MotorA();

esp_err_t set_MotorA(int direction, uint32_t dutycycle);