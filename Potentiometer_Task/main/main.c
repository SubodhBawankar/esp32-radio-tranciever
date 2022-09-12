#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/adc.h"
static const char* TAG = "Potentiometer";
uint32_t value;

void Get_Value()
{
while(1)
 {
    value = adc1_get_raw(ADC1_CHANNEL_7);
    ESP_LOGI(TAG, "Voltage: %d", value);
    vTaskDelay(100 / portTICK_PERIOD_MS);
 }
}
void app_main()
{
 adc1_config_width(ADC_WIDTH_BIT_12);
 adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
 xTaskCreate(Get_Value,"Getting value from potentiometer",4096,NULL,1,NULL);
}