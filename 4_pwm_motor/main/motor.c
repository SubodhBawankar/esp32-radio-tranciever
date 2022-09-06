#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "motor.h"

void app_main(void){
    ESP_LOGI("MOTOR", "In app_main");
    int reading;
    reading = 0;
    config_MotorA();
    for (reading = 100; reading <= 255; reading++){
        set_MotorA(1, reading);
    }
}
