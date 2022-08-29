#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define POTEN_PIN 32
// Potentiometer Output Pin is connected to 32 Pin of ESP32

void app_main(){
    int64_t bit_mask = 0;
    bit_mask = (1ULL << POTEN_PIN);

    gpio_config_t config_variable;
    config_variable.pin_bit_mask = bit_mask;
    config_variable.mode = GPIO_MODE_INPUT;
    config_variable.pull_up_en = 0;
    config_variable.pull_down_en = 0; // 
    config_variable.intr_type = GPIO_INTR_DISABLE;

    esp_err_t err = gpio_config(&config_variable);

    if (err == ESP_OK){
        ESP_LOGI("GPIO Config Done");
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC_CHANNEL_4, ADC_ATTEN_DB_0);
        int reading;

        while (1){
            reading = adc1_get_raw(ADC_CHANNEL_4);
            ESP_LOGI("Reading %d", reading);
        }
    }
    else{
        ESP_LOGE("ERROR: ", esp_err_to_name(err));
    }


}
