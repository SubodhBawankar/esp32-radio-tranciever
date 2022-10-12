#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


int Char_to_Ascii(char value[]){
    int size = (sizeof(value) / 3);
    uint8_t converted[size];
    for(int i = 0; i < 10; i++){
        converted[i] = (int )value[i];
    }
    return converted;
}

void app_main(void)
{
    char data[10] = "Aryan";
    uint8_t senddata[10];
    printf("Char to ASCII conversion:\n")
    senddata = Char_to_Ascii(data);

    for(int i = 0; i < 10; i++){
        printf("%d ", senddata[i]);
    }
    
    /*
    printf("ASCII to char conversion:\n")
    for (int i = 0; i < 10; i++) {
        printf("%c ", (char )senddata[i]);
    }
    */


}
