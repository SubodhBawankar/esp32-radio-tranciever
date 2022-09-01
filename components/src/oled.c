#include "oled.h"

esp_err_t led_config(u8g2_t *config_u8g2){

    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda = OLED_SDA;
	u8g2_esp32_hal.scl = OLED_SCL;
	u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_Setup_ssd1306_i2c_128x32_univision_f(
        config_u8g2, 
        U8G2_R0, 
        u8g2_esp32_i2c_byte_cb, 
        u8g2_esp32_gpio_and_delay_cb);
	
    //u8x8_SetI2CAddress(&u8g2->u8x8, 0x78);

    u8g2_InitDisplay(config_u8g2);// send init sequence to the display, 
    //display is in sleep mode after this

    u8g2_SetPowerSave(config_u8g2, 0); // wake up
    
    u8g2_ClearBuffer(config_u8g2);
	u8g2_SendBuffer(config_u8g2);
	ESP_LOGI("TAG_OLED", "Configured OLED Display");

    return ESP_OK;
}

esp_err_t printvoltage(u8g2_t *config_u8g2, int reading){    
    char volt_str[20];

    u8g2_SetFont(config_u8g2, u8g2_font_ncenB10_tr);

    sprintf(volt_str, "Voltage : %d", reading);
    u8g2_ClearBuffer(config_u8g2);
    u8g2_DrawStr(config_u8g2, 2, 15, volt_str);
    u8g2_SendBuffer(config_u8g2);
    return ESP_OK;
}
