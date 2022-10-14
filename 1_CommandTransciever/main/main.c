#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/adc.h"

#include "motor.h"
#include "switch.h"
#include "spi2.h"


static const char* TAG = "Transciever";

int reading = 1;
int pwm;

void config_adc(){

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
    // ADC_CHANNEL_7 refers to 35 pin on 
    // esp32 where we have connected our potentiometer
}


void Transmitter(){
    NRF24_t device;
    SPI_Config(&device);
    uint8_t mydata = 0;
    uint8_t* ptr_data = &mydata;
    // ESP_LOGI(TAG, "\nPWM signal: %d", *ptr_data);
    uint8_t payload = sizeof(mydata);
    uint8_t channel = 90;
    Register_Config(&device, channel, payload);
    SetSpeedRates(&device, 2); // 2 -- 250kbps speed rate

    esp_err_t ret = setTADDR(&device, (uint8_t *)"ARYAN");
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Unable to set TADDR");
		while(1) { vTaskDelay(1); }
	}
    else{
        while (1){
            reading = adc1_get_raw(ADC1_CHANNEL_7);
            pwm = (reading*100) / 4095;
            mydata = pwm;
            if(true){
                ESP_LOGI(TAG, "\nPWM signal: %d", *ptr_data);
                ESP_LOGI(TAG, "Sending Data");
                Send_data(&device, ptr_data, payload);
                isSend(&device);
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            
        }
    }

}

void Reciever(){
    NRF24_t device;
    SPI_Config(&device);
    uint8_t reci_data = 0;
    uint8_t *ptr_reci_data = &reci_data;
    uint8_t payload = sizeof(reci_data);
    uint8_t channel = 90;
    Register_Config(&device, channel, payload);
    SetSpeedRates(&device, 2); // 250kbps speed rate
    
    esp_err_t ret = setRADDR(&device,  (uint8_t *)"ARYAN" );
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Unable to set RADDR");
		while(1) { vTaskDelay(1); }
	}
    ESP_LOGI(TAG, "Listening ");
    while(1){
        if(data_ready(&device)){

            // code to read data
            Get_Data(&device, ptr_reci_data, payload);
            ESP_LOGI(TAG, "\nRecieved PWM data: %d", *ptr_reci_data);
            set_MotorA(0, (float)reci_data);
        
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}

void app_main(void){
    enable_switches();

    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        if(read_switch(SWITCH_1)){
            config_adc();
            ESP_LOGI(TAG, "Switch 2: Transmitter Code");
            xTaskCreate(
                Transmitter,
                "Transmitter",
                1024*3,
                NULL,
                2,
                NULL
            );
        }

        else if(read_switch(SWITCH_2)){
            if (config_MotorA() == ESP_OK){
                ESP_LOGI(TAG, "Motors Config Done");    
            }
            ESP_LOGI(TAG, "Switch 2: Reciever Code");
            xTaskCreate(
                Reciever,
                "Reciever",
                1024*3,
                NULL,
                2,
                NULL
            );
        }
    }
}
