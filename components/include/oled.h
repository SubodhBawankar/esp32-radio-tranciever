#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "u8g2.h"
#include "u8g2_esp32_hal.h"
#include "esp_log.h"

#define OLED_SDA 22
#define OLED_SCL 21
/*
uint8_t u8g2_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr){
    
    return 1;
}
*/

esp_err_t led_config(u8g2_t config_u8g2);

esp_err_t printvoltage(u8g2_t , int );