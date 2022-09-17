#include <stdio.h>
#include <stdlib.h>
#include "spi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

struct spi_device_t *handle;

esp_err_t spi_config(){
    
    // Bus Config
    spi_bus_config_t bconfig;
    bconfig.mosi_io_num = MOSI;
    bconfig.sclk_io_num = 18;
    bconfig.miso_io_num = MISO;
    bconfig.max_transfer_sz = 1;
    bconfig.quadwp_io_num = -1;  // -1 not used
    bconfig.quadhd_io_num = -1; 
    spi_bus_initialize(SPI2_HOST, &bconfig, 0);

    // Device Config
    
    spi_device_interface_config_t dconfig;
    dconfig.command_bits = 8;
    dconfig.address_bits = 8;
    dconfig.dummy_bits = 0;
    dconfig.mode = 1;
    dconfig.spics_io_num = -1;
    dconfig.queue_size = 1;
    spi_bus_add_device(SPI2_HOST, &dconfig, &handle);

    return ESP_OK;
}

esp_err_t send(){
    spi_transaction_t tConfig;
    
    // Hello World in Hex
    // 48 65 6C 6C 6F 20 57 6F 72 6C 64
    uint8_t sent_data[11] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};

    tConfig.length = 8;
    tConfig.tx_buffer = sent_data;
    tConfig.rx_buffer = NULL;
    spi_device_transmit(handle, &tConfig);
    return ESP_OK;
}

esp_err_t recieve(){
    spi_transaction_t tConfig;

    tConfig.length = 8;
    tConfig.tx_buffer = NULL;
    tConfig.rx_buffer = NULL;
    spi_device_transmit(handle, &tConfig);
    return ESP_OK;
}



esp_err_t write_reg(uint8_t reg, uint8_t  value){
    // set transaction for write command 
    spi_transaction_t tConfig;
    // command = 0x20 = 0010 0000 = write at CONFIG register.
    // Value = 0111 1110 Tx
    // Value = 0111 1111 Rx
    uint8_t write_data[3] = {W_REGISTER, reg, value}; 
    tConfig.length = 3 * 8;
    tConfig.tx_buffer = write_data;
    tConfig.rx_buffer = NULL;

    // CE or CS need to be high
    // Cs = low ---- that chip is selected for communication 
    // CSN
    // to start SPI Interface CSN need high to low 
    gpio_reset_pin(CS);
	gpio_set_direction(CS, GPIO_MODE_OUTPUT);
	gpio_set_level(CS, 0);

	gpio_reset_pin(CSN);
	gpio_set_direction(CSN, GPIO_MODE_OUTPUT);
	gpio_set_level(CSN, 1);    


    // then send the value for data
    spi_device_transmit(handle, &tConfig);

    // Done transmission 
    // CS back to 1
    gpio_reset_pin(CS);
	gpio_set_direction(CS, GPIO_MODE_OUTPUT);
	gpio_set_level(CS, 1);

	gpio_reset_pin(CSN);
	gpio_set_direction(CSN, GPIO_MODE_OUTPUT);
	gpio_set_level(CSN, 0);

    return ESP_OK;
}
