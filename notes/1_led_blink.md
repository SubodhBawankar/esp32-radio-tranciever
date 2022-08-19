# Notes
### led blinks using ESP32

* _#include "freertos/FreeRTOS.h"_
_void app_main()_
We need FreeRtos header file for distributing memory to function or we can say for memory management.
App main is like main func which gets executed on start of program.

* _ESP_ERROR_CHECK(enable_bar_graph())._
This activates all the gpins and checks for errors if any.

* _ESP_ERROR_CHECK(set_bar_graph(0xFF));_
Here we are setting the LED on or off according to the 8 bits  hexadecimal number.

* _vTaskDelay(1000 / portTICK_RATE_MS);_
delay of 1 Sec.

>> Those functions are defined inside SRA header file
#include "sra_board.h"

Date: 19-08
