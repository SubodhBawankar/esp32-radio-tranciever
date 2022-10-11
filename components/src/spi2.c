#include <stdio.h>
#include <stdlib.h>

#include "spi2.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

static const char* TAG = "SPI2.c";

int PTX;


void SPI_Config(NRF24_t * dev){
    esp_err_t ret;

	gpio_reset_pin(CSN);
    Pin_CSN(1);

	spi_bus_config_t spi_bus_config = {
		.sclk_io_num = SCLK,
		.mosi_io_num = MOSI,
		.miso_io_num = MISO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	ret = spi_bus_initialize( SPI2_HOST, &spi_bus_config, SPI_DMA_CH_AUTO );
	ESP_LOGI(TAG, "spi_bus_initialize=%d",ret);

	spi_device_interface_config_t devcfg;
	memset( &devcfg, 0, sizeof( spi_device_interface_config_t));
	devcfg.clock_speed_hz = 4000000 ;
	devcfg.spics_io_num = -1;
	devcfg.queue_size = 7;
	devcfg.mode = 0;
	devcfg.flags = SPI_DEVICE_NO_DUMMY;

	spi_device_handle_t handle;
	ret = spi_bus_add_device( SPI2_HOST, &devcfg, &handle);
	ESP_LOGI(TAG, "spi_bus_add_device=%d",ret);
    ESP_LOGI(TAG, "SPI_CONFIG Done");

	dev->_SPIHandle = handle;
    
}

void Pin_CSN(int x){
    gpio_set_direction(CSN, GPIO_MODE_OUTPUT);
    gpio_set_level(CSN, x);
}

void Pin_CE(int x){
    gpio_set_direction(CE, GPIO_MODE_OUTPUT);
    gpio_set_level(CE, x);
}

void Register_Config(NRF24_t * dev ,uint8_t channel, uint8_t payload){
    configRegister(dev, RF_CH, channel); // Set RF channel

	configRegister(dev, RX_PW_P0, payload);
	configRegister(dev, RX_PW_P1, payload); // Set length of incoming payload
    powerUpRx(dev);
    spi_transfer(dev, FLUSH_RX);
}

void configRegister(NRF24_t * dev, uint8_t reg, uint8_t value){
    Pin_CSN(0);
    spi_transfer(dev, W_REGISTER | (REGISTER_MASK & reg));
    spi_transfer(dev, value);
    Pin_CSN(1);
}

uint8_t spi_transfer(NRF24_t * dev, uint8_t address) {
	uint8_t datain[1];
	uint8_t dataout[1];
	dataout[0] = address;
	spi_read_byte(dev, datain, dataout, 1 );
	return datain[0];
}

bool spi_read_byte(NRF24_t * dev, uint8_t* Datain, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = Datain;
		spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	}
	return true;
}

bool spi_write_byte(NRF24_t * dev, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = NULL;
		spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	}

	return true;
}

void WriteRegister(NRF24_t * dev, uint8_t reg, uint8_t * value, uint8_t len)
{
	Pin_CSN(0);
	spi_transfer(dev, W_REGISTER | (REGISTER_MASK & reg));
	spi_write_byte(dev, value, len);
	Pin_CSN(1);

}

void ReadRegister(NRF24_t * dev, uint8_t reg, uint8_t * value, uint8_t len)
{
    Pin_CSN(0);
	spi_transfer(dev, R_REGISTER | (REGISTER_MASK & reg));
	spi_read_byte(dev, value, value, len);
	Pin_CSN(1);
}

void powerUpRx(NRF24_t * dev) {
	PTX = 0;
    Pin_CE(0);
	configRegister(dev, CONFIG, mirf_CONFIG | ( (1 << PWR_UP) | (1 << PRIM_RX) ) ); //set device as TX mode
	Pin_CE(1);
	configRegister(dev, STATUS, (1 << TX_DS) | (1 << MAX_RT)); //Clear seeded interrupt and max tx number interrupt
}

void powerUpTx(NRF24_t * dev) {
	PTX = 1;
	configRegister(dev, CONFIG, mirf_CONFIG | ( (1 << PWR_UP) | (0 << PRIM_RX) ) );
}

esp_err_t setTADDR(NRF24_t * dev, uint8_t * adr)
{
	esp_err_t ret = ESP_OK;
	WriteRegister(dev, RX_ADDR_P0, adr, mirf_ADDR_LEN);
	WriteRegister(dev, TX_ADDR, adr, mirf_ADDR_LEN);
	
	// this to verity whether address is properly set or not
	uint8_t buffer[5];
	ReadRegister(dev, RX_ADDR_P0, buffer, sizeof(buffer));
	ESP_LOGI(TAG, "Buffer = 0x%s", buffer);
    for (int i=0;i<5;i++) {
		ESP_LOGI(TAG, "adr[%d]=0x%x RX_ADDR_P0 buffer[%d]=0x%x", i, adr[i], i, buffer[i]);
	}
	return ret;
}


