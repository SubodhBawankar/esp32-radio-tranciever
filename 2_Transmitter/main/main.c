#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "switch.h"
#include "spi2.h"
#include "string_nvs.h"

static const char* TAG = "Transciever";

// void server(){
//     while(1){
  
//     }
// }

void Transmitter(){

    
    NRF24_t device;
    SPI_Config(&device);
    // while(1)
    // {
    char *message = read_message().str;
    ESP_LOGI(TAG,"Message is: %s",message);
    uint8_t *mydata = (uint8_t *)message;
    //ESP_LOGI(TAG, "mydata is: %d", *mydata);
    uint8_t payload = (sizeof(mydata) * 8);
    uint8_t channel = 90;
    Register_Config(&device, channel, payload);
    SetSpeedRates(&device, 2); // 250kbps speed rate

    esp_err_t ret = setTADDR(&device, (uint8_t *)"ARYAN");
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "nrf24l01 not installed");
		while(1) { vTaskDelay(2); }
	}
    else{
        ESP_LOGI(TAG, "Done Till now");
        while (1){
            char *message = read_message().str;
        //ESP_LOGI(TAG,"Message is: %s",message);
    uint8_t *mydata = (uint8_t *)message;

            vTaskDelay(3000 / portTICK_PERIOD_MS);

            ESP_LOGI(TAG, "Sending Data");
            Send_String(&device, mydata, payload);
            //*message = read_message().str;
            ESP_LOGI(TAG, "Data from webserver %s",mydata);
            isSend(&device);

        }
    
    }

}

void Reciever(){
    NRF24_t device;
    SPI_Config(&device);
    uint8_t *reci_data = (uint8_t *)"";
    uint8_t *ptr_reci_data = &reci_data;
    
    uint8_t payload = (sizeof(reci_data) * 8);
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
            //Get_Data(&device, reci_data, payload);
            ESP_LOGI(TAG, "\nRecieved Message: %s", ptr_reci_data);
            //ESP_LOGI(TAG, "\n data: %s", (char *)reci_data);
        }
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }

}

void app_main(void){
    // enable_switches();

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    
    xTaskCreatePinnedToCore(
                    start_tuning_http_server,   /* Function to implement the task */
                    "start_tuning_http_server", /* Name of the task */
                    10000,      /* Stack size in words */
                    NULL,       /* Task input parameter */
                    0,          /* Priority of the task */
                    NULL,       /* Task handle. */
                    1);


        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // if(false){
            ESP_LOGI(TAG, "Switch 1: Transmitter Code");
            xTaskCreatePinnedToCore(
                Transmitter,
                "Transmitter",
                1024*3,
                NULL,
                2,
                NULL,
                0
            );
        // }

        
            // ESP_LOGI(TAG, "Switch 2: Reciever Code");
            // xTaskCreatePinnedToCore(
            //     Reciever,
            //     "Reciever",
            //     1024*3,
            //     NULL,
            //     2,
            //     NULL,
            //     0
            // );
// while(1)
// // {     
// xTaskCreate(Transmitter , "Transmitting ", 1024*3, NULL, 1, NULL);
// vTaskDelay(1);
// xTaskCreate(start_tuning_http_server, "http server ", 1024*3, NULL, 0, NULL);
// vTaskDelay(1);
//}
}