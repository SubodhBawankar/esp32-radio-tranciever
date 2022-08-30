#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

static const char* TAG = "POTENTIOMETER ";

#define DEFAULT_VREF 1100

#define POTEN_PIN 32
// Potentiometer Output Pin is connected to 32 Pin of ESP32

void config_adc(){

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL_4, ADC_ATTEN_DB_0);
    // ADC_CHANNEL_4 refers to 32 pin on 
    // esp32 where we have connected our potentiometer
    ESP_LOGI(TAG, "Config Done");
}

void character_adc(){
    esp_adc_cal_characteristics_t *adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));

    // esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t adc_val = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    
    uint32_t reading;
    uint32_t voltage;
    reading = adc1_get_raw(ADC_CHANNEL_4);
    ESP_LOGI(TAG, "Reading %d", reading);
    voltage = esp_adc_cal_raw_to_voltage(reading, adc_chars);

    ESP_LOGI(TAG, "Voltage %d", voltage);
}



void app_main(void){

    config_adc();
    
    while (1){
        character_adc();
    }
}
