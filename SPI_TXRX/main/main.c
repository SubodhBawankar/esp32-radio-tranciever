#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "spi.h"

uint8_t value;
uint8_t new_value;
#define TAG1 "Transmittor:"
#define TAG2 "Receiver:"

 void transmitter(void *pt)
 {
    ESP_LOGI(TAG1,"Start");
    new_value=0;
    uint8_t* valpt = &new_value;
    spi_config();
    uint8_t payload = sizeof(new_value);
    uint8_t channel = 90;
    Config_nRF(channel,payload);
    esp_err_t ret = set_T_Add((uint8_t*)"SRAVJ");
    if(ret!= ESP_OK)
    {
        ESP_LOGE(pcTaskGetName(0), "nrf24l01 not installed");
        while(1)
        {
            vTaskDelay(1);
        }
    }
    else
    {
        ESP_LOGI(TAG1," Adress Set");
    }
    while(1)
    {
        new_value = 0;
        printf("Data : %d", *valpt);
        Send(valpt);
        vTaskDelay(1);
        ESP_LOGI(TAG1,"Waiting for data to be sent");
        if(Sending())
        {
          ESP_LOGI(pcTaskGetName(0),"Data Sent");
        }
        else
        {
            ESP_LOGW(pcTaskGetName(0),"Data not sent");
        }
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
 }

 void receiver(void *pr)
 {
    new_value = 0;
    uint8_t* valpt = &new_value;
    ESP_LOGI(TAG2, "Start");
    spi_config();
    uint8_t payload = sizeof(new_value);
    uint8_t channel = 90;
    Config_nRF(channel, payload);
    esp_err_t ret = set_R_Add((uint8_t*)("SRAVJ"));
    if(ret != ESP_OK)
    {
     ESP_LOGE(pcTaskGetName(0), "nrf24l01 not installed");
	 while(1) { vTaskDelay(1); }
    }
    else
    {
      ESP_LOGI(TAG2, "Address Set");
    }
    ESP_LOGI(TAG2, "Listening....");
    while(1)
    {
        if(Data_Ready())
        {
            get_data(valpt);
            printf("Received Data : %d\n", *valpt);
            vTaskDelay(1);
        }
        else
        {
         ESP_LOGW(TAG2,"Data not received");
        }
    }
 }

void app_main(void)
{
    int data;
    printf("Select mode:\n");
    printf("Transmittor : 1\n");
    printf("Receiver : 0\n");
    scanf("%d",&data);
    if(data==1)//Transmitter
    {
     xTaskCreate(transmitter, "TRANSMITTER", 1024*3, NULL, 2, NULL);
    }
    else//receiver
    {
     xTaskCreate(receiver, "RECEIVER", 1024*3, NULL, 2, NULL);
    }
}
