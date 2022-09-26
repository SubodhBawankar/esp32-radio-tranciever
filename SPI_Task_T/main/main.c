#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "spi.h"

uint8_t value[50]="Hello World"; 

void Transmittor()
{
    ESP_LOGI(pcTaskGetName(0), "Start");
	uint8_t pl = sizeof(value);
	uint8_t ch = 90;
	Config_nRF(ch, pl);

	esp_err_t ret = set_T_Add((uint8_t *)"FGHIJ");
	if (ret != ESP_OK) 
    {
		ESP_LOGE(pcTaskGetName(0), "ERROR:nrf24l01 not installed");
		while(1)
        { 
         vTaskDelay(1); 
        }
	}
    
    while(1)
    {
		Send(value);
		vTaskDelay(1);

		ESP_LOGI(pcTaskGetName(0), "Wait for sending.....");

		/*if (Sending())
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
        {
			ESP_LOGI(pcTaskGetName(0),"Send success:%lu", mydata.now_time);
		} 
        else
        {
			ESP_LOGW(pcTaskGetName(0),"Send fail:");
		}*/
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
 
void app_main()
{
    xTaskCreate(Transmittor, "TRANSMITTER", 1024*3, NULL, 2, NULL);
}