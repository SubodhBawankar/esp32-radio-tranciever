#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"

#include "oled.h"
#include "u8g2.h"

static const char* TAG = "Poten+Oled";

#define POTEN_PIN 35
// Potentiometer Output Pin is connected to 35 Pin of ESP32

void config_adc(){

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    // ADC_CHANNEL_7 refers to 35 pin on 
    // esp32 where we have connected our potentiometer
    ESP_LOGI(TAG, "Config Done");
}

void character_adc(){

    int reading;
    reading = adc1_get_raw(ADC1_CHANNEL_7);
    ESP_LOGI(TAG, "Reading %d", reading);

    u8g2_t u8g2_structure;
    led_config(u8g2_structure);
    printvoltage(u8g2_structure, reading);

}



void app_main(void){

    config_adc();
    while (1){
        character_adc();
    }
}

