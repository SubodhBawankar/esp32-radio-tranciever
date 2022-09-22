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
    uint8_t mydata[30] = "HelloWorld";
    ESP_LOGI(TAG, "\nMy data is: %s", mydata);
    uint8_t payload = sizeof(mydata);
    uint8_t channel = 90;
    // Nrf24_config(&dev, channel, payload);
    Register_Config(channel, payload);
    
    esp_err_t ret = setTADDR( (uint8_t *)"FGHIJ" );
	if (ret != ESP_OK) {
		ESP_LOGE(pcTaskGetName(0), "nrf24l01 not installed");
		while(1) { vTaskDelay(1); }
	}
    else{
        while (1)
        {
            ESP_LOGI(TAG, "Done Till now");
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            ESP_LOGI(TAG, "Sending Data");
            while(1) {
                Send_data(&mydata, payload);
                vTaskDelay(1000 / portTICK_PERIOD_MS);

                ESP_LOGI(pcTaskGetName(0), "Wait for sending.....");
                /*
                if (Nrf24_isSend(&dev)) {
                    ESP_LOGI(pcTaskGetName(0),"Send success:%lu", mydata.now_time);
                } else {
                    ESP_LOGW(pcTaskGetName(0),"Send fail:");
                }
                */
                vTaskDelay(1000/portTICK_PERIOD_MS);
	        }

        }
    }

}


void app_main(void){
    xTaskCreate(
        Transmitter,
        "Transmitter",
        1024*3,
        NULL,
        2,
        NULL
    );
}
