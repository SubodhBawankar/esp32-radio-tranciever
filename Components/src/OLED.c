#include "OLED.h"

esp_err_t config_led(u8g2_t *p)
{

	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda = OLED_SDA;
	u8g2_esp32_hal.scl = OLED_SCL;
	u8g2_esp32_hal_init(u8g2_esp32_hal);

	u8g2_Setup_ssd1306_i2c_128x32_univision_f(
		p,
		U8G2_R0,
		u8g2_esp32_i2c_byte_cb,
		u8g2_esp32_gpio_and_delay_cb);

	u8x8_SetI2CAddress(&p->u8x8, 0x78);

	u8g2_InitDisplay(p);

	u8g2_SetPowerSave(p, 0);
	
	u8g2_ClearBuffer(p);
	u8g2_SendBuffer(p);
	ESP_LOGI(TAG_OLED, "Configured OLED Display");

	return ESP_OK;
}

esp_err_t displayValue(u8g2_t *p, int value)
{
  u8g2_ClearBuffer(p);
  char volt_str[20];
  u8g2_SetFont(p, u8g2_font_ncenB10_tr);
  sprintf(volt_str,"Value of voltage : %d",value);
  u8g2_DrawStr(p,0,10,volt_str);
  u8g2_SendBuffer(p);
  return ESP_OK;
}
