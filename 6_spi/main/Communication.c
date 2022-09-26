#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "spi2.h"

static const char* TAG = "Communication.c";

void Transmitter(){
    SPI_Config();
    int mydata = 128;
    ESP_LOGI(TAG, "\nMy data is: %d", mydata);
    uint8_t payload = sizeof(mydata);
    uint8_t channel = 90;
    Register_Config(channel, payload);
    
    esp_err_t ret = setTADDR((uint8_t *)"FGHIJ");
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "nrf24l01 not installed");
		while(1) { vTaskDelay(1); }
	}
    else{
        ESP_LOGI(TAG, "Done Till now");
        while (1){
            
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            ESP_LOGI(TAG, "Sending Data");
            while(1) {
                Send_data(&mydata, payload);
                ESP_LOGI(TAG, "Data Send ------");
                vTaskDelay(3000/portTICK_PERIOD_MS);
	        }

        }
    }

}




void Reciever(){
    SPI_Config();
    uint8_t reci_data;
    
    uint8_t payload = sizeof(reci_data);
    uint8_t channel = 90;
    Register_Config(channel, payload);
    
    esp_err_t ret = setRADDR( (uint8_t *)"FGHIJ" );
	if (ret != ESP_OK) {
		ESP_LOGE(pcTaskGetName(0), "nrf24l01 not installed");
		while(1) { vTaskDelay(1); }
	}
    
    while(1){
        if(true){ //data is ready

            // code to read data
            data_ready();
            Get_Data(payload);
            // ESP_LOGI(TAG, "\nRecieved data is: %s", reci_data);
            
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}

void app_main(void){

    char choice;
    ESP_LOGI(TAG, "Enter 1 for Transmitter and 0 for Reciever");
    // scanf("%d", &choice);
    // choice = fscanf(stdin);
    choice = 1;
    if(choice == 1){
        xTaskCreate(
            Transmitter,
            "Transmitter",
            1024*3,
            NULL,
            2,
            NULL
        );
    }
    else if(choice == 0){
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
