#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
 
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "spi.h"
 
#define TAG1 "NRF24L01"
#define TAG2 "Transmitted"
#define TAG3 "Received"
#define TAG4 "Transmittor : "
#define TAG5 "Receiver : "
#define HOST SPI2_HOST
 
uint8_t channel = 90;
uint8_t payload ;
uint8_t PTX = 1;//Default transmittor mode

struct spi_device_t *spi_device_handle;

void spi_config()
{
 esp_err_t ret;
 
 gpio_reset_pin(CE);
 gpio_set_direction(CE, GPIO_MODE_OUTPUT);
 gpio_set_level(CE, 0);

 gpio_reset_pin(CSN);
 gpio_set_direction(CSN, GPIO_MODE_OUTPUT);
 gpio_set_level(CSN, 1);

 spi_bus_config_t bus_config = {
  .mosi_io_num = MOSI,
  .miso_io_num = MISO,
  .sclk_io_num = SCK,
  //bus_config.max_transfer_sz = 32;
  .quadwp_io_num = -1,
  .quadhd_io_num = -1
 };

  ret = spi_bus_initialize(HOST, &bus_config, SPI_DMA_CH_AUTO);
  
  ESP_LOGI(TAG1, "Initialized SPI Bus %d",ret);

  spi_device_interface_config_t dev_config;
   memset( &dev_config, 0, sizeof( spi_device_interface_config_t ) );
   dev_config.clock_speed_hz = 4000000;
   //dev_config.command_bits = 8;
   //dev_config.address_bits = 8;
   //dev_config.dummy_bits = 1;
   //dev_config.clock_speed_hz = 2400;
   dev_config.spics_io_num = -1;
   dev_config.queue_size = 7;
   dev_config.mode = 0;
   dev_config.flags = SPI_DEVICE_NO_DUMMY;

   ret=spi_bus_add_device(HOST, &dev_config, &spi_device_handle);
   ESP_LOGI(TAG1, "Device added %d",ret);
   ESP_LOGI(TAG1, "SPI Configuration Completed");
}

void Config_nRF(uint8_t ch, uint8_t pl)//Nrf24_config
{
 Config_reg(RF_CH , ch);
 Config_reg(RX_PW_P0 , pl);
 Config_reg(RX_PW_P1 , pl);
 PWR_RX();
 transfer_data(FLUSH_RX );
 ESP_LOGI(TAG1,"nRF24L01 Configuration done");
}

void Config_reg(uint8_t reg, uint8_t value)//Nrf24_configRegister
{
  CSN_Low();
  transfer_data(W_REGISTER | (REGISTER_MASK & reg));
  transfer_data(value);
  CSN_High();
  ESP_LOGI(TAG1,"Config_reg ");
}

esp_err_t set_R_Add(uint8_t * adrs)//Nrf24_setRADDR
{
 esp_err_t ret = ESP_OK;
 Write_reg(RX_ADDR_P1,adrs,ADD_LEN);
 uint8_t buffer[5];
 Read_reg(RX_ADDR_P1, buffer, sizeof(buffer));
    for (int i=0;i<5;i++) 
    {
        ESP_LOGI(TAG3, "address[%d]=0x%x buffer[%d]=0x%x", i, adrs[i], i, buffer[i]);
        if (adrs[i] != buffer[i])
        {
         ret = ESP_FAIL;
        }
        else
        {
         ret = ESP_OK;
         ESP_LOGI(TAG1,"set_R_Add");
        }
    }
    return ret;
}
 
esp_err_t set_T_Add(uint8_t * adrs)//Nrf24_setTADDR
{
 //esp_err_t ret = ESP_OK;
 esp_err_t ret = ESP_OK;
 Write_reg(RX_ADDR_P0, adrs, ADD_LEN); //RX_ADDR_P0 must be set to the sending addr for auto ack to work.
 Write_reg(TX_ADDR, adrs, ADD_LEN);
 uint8_t buffer[5];
 Read_reg(RX_ADDR_P0, &buffer, sizeof(buffer));

 ESP_LOGI(TAG1,"Address: %d",*buffer);

 for (int i=0;i<5;i++) 
  {
   ESP_LOGI(TAG4, "adrs[%d]=0x%x buffer[%d]=0x%x", i, adrs[i], i, buffer[i]);
   if (adrs[i] != buffer[i]) 
   {
   ret = ESP_FAIL;
   }
   else
   {
    ret = ESP_OK;
    ESP_LOGI(TAG1,"set_T_Add");
   }
  }
    return ret;
}

void Read_reg(uint8_t reg, uint8_t* value, uint8_t len)//Nrf24_readRegister
{
  CSN_Low();
  transfer_data(R_REGISTER | (REGISTER_MASK &reg));
  read_data(&reg,&value,len);
  CSN_High();
  ESP_LOGI(TAG1,"Read_reg");
}
 
void Write_reg(uint8_t reg, uint8_t * value, uint8_t len)//Nrf24_writeRegister
{
  CSN_Low();
  transfer_data(W_REGISTER | (REGISTER_MASK & reg));
  write_data(&value,len);
  CSN_High();
  ESP_LOGI(TAG1,"write reg");
}

extern bool Data_Ready()
{
    uint8_t status = get_Status();
    if ( status & (1 << RX_DR) )
    {
      ESP_LOGI(TAG1,"Data_Ready");
      return 1;
    }
    else
    {
    return 0;
    }
}
 
