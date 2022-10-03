#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "spi.h"

static const char* TAG = "Communication status";
uint8_t value[10];
uint8_t pl = sizeof(value);
uint8_t ch = 50;
//unsigned long now_time;

void Receiver()
{
    ESP_LOGI(pcTaskGetName(0), "Start");
    spi_config();
    ESP_LOGI(TAG,"channel:%d \nSize of value: %d",ch,pl);
    //printf("channel:%s \n size of value: %s",ch,pl);
    Config_nRF(ch,pl);

    esp_err_t ret = set_R_Add((uint8_t *)"SRAVJ");
    if(ret != ESP_OK)
    {
        ESP_LOGE(pcTaskGetName(0), "ERROR:nrf24l01 not installed");
        while(1)
        {
            vTaskDelay(1);
        }
    }
    else
    {
        ESP_LOGI(TAG , "Started Recieving");
        while(1)
        {
          if(Data_Ready()==1)
          {
            get_data(value);
            ESP_LOGI(pcTaskGetName(0), "Received Data:%s", value);
           // ESP_LOGI(pcTaskGetName(0), "Received Data:%lu", mydata.now_time);
          }
          else
          {
            ESP_LOGI(TAG, "Data not received");
          }
          vTaskDelay(1000/ portTICK_PERIOD_MS);
        }
    }
}

void app_main(void)
{
    xTaskCreate(Receiver, "Receiver", 1024*3, NULL, 2, NULL);
}