#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "esp_err.h"

#include "spi.h"

# define static const char* TAG = "Communication status";
uint8_t value[4];

void Receiver()
{
    ESP_LOGI(pcTaskGetName(0), "Start");
    spi_config();
    uint8_t pl = sizeof(value);
    uint8_t ch = 90;
    Config_nRF(ch,pl);

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
            get_data(value);
           // ESP_LOGI(pcTaskGetName(0), "Received Data:%lu", mydata.now_time);
          }
          vTaskDelay(1);
        }
    }
}

void app_main(void)
{
    xTaskCreate(Receiver, "Receiver", 1024*3, NULL, 2, NULL);
}
