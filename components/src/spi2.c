#include <stdio.h>
#include <stdlib.h>

#include "spi2.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

static const char* TAG = "SPI2.c";

struct spi_device_handle_t * handle;




void SPI_Config(){
    esp_err_t ret;

	//gpio_pad_select_gpio(CONFIG_CSN_GPIO);
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
	// It does not work with hardware CS control.
	//devcfg.spics_io_num = csn_pin;
	// It does work with software CS control.
	devcfg.spics_io_num = -1;
	devcfg.queue_size = 7;
	devcfg.mode = 0;
	devcfg.flags = SPI_DEVICE_NO_DUMMY;

	ret = spi_bus_add_device( SPI2_HOST, &devcfg, &handle);
	ESP_LOGI(TAG, "spi_bus_add_device=%d",ret);
    ESP_LOGI(TAG, "SPI_CONFIG Done");

    
}

void Pin_CSN(int x){
    gpio_set_direction(CSN, GPIO_MODE_OUTPUT);
    gpio_set_level(CSN, x);
}

void Pin_CE(int x){
    gpio_set_direction(CE, GPIO_MODE_OUTPUT);
    gpio_set_level(CE, x);
}

void Register_Config(uint8_t channel, uint8_t payload){
    configRegister(RF_CH, channel); // Set RF channel

	configRegister(RX_PW_P1, payload);
	configRegister(RX_PW_P0, payload); // Set length of incoming payload
    powerUpRx();
    spi_transfer(FLUSH_RX);
    spi_transfer(FLUSH_TX);
}

void configRegister(uint8_t reg, uint8_t value){
    Pin_CSN(0);
    spi_transfer(W_REGISTER | (REGISTER_MASK & reg));
    spi_transfer(value);
    Pin_CSN(1);
}

uint8_t spi_transfer(uint8_t address) {
	uint8_t datain[1];
	uint8_t dataout[1];
	dataout[0] = address;
	//spi_write_byte(dev, dataout, 1 );
	spi_read_byte(datain, dataout, 1 );
	return datain[0];
}

bool spi_read_byte(uint8_t* Datain, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = Datain;
		spi_device_transmit( handle, &SPITransaction );
	}
	return true;
}

bool spi_write_byte(uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = NULL;
		spi_device_transmit( handle, &SPITransaction );
	}

	return true;
}

void WriteRegister(uint8_t reg, uint8_t * value, uint8_t len)
{
	Pin_CSN(0);
	spi_transfer(W_REGISTER | (REGISTER_MASK & reg));
	spi_write_byte(value, len);
	Pin_CSN(1);

}

void ReadRegister(uint8_t reg, uint8_t * value, uint8_t len)
{
    Pin_CSN(0);
	spi_transfer(R_REGISTER | (REGISTER_MASK & reg));
	spi_read_byte(value, value, len);
	Pin_CSN(1);
}

void powerUpRx() {

    Pin_CE(0);
	configRegister(CONFIG, mirf_CONFIG | ( (1 << PWR_UP) | (1 << PRIM_RX) ) ); //set device as TX mode
	Pin_CE(1);
	//configRegister(STATUS, (1 << TX_DS) | (1 << MAX_RT)); //Clear seeded interrupt and max tx number interrupt
}

void powerUpTx() {
	configRegister(CONFIG, mirf_CONFIG | ( (1 << PWR_UP) | (0 << PRIM_RX) ) );
}

esp_err_t setTADDR(uint8_t * adr)
{
	esp_err_t ret = ESP_OK;
	WriteRegister(RX_ADDR_P0, adr, mirf_ADDR_LEN);
	WriteRegister(TX_ADDR, adr, mirf_ADDR_LEN);
	
	// this to verity whether address is properly set or not
	uint8_t buffer[5];
	ReadRegister(RX_ADDR_P0, buffer, sizeof(buffer));
	//ESP_LOGI(TAG, "Buffer = %x", buffer);
	ESP_LOGI(TAG, "Buffer = %d", *buffer);
    for (int i=0;i<5;i++) {
		ESP_LOGI(TAG, "adr[%d]=0x%xRX_ADDR_P0 buffer[%d]=0x%x", i, adr[i], i, buffer[i]);
		// if (adr[i] != buffer[i]) ret = ESP_FAIL;
	}
	return ret;
}

