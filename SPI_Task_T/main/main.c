#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "spi.h"


void Transmitter()
{
    ESP_LOGI(pcTaskGetName(0), "Start");
	spi_config();
	uint8_t value = 100; 
	uint8_t pl = sizeof(value);
	uint8_t ch = 50;
	Config_nRF(ch, pl);

	esp_err_t ret = set_T_Add((uint8_t *)"SRAVJ");
	if (ret != ESP_OK) 
    {
		ESP_LOGE(pcTaskGetName(0), "ERROR : nrf24l01 not installed");
		while(1)
        { 
         vTaskDelay(1); 
        }
	}
	else
	{
    while(1)
    {
		Send(&value);
		vTaskDelay(1);

		ESP_LOGI(pcTaskGetName(0), "Wait for sending.....");

		if (Sending())
        {
			ESP_LOGI(pcTaskGetName(0),"Sent successfully:");
			ESP_LOGI(pcTaskGetName(0),"Data:%d",value);
		} 
        else
        {
			ESP_LOGW(pcTaskGetName(0),"Data not sent:");
		}
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
	}
}

void app_main()
{
    xTaskCreate(Transmitter, "TRANSMITTER", 1024*3, NULL, 2, NULL);
}