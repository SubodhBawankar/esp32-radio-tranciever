#include "switch.h"

static const char *TAG_SWITCHES = "switches";
static int enabled_switches_flag = 0;

esp_err_t enable_switches()
{
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = ((1ULL << SWITCH_1) | (1ULL << SWITCH_2));
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    esp_err_t ret = gpio_config(&io_conf);
    
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG_SWITCHES, "Switch Enabled");
        enabled_switches_flag = 1;
    }
    else
    {
        ESP_LOGE(TAG_SWITCHES, "Error");
        enabled_switches_flag = 0;
    }

    return ret;
}

int read_switch(int switch_id){

    int value = gpio_get_level((gpio_num_t)switch_id);
    return !value;
}