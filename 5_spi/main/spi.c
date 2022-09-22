#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "spi.h"

static const char* TAG = "APP_Main";
// static char* senddata = "HelloWorld";
uint16_t senddata = 0x48;
// H 48, E 65, L 6c, L 6c, O 6f; 


void transmitter(){
    spi_config();
    uint8_t payload_size = sizeof(senddata);
    config_sender(payload_size);
    while(1){
        send((uint8_t)senddata, payload_size);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void){
    xTaskCreate(transmitter, "Transmitter", 1024*3, NULL, 1, NULL);
}
