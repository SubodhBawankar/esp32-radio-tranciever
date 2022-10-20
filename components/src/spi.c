#include <stdio.h>
#include <stdlib.h>
#include "spi.h"
#include <string.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_check.h"

#define ADDR_MASK   0x7f

static const char* TAG = "SPI.C";

struct spi_device_t *handle;

esp_err_t spi_config(){

    esp_err_t ret;
    gpio_reset_pin(CSN);
    Pin_CSN(0);
    Pin_CSN(1);

    //Bus Config    
    spi_bus_config_t buscfg={
        .miso_io_num = MISO,
        .mosi_io_num = MOSI,
        .sclk_io_num = CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // .max_transfer_sz = 32,
    };

    ret =  spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret == ESP_OK){
        ESP_LOGI(TAG, "Bus Config Done RET: %d", ret);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    // Device Config
    spi_device_interface_config_t dconfig;
	memset( &dconfig, 0, sizeof( spi_device_interface_config_t ) );
	dconfig.clock_speed_hz = 40000000;
	dconfig.spics_io_num = -1;
	dconfig.queue_size = 7;
	dconfig.mode = 0;
	dconfig.flags = SPI_DEVICE_NO_DUMMY;
    
    ret = spi_bus_add_device(SPI2_HOST, &dconfig, &handle);
    if(ret == ESP_OK){
        ESP_LOGI(TAG, "Device added to Spi Bus RET: %d", ret);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return ret;
}

esp_err_t spi_write_byte(uint8_t* Dataout, size_t DataLength)
{
	spi_transaction_t SPITransaction;
    esp_err_t ret;
    uint8_t* reg = Dataout;
    size_t t = DataLength;
    if (Dataout == RX_ADDR_P0 || Dataout == TX_ADDR)
    {
        Dataout = W_REGISTER | (REGISTER_MASK & reg);
        DataLength = 5;
    }
    else{
        DataLength = t;
    }
    

	memset(&SPITransaction, 0, sizeof( spi_transaction_t ));
	SPITransaction.length = DataLength * 8;
	SPITransaction.tx_buffer = Dataout;
	SPITransaction.rx_buffer = NULL;
	ret = spi_device_transmit(handle, &SPITransaction);
    if(ret == ESP_OK){
        printf("\n%s Write Command Executed", Dataout);
    }
	return ret;
}

esp_err_t spi_read_byte(uint8_t* Datain, uint8_t* Dataout, size_t DataLength ){

	spi_transaction_t SPITransaction;
    esp_err_t ret;

	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = DataLength * 8;
	SPITransaction.tx_buffer = Dataout;
	SPITransaction.rx_buffer = Datain;
	
    ret = spi_device_transmit( handle, &SPITransaction );
    // ESP_LOGI(TAG, "RET: %d", ret);
    printf("\n%d\n", (int)Datain);

    if(ret == ESP_OK){
        printf("\n%s Read Command Executed\n", Dataout);
    }
	return ret;
}
esp_err_t assign_register(uint8_t reg, uint8_t value){
    
    esp_err_t ret;
    uint8_t datain[1];
	uint8_t dataout[1];
    uint8_t val[1];
    val[0] = value;
    dataout[0] = W_REGISTER | (REGISTER_MASK & reg);
    
    Pin_CSN(0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ret = spi_read_byte(datain, dataout, 1);
    // spi_write_byte(dataout, 1);
    Pin_CSN(1);

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    Pin_CSN(0);
    //spi_read_byte(datain, value, 1);
    ret = spi_write_byte(val, 1);

    Pin_CSN(1);
   	return ret;
}


esp_err_t read_register(uint8_t reg, uint8_t* value){
    uint8_t datain[1];
    uint8_t dataout[1];

    Pin_CSN(0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    dataout[0] = R_REGISTER | (REGISTER_MASK & reg);
    spi_read_byte(datain, dataout, 1);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Pin_CSN(1);

    // printf("\nint %x\n", (int)datain);
    // printf("\n 0x: %02x", datain[0]);
    // printf("Char %x\n", (char)datain);
    for (int i=0;i<5;i++) {
		ESP_LOGI(TAG, "\ndatain=0x%x", (char)datain[i]);
	}
    // data in has the values
    // ESP_LOGI(TAG, "Value is %d", (int)datain);
    return 0;

}

void Pin_CSN(int x){
    // gpio_reset_pin(CSN);
	gpio_set_direction(CSN, GPIO_MODE_OUTPUT);
	gpio_set_level(CSN, x);
}

void config_sender(uint8_t payload_size){
    uint8_t channel = 0x32; // frequency ch =  50
    uint8_t size = payload_size;
    esp_err_t ret;
    // Transmitter Setting
    ret = assign_register(CONFIG, 0x7e); // 0x7E --> 126
    if (ret == ESP_OK)
    {       
        printf("\nCONFIG Done");
    }
    
    ret = assign_register(RF_CH, channel);
    if (ret == ESP_OK)
    {       
        printf("\nRF_CH Done");
    }
    ret = assign_register(EN_RXADDR, 0x02); // data pipe 1 is on
    if (ret == ESP_OK)
    {       
        printf("\nEN_RXADDR Done");
    }
    ret = assign_register(SETUP_AW, 0x03); // address bit width = 5
    if (ret == ESP_OK)
    {       
        printf("\nSETUP_AW Done");
    }
    ret = assign_register(RF_SETUP, 0x0f); // frequency power and data rate is setup
    if (ret == ESP_OK)
    {       
        printf("\nRF_SETUP Done");
    }
    ret = assign_register(SETUP_RETR, 0x03); //max retries = 3
    if (ret == ESP_OK)
    {       
        printf("\nSETUP_RETR Done");
    }
    // assign_register(RX_ADDR_P2, 0xc3);
    
} 


void Set_ADDRESS(uint8_t * address){
    
}


void send(uint8_t senddata, uint8_t payload_size){
    esp_err_t ret;
    
    spi_write_byte((uint8_t *)RX_ADDR_P0, (uint8_t *)"FGHIJ", 5);

    ret = spi_write_byte((uint8_t *)TX_ADDR, (uint8_t *)"FGHIJ", 5);
    if (ret == ESP_OK)
    {       
        printf("\nTX_ADDR Done");
    }
    uint8_t buffer[5];
	read_register(RX_ADDR_P0, buffer, sizeof(buffer));
	for (int i=0;i<5;i++) {
		ESP_LOGD(TAG, "RX_ADDR_P0=0x%x buffer[%d]=0x%x", i, adr[i], i, buffer[i]);
		if (adr[i] != buffer[i]) ret = ESP_FAIL;
	}
    
    
    
    
    
    
    ret = assign_register(W_TX_PAYLOAD, senddata);
    if(ret != ESP_OK){
        printf("\nError: Data not Transmitted");
        printf("\nRET: %d", ret);
    }
    Pin_CSN(1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Pin_CSN(0);
    printf("\nData Send");
}