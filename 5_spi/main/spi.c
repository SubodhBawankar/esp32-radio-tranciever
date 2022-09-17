#include <stdio.h>
#include <stdlib.h>

#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "spi.h"


void app_main(void){

    spi_config();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

}
