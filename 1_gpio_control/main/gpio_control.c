#include <stdio.h>


#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG = "GPIO_CONTROL";


//////// bar graph //////////
#define BG_LED_1 32
#define BG_LED_2 33
#define BG_LED_3 25
#define BG_LED_4 26
#define BG_LED_5 27
#define BG_LED_6 14
#define BG_LED_7 12
#define BG_LED_8 13

void app_main()
{
    int on_off = 1;
    static const int pin_out[8] = {BG_LED_1, BG_LED_2, BG_LED_3, BG_LED_4, BG_LED_5, BG_LED_6, BG_LED_7, BG_LED_8};
    int64_t bit_mask = 0;
    bit_mask = (1ULL << BG_LED_1) | (1ULL << BG_LED_2) | (1ULL << BG_LED_3) | (1ULL << BG_LED_4) | (1ULL << BG_LED_5) | (1ULL << BG_LED_6) | (1ULL << BG_LED_7) | (1ULL << BG_LED_8);

    gpio_config_t config_variable;
    config_variable.pin_bit_mask = bit_mask;
    config_variable.mode = GPIO_MODE_OUTPUT;
    config_variable.pull_up_en = 0;
    config_variable.pull_down_en = 1;
    config_variable.intr_type = GPIO_INTR_DISABLE;
    esp_err_t err = gpio_config(&config_variable);
    if (err == ESP_OK){
        ESP_LOGI(TAG, "Config Done");
        while (1){

            for (int i = 0; i < 8; i++)
            {
                gpio_set_level((gpio_num_t)pin_out[i], on_off);
            }
        
            if(on_off == 1){
                on_off = 0;
            }
            else{
                on_off = 1;
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);        
        }
    }
    else{
        ESP_LOGE(TAG, "Error: ");
    }
}
