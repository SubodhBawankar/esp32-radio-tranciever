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
    SPI_Config();
    uint8_t mydata[3];
    mydata[0] = 128;
    ESP_LOGI(TAG, "\nMy data is: %d", (int )mydata);
    uint8_t payload = sizeof(mydata);
    uint8_t channel = 50;
    Register_Config(channel, payload);
    
    esp_err_t ret = setTADDR( (uint8_t *)"SRAVJ");
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "nrf24l01 not installed");
		while(1) { vTaskDelay(1); }
	}
    else{
        ESP_LOGI(TAG, "Done Till now");
        while (1){
            
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            ESP_LOGI(TAG, "Sending Data");
            Send_data(&mydata, payload);
            ESP_LOGI(TAG, "Wait for sending");
            if (isSend()) {
			    ESP_LOGI(TAG, "Send success");
		    } 
            else {
			    ESP_LOGW(TAG ,"Send fail:");
		    }
        }
    }

}

void Reciever(){
    SPI_Config();
    uint8_t reci_data[3];
    // int reci_data[3];
    
    uint8_t payload = sizeof(reci_data);
    uint8_t channel = 50;
    Register_Config(channel, payload);
    
    esp_err_t ret = setRADDR( (uint8_t *)"SRAVJ" );
	if (ret != ESP_OK) {
		ESP_LOGE(pcTaskGetName(0), "nrf24l01 not installed");
		while(1) { vTaskDelay(1); }
	}
    ESP_LOGI(TAG, "Listening ");
    while(1){
        if(data_ready()){

            // code to read data
            Get_Data(&reci_data, payload);
            ESP_LOGI(TAG, "\nRecieved data is: %d", (int )reci_data[0]);
            
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