uint8_t get_Status()//Nrf24_getStatus
{
  uint8_t rv;
    Read_reg(STATUS, &rv, 1);
    ESP_LOGI(TAG1,"get_Status");
    return rv;
}

void get_data(uint8_t * data)//Nrf24_getData
{
  CSN_Low();// Pull down chip select
	transfer_data(R_RX_PAYLOAD); // Send cmd to read rx payload
	read_data(data, data, payload); // Read payload
	CSN_High(); 
	Config_reg(STATUS, (1 << RX_DR)); // Reset status register
  printf("Data to be sent : %s",data);
  ESP_LOGI(TAG1,"get_data");
  vTaskDelay(1000/ portTICK_PERIOD_MS);
}

bool write_data(uint8_t* data_s, uint8_t len)
{
 spi_transaction_t trans;
 if ( len > 0 )
 {
 memset( &data_s, 0, len);
  //trans.flags = SPI_TRANS_USE_TXDATA;
  trans.length = len*8;
  trans.tx_buffer = data_s; 
  trans.rx_buffer = NULL; 
  spi_device_transmit( spi_device_handle, &trans);
  //ESP_LOGI(TAG1, "Sending data \n");
  //vTaskDelay(1000/ portTICK_PERIOD_MS);
  //ESP_LOGI(TAG2, "Message:");
  ESP_LOGI(TAG1,"write_data");
 }
  return true;
}

bool read_data(uint8_t* Data_in, uint8_t* Data_out, size_t DLen )
{
  //esp_err_t ret;
  spi_transaction_t trans;
  if(DLen>0)
  {
  memset( &trans, 0, sizeof( spi_transaction_t ) );
  //trans.flags = SPI_TRANS_USE_RXDATA;
  trans.length = DLen*8;
  trans.tx_buffer = Data_out; 
  trans.rx_buffer = Data_in;
  spi_device_transmit(spi_device_handle, &trans);
  //ESP_LOGI(TAG1, "Receiving data \n");
  vTaskDelay(1000/ portTICK_PERIOD_MS);
  ESP_LOGI(TAG1,"read_data");

  }
  return true;
}
 
uint8_t transfer_data(uint8_t addr)//spi_transfer
{
 uint8_t datain[1];
 uint8_t dataout[1];
 dataout[0] = addr;
 read_data(datain, dataout, 1);
 ESP_LOGI(TAG1,"transfer_data");
 return datain[0];
}
 
bool Sending()
{
  uint8_t status;
	if (PTX==1) 
  {
		while(1) 
    {
			status = get_Status();
			if (status & (1 << TX_DS)) 
      { // Data Sent TX FIFO interrup
				PWR_RX();
				return true;
			}

			if (status & (1 << MAX_RT)) 
      { // Maximum number of TX retries interrupt
				PWR_RX();
				return false;
			}
			vTaskDelay(1);
		}
	}
	return false;
}

void Send(uint8_t *value)//Nrf24_send
{
    uint8_t status;
    status = get_Status();
    while (PTX == 1) // Wait until last paket is send
    {
        status = get_Status();
        //printf("Status: %d\n",status);
        if ((status & ((1 << TX_DS)  | (1 << MAX_RT))))
        {
            PTX = 0;//Changing mode to receiver
            break;
        }
    }
 CE_Low();
 PWR_TX();
 CSN_Low();
 transfer_data(FLUSH_TX);
 CSN_High();

 CSN_Low();
 transfer_data(W_TX_PAYLOAD);
 write_data(value,payload);
 CSN_High();
 CE_High();
 //vTaskDelay(1000/ portTICK_PERIOD_MS);
 ESP_LOGI(TAG1,"Send");
}
 
void PWR_RX()//Nrf24_powerUpRx
{
  PTX = 0;
  CE_Low();
  Config_reg(CONFIG, NEW_CONFIG | ( (1 << PWR_UP) | (0 << PRIM_RX) ));
  CE_High();
	Config_reg(STATUS, (1 << TX_DS) | (1 << MAX_RT)); //Clear seeded interrupt and max tx number interrupt
  ESP_LOGI(TAG1,"PWR_RX");
}

void PWR_TX()//Nrf24_powerUpTx
{
  PTX = 1;
  Config_reg(CONFIG, NEW_CONFIG | ( (1 << PWR_UP) | (0 << PRIM_RX) ) );
  ESP_LOGI(TAG1,"PWR_TX");
  }

void PWR_DWN()//Nrf24_powerDown
{
  CE_Low();
  Config_reg(CONFIG , NEW_CONFIG);
  ESP_LOGI(TAG1,"PWR_DWN");
}
void CSN_Low()//spi_csnLow
{
 gpio_set_level(CSN, 0);
 ESP_LOGI(TAG1,"CSN_low");
}
 
void CSN_High()//spi_csnHi
{
 gpio_set_level(CSN, 1);
 ESP_LOGI(TAG1,"CSN_high");
}
 
void CE_Low()
{
  gpio_set_level(CE,0);
  ESP_LOGI(TAG1,"CE_low");
}
 
void CE_High()
{
  gpio_set_level(CE,1);
  ESP_LOGI(TAG1,"CE_high");
}