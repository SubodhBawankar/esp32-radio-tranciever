#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"

#include "motor.h"
#include "oled.h"
#include "u8g2.h"

static const char* TAG = "Motor Control";

int reading = 1;
int pwm;

void config_adc(){

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    // ADC_CHANNEL_7 refers to 35 pin on 
    // esp32 where we have connected our potentiometer
}

void character_adc(){
    while(1){
        reading = adc1_get_raw(ADC1_CHANNEL_7);
        pwm = (reading*100) / 4095;
        ESP_LOGI(TAG, "Reading %d", reading);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void motor_control(){
    
    // converting reading from range of 0 to 4095 to pwm range 100 to 225
    // reading ---- 4095 (0-4095)

    while (1){    
        
        set_MotorA(0, pwm);
        ESP_LOGI(TAG, "Duty Cycle = %d", pwm);
        
        // 0 - forward; pwm - duty cycle
        vTaskDelay(100 / portTICK_PERIOD_MS); 

               
    }
}

void app_main(void){

    config_adc();
    if (config_MotorA() == ESP_OK){
        ESP_LOGI(TAG, "Motors Config Done");    
    }
    
    xTaskCreate( 
        character_adc, 
        "character_adc", 
        2048, 
        NULL, 
        2, 
        NULL
    );
    
    
    xTaskCreate(
        motor_control, 
        "motor_control", 
        2048, 
        NULL, 
        1, 
        NULL
    );
}

