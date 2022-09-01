# LED Blink CODE  <br>
Basic header file for c functions <br>
Header files to interact with ESP microprocessor <br>
Custom header file for different functions **sra_board.h** <br>
In main function <br>
Function to check whether gpio pins are active or not **[ESP_ERROR_CHECK(enable_bar_graph())]** <br>
Run an infinte loop <br>
Run a `for` loop to blink LEDs 5 times i.e setting their values to 1 then a delay of a second and setting the value to 0 followed by a delay again. Repeating this 5 times
Assign value 00000001 to an undeclared integer with 8 bits using the command - <br>
``` c
 uint8_t var = 0x01;
```
Again run an infinte loop for blinking LEDs one by one <br>
Apply `if` condition to check whether all all LEDs are off; in such a case change the variable value to the initial one using <br>
``` c
 var = 0x01;
```
Check the gpio pins and run **ESP_ERROR_CHECK(set_bar_graph(var))** function considering the variable as parameter <br>
Using `<<` operator left shift the variable by 1 i.e instead of the 8th now the 7th LED will be turned on <br>
Add a delay of a second using **vTaskDelay(1000 / portTICK_PERIOD_MS)** function <br>
