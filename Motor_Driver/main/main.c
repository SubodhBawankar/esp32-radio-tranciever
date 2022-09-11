#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/adc.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

static const char* TAG = "Potentiometer";
static const char* TAG2 = "Motor Control";
uint32_t value;

void config_motor()
{
 mcpwm_pin_config_t var;
 var.mcpwm0a_out_num = 32;
 var.mcpwm0b_out_num = 33;

 mcpwm_config_t mvar;
 mvar.frequency = 20000;
 mvar.cmpr_a = 0.0;
 mvar.cmpr_b = 0.0;
 mcpwm_duty_type_t MCPWM_DUTY_MODE_0;
 mcpwm_counter_type_t MCPWM_UP_COUNTER;
 mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &mvar);
}

void Get_Value()
{
while(1)
 {
    value = adc1_get_raw(ADC1_CHANNEL_7);
    ESP_LOGI(TAG, "Voltage: %d", value);
    value = (value*225)/4095;
    ESP_LOGI(TAG2, "Dutycycle for motor: %d", value);
    vTaskDelay(100 / portTICK_PERIOD_MS);
 }
}

void motor_driver()
{
    while(1)
    {
    mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_A, value);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0,MCPWM_OPR_A, MCPWM_UNIT_0);
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
    ESP_LOGI(TAG2, "Dutycycle for motor: %d", value);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
 adc1_config_width(ADC_WIDTH_BIT_12);
 adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

 config_motor();

 xTaskCreate(Get_Value,"Getting value from potentiometer",4096,NULL,1,NULL);
 xTaskCreate(motor_driver,"Turning the motor on",4096,NULL,2,NULL);
}