bool spi_send_byte(NRF24_t * dev, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = NULL;
		spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	}

	return true;
}


void Send_data(NRF24_t * dev, uint8_t * value, uint8_t payload){

	
	uint8_t status;
	status = GetStatus(dev);
	while (PTX) // Wait until last paket is send
	{
		vTaskDelay(100 / portTICK_PERIOD_MS);
		status = GetStatus(dev);
		printf("\nStatus: %d\n", status);
		if ((status & ((1 << TX_DS))))
		{
			PTX = 0;
			break;
		}
	}

    Pin_CE(0);
    powerUpTx(dev); // Set to transmitter mode , Power up
    
    Pin_CSN(0); // Pull down chip select
	spi_transfer(dev, FLUSH_TX ); // Write cmd to flush tx fifo
	Pin_CSN(1); // Pull up chip select
	

    Pin_CSN(0); // Pull down chip select
	spi_transfer(dev, W_TX_PAYLOAD); // Write cmd to write payload
	

    spi_send_byte(dev, value, payload); // Write payload
	Pin_CSN(1); // Pull up chip select
    Pin_CE(1); // Start transmission
	
}


bool isSend(NRF24_t * dev){
	uint8_t status;
	if (PTX) {
		while(1) {
			status = GetStatus(dev);

			if (status & (1 << TX_DS)) { // Data Sent TX FIFO interrup
				powerUpRx(dev);
				return true;
			}

			if (status & (1 << MAX_RT)) { // Maximum number of TX retries interrupt
				powerUpRx(dev);
				return false;
			}
			vTaskDelay(1);
		}
	}
	return false;
}

uint8_t GetStatus(NRF24_t * dev) {
	uint8_t rv;
	ReadRegister(dev, STATUS, &rv, 1);
	return rv;
}

uint8_t GetFIFOStatus(NRF24_t * dev) {
	uint8_t rv;
	ReadRegister(dev, FIFO_STATUS, &rv, 1);
	return rv;
}

esp_err_t setRADDR(NRF24_t * dev, uint8_t * adr){
	esp_err_t ret = ESP_OK;

	WriteRegister(dev, RX_ADDR_P1, adr, mirf_ADDR_LEN);

	// this to verity whether address is properly set or not
	uint8_t buffer[5];
	ReadRegister(dev, RX_ADDR_P1, buffer, sizeof(buffer));
	//ESP_LOGI(TAG, "Buffer = %x", buffer);
	ESP_LOGI(TAG, "Buffer = %d", *buffer);
    for (int i=0;i<5;i++) {
		ESP_LOGI(TAG, "adr[%d]=0x%x RX_ADDR_P1 buffer[%d]=0x%x", i, adr[i], i, buffer[i]);
	}
	return ret;
}

bool data_ready(NRF24_t * dev){
	uint8_t fstatus;
	fstatus = GetFIFOStatus(dev);
	
	if((fstatus & 0x01)){
		ESP_LOGI(TAG, "Waiting for data.");
		return false;
	}
	else{
		printf("\nFifo Status: %d\n", fstatus);
		ESP_LOGI(TAG, "Data Ready in RX FIFO");
		return true;
	}
}

void Get_Data(NRF24_t * dev, uint8_t * reci_data, uint8_t payload){
	Pin_CSN(0);
	spi_transfer(dev, R_RX_PAYLOAD); // Send cmd to read rx payload
	spi_read_byte(dev, reci_data, reci_data, payload); // Read payload
	Pin_CSN(1); // Pull up chip select
	configRegister(dev, STATUS, (1 << RX_DR));
}

bool spi_recieve_byte(NRF24_t * dev, uint8_t* Datain, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = Datain;
		spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	}
	return true;
}

void SetSpeedRates(NRF24_t * dev, uint8_t val)
{
	if (val > 2) return;

	uint8_t value;
	ReadRegister(dev, RF_SETUP, &value, 1);
	if(val == 2)
	{
		value = value | 0x20;
		value = value & 0xF7;
		configRegister(dev, RF_SETUP, value);
	}
	else
	{
		value = value & 0xD7;
		value = value | (val << RF_DR_HIGH);
		configRegister(dev, RF_SETUP,	value);
	}
} 
