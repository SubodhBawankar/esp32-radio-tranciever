#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "switch.h"
#include "spi2.h"


static const char* TAG = "Transciever";


void Transmitter(){
    NRF24_t device;
    SPI_Config(&device);
    uint8_t mydata = 65;
    uint8_t* ptr_data = &mydata;

    ESP_LOGI(TAG, "\nMy data is: %d", *ptr_data);
    uint8_t payload = sizeof(mydata);
    uint8_t channel = 90;
    Register_Config(&device, channel, payload);
    SetSpeedRates(&device, 2); // 250kbps speed rate

    esp_err_t ret = setTADDR(&device, (uint8_t *)"ARYAN");
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "nrf24l01 not installed");
		while(1) { vTaskDelay(1); }
	}
    else{
        ESP_LOGI(TAG, "Done Till now");
        while (1){
            
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            ESP_LOGI(TAG, "Sending Data");
            Send_data(&device, ptr_data, payload);
            ESP_LOGI(TAG, "Wait for sending");
            isSend(&device);
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
		ESP_LOGE(pcTaskGetName(0), "nrf24l01 not installed");
		while(1) { vTaskDelay(1); }
	}
    ESP_LOGI(TAG, "Listening ");
    while(1){
        if(data_ready(&device)){

            // code to read data
            Get_Data(&device, ptr_reci_data, payload);
            ESP_LOGI(TAG, "\nRecieved data is: %c", (char )*ptr_reci_data);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}

void app_main(void){
    enable_switches();

    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        if(read_switch(SWITCH_1)){
            ESP_LOGI(TAG, "Switch 1: Transmitter Code");
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