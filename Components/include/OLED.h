#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "u8g2.h"
#include "u8g2_esp32_hal.h"
#include "esp_log.h"

#define OLED_SDA 22
#define OLED_SCL 21

esp_err_t config_led(u8g2_t *p);
esp_err_t displayValue(u8g2_t *, int );
