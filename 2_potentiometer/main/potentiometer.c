#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

static const char* TAG = "POTENTIOMETER ";

#define POTEN_PIN 35
// Potentiometer Output Pin is connected to 32 Pin of ESP32

void config_adc(){

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    // ADC_CHANNEL_7 refers to 35 pin on 
    // esp32 where we have connected our potentiometer
    ESP_LOGI(TAG, "Config Done");
}

void character_adc(){

    uint32_t reading;
    reading = adc1_get_raw(ADC1_CHANNEL_7);
    ESP_LOGI(TAG, "Reading %d", reading);
}



void app_main(void){

    config_adc();
    
    while (1){
        character_adc();
    }
}
