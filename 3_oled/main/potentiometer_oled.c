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

int reading = 1;

void config_adc(){

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    // ADC_CHANNEL_7 refers to 35 pin on 
    // esp32 where we have connected our potentiometer
}

void oled_display(){
    
    u8g2_t u8g2_structure;
    led_config(&u8g2_structure);
    ESP_LOGI(TAG, "OLED Config Done");
    while(1){
        printvoltage(&u8g2_structure, reading);
        vTaskDelay(100 / portTICK_PERIOD_MS)
    }
}

void character_adc(){
    while(1){
        reading = adc1_get_raw(ADC1_CHANNEL_7);
        ESP_LOGI(TAG, "Reading %d", reading);
        vTaskDelay(10 / portTICK_PERIOD_MS)
    }
}

void app_main(void){

    config_adc();
    ESP_LOGI(TAG, "Adc Config Done");    
    xTaskCreate( 
        character_adc, 
        "character_adc", 
        2048, 
        NULL, 
        1, 
        NULL
    );
    
    xTaskCreate( 
        oled_display, 
        "oled_display", 
        2048, 
        NULL, 
        2, 
        NULL
    );
}

