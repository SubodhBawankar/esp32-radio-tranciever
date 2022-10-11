#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#include "esp_log.h"

#include "spi.h"

uint8_t value;
uint8_t new_value;
#define TAG1 "Transmittor:"
#define TAG2 "Receiver:"
#define TAG "Potentiometer"

int flag = 0;
uint8_t pre_val;
void* trans;

// void Get_Value()
// {
// while(1)
//  {
//     value = adc1_get_raw(ADC1_CHANNEL_7);
//     ESP_LOGI(TAG, "Voltage: %d", value);
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//  }
// }
// for(flag==0)
// {

 void transmitter(void *pt)
 {
    ESP_LOGI(TAG1,"Start");
    new_value=value;
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
        // value = adc1_get_raw(ADC1_CHANNEL_7);
        // while(1)
        // {
        // pre_val = value;
        // ESP_LOGI(TAG, "Voltage: %d", value);
        // vTaskDelay(100 / portTICK_PERIOD_MS);
        // adc1_config_width(ADC_WIDTH_BIT_12);
        // adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
        // value = adc1_get_raw(ADC1_CHANNEL_7);
        //for(pre_value==value)
        //{
        new_value = value;
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

void potentiometer()
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
 while(1)
 {
    if(flag==0)
    {
    value = adc1_get_raw(ADC1_CHANNEL_7);
    ESP_LOGI(TAG, "Voltage1: %d", value);
    pre_val = value;
    flag = 1;
    }
    else
    {
     value = adc1_get_raw(ADC1_CHANNEL_7);
     ESP_LOGI(TAG, "Voltage2: %d", value);
     if(value != pre_val)
     {
        transmitter(trans);
     }
     flag = 0;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
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
    //xTaskCreate(potentiometer,"Getting value from potentiometer",4096,NULL,1,NULL);
    if(data==1)//Transmitter
    {
     xTaskCreate(potentiometer, "Potentiometer and nrf", 1024*3, NULL, 2, NULL);
    }
    else//receiver
    {
     xTaskCreate(receiver, "RECEIVER", 1024*3, NULL, 2, NULL);
    }
}