void Send_data(uint8_t * value, uint8_t payload){

	/*
	uint8_t status;
	int PTX = 1;
	status = GetStatus();
	while (PTX) // Wait until last paket is send
	{
		vTaskDelay(100 / portTICK_PERIOD_MS);
		status = GetStatus();
		if ((status & ((1 << TX_DS)  | (1 << MAX_RT))))
		{
			PTX = 0;
			configRegister(STATUS, (1 << TX_DS) | (1 << MAX_RT));
			break;
		}
	}
	*/

	int PTX = 1; 
	uint8_t status;
	status = GetStatus();
	
	
	while (PTX == 1) // Wait until last paket is send
	{
		vTaskDelay( 1000 / portTICK_PERIOD_MS);
		status = GetStatus();
		printf("\nStatus: %d\n", status);
		
		if((status & 0x20) == 0x00){
			PTX = 0;
			break;
		}
	}

    Pin_CE(0);
    powerUpTx(); // Set to transmitter mode , Power up
    
    Pin_CSN(0); // Pull down chip select
	spi_transfer(FLUSH_TX ); // Write cmd to flush tx fifo
	Pin_CSN(1); // Pull up chip select
	

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Pin_CSN(0); // Pull down chip select
	spi_transfer(W_TX_PAYLOAD); // Write cmd to write payload
	Pin_CSN(1);
	
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Pin_CSN(0);
    spi_write_byte(value, payload); // Write payload
	Pin_CSN(1); // Pull up chip select
    Pin_CE(1); // Start transmission
	
}

uint8_t GetStatus() {
	uint8_t rv;
	ReadRegister(STATUS, &rv, 1);
	return rv;
}

uint8_t GetFIFOStatus() {
	uint8_t rv;
	ReadRegister(FIFO_STATUS, &rv, 1);
	return rv;
}

void Recieve_data(){
	//code
}

esp_err_t setRADDR(uint8_t * adr){
	esp_err_t ret = ESP_OK;

	WriteRegister(RX_ADDR_P1, adr, mirf_ADDR_LEN);

	// this to verity whether address is properly set or not
	uint8_t buffer[5];
	ReadRegister(RX_ADDR_P1, buffer, sizeof(buffer));
	//ESP_LOGI(TAG, "Buffer = %x", buffer);
	ESP_LOGI(TAG, "Buffer = %d", *buffer);
    for (int i=0;i<5;i++) {
		ESP_LOGI(TAG, "adr[%d]=0x%xRX_ADDR_P1 buffer[%d]=0x%x", i, adr[i], i, buffer[i]);
		// if (adr[i] != buffer[i]) ret = ESP_FAIL;
	}
	return ret;
}

bool data_ready(){
	uint8_t fstatus;
	fstatus = GetFIFOStatus();
	printf("\nFifo Status: %d\n", fstatus);

	if((fstatus & 0x03) == 0x02){
		ESP_LOGI(TAG, "Data Ready in RX FIFO");
		return true;
	}
	else{
		ESP_LOGI(TAG, "Waiting for data.");
		return false;
	}
}

void Get_Data(uint8_t payload){
	uint8_t reci_mydata;
	Pin_CSN(0);
	spi_transfer(R_RX_PAYLOAD ); // Send cmd to read rx payload
	spi_read_byte(&reci_mydata, &reci_mydata, payload); // Read payload
	Pin_CSN(1); // Pull up chip select
	configRegister(STATUS, (1 << RX_DR));
	printf("Data: %d\n", reci_mydata);
}