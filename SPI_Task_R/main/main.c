#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "esp_err.h"

#include "spi.h"

# define static const char* TAG = "Communication status";

void Receiver()
{
    ESP_LOGI(pcTaskGetName(0), "Start");
    uint8_t payload = size(mydata.value);
    uint8_t channel = 90;
    Config_nRF(channel,payload);

    esp_err_t ret = set_R_Add((uint8_t *)"FGHIJ");
    if(ret != ESP_OK)
    {
        ESP_LOGE(pcTaskGetName(0), "ERROR:nrf24l01 not installed");
        while(1)
        {
            vTaskDelay(1);
        }
        while(1)
        {
          if(Data_Ready())
          {
            get_data(mydata.value);
           // ESP_LOGI(pcTaskGetName(0), "Received Data:%lu", mydata.now_time);
          }
          vTaskDelay(1);
        }
    }
}

void app_main(void)
{
    xTaskCreate(Receiver,"Receiver",1024*3,NULL,2,NULL);
}
