#include <stdio.h>
 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
 
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "esp_err.h"
 
#include "SPI.h"
 
#define TAG1 "NRF24L01"
#define TAG2 "Transmitted"
#define TAG3 "Received"
 
spi_device_t *spi_device_handle;
spi_device_handle_t *p_handle;
 
esp_err_t spi_config()
{
 esp_err_t ret;
 
 spi_bus_config_t bus_congif;
  bus_congif.mosi_io_num = 23;
  bus_congif.miso_io_num = 19;
  bus_congif.sclk_io_num = 18;
  bus_congif.max_transfer_sz = 4092;
  bus_config.quadwp_io_num = -1;
  bus_config.quadhd_io_num = -1;
  ret = spi_bus_initialize(SPI2_HOST, &bus_congif, 0);
  ESP_LOGI(TAG1, "Initialized SPI Bus %d",ret);
 
 assert(ret==ESP_OK);
 spi_device_interface_config_t dev_config;
  memset( &dev_config, 0, sizeof( spi_device_interface_config_t ) );
  devcfg.clock_speed_hz = 4000000;
  dev_config.command_bits = 8;
  dev_config.address_bits = 8;
  dev_config.dummy_bits = 1;
  //dev_config.clock_speed_hz = 2400;
  dev_config.spics_io_num = 15;
  dev_config.flags = SPI_DEVICE_NO_DUMMY;
  dev_config.queue_size = 8;
  dev_config.mode = 0;
  ret=spi_bus_add_device(SPI2_HOST, &dev_config,&p_handle);
  ESP_LOGI(TAG1, "Device added %d",ret);
  assert(ret==ESP_OK);
}
 
esp_err_t send_data(uint8_t* data_s)
{
 spi_transaction_t trans;
 memset( &data_s, 0, sizeof( spi_transaction_t ) );
  trans.flags = SPI_TRANS_USE_TXDATA;
  trans.length = sizeof(data_s);
  trans.tx_buffer = daya_s; 
  trans.rx_buffer = NULL;
  spi_device_transmit(spi_device_handle, &trans);
  ESP_LOGI(TAG1, " ");
  ESP_LOGI(TAG2, "Message:");
}
 
esp_err_t receive_data(uint8_t* Data_in, uint8_t* Data_out, size_t DLen )
{
 spi_transaction_t trans;
  memset( &trans, 0, sizeof( spi_transaction_t ) );
  trans.flags = SPI_TRANS_USE_RXDATA;
  trans.length = DLen*8;
  trans.tx_buffer = Data_out; 
  trans.rx_buffer = Data_in;
  spi_device_transmit(spi_device_handle, &trans)
}
 
uint8_t transfer_data(uint8_t addr);//spi_transfer
{
 uint8_t datain[1];
 uint8_t dataout[1];
 dataout[0] = addr;
 receive_data(datain, dataout, 1);
 return datain[0];
}
 
void CSN_Low();//spi_csnLow
{
 gpio_set_level(CSN, 0);
}
 
void CSN_High();//spi_csnHi
{
 gpio_set_level(CSN, 1);
}
 
void CE_Low();
{
  gpio_set_level(CE,0);
}
 
void CE_High()
{
  gpio_set_level(CE,1);
}
 
void Config_nRF(uint8_t ch, uint8_t pl);//Nrf24_config
{
 channel=ch;
 payload=pl;
 Config_reg(RF_CH , channel);
 Config_reg(RX_PW_P0 , payload);
 Config_reg(RX_PW_P1 , payload);
 PWR_RX();
 transfer_data(FLUSH_RX );
}
 
void Send(uint8_t *value);//Nrf24_send
{
    uint8_t status;
    status = Get_Status();
    while (PTX) // Wait until last paket is send
    {
        status = get_Status();
        if ((status & ((1 << TX_DS)  | (1 << MAX_RT))))
        {
            PTX = 0;
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
 send_data(value,payload);
 CSN_High();
 CE_High();
}
 
extern bool Data_Ready()
{
    uint8_t status = get_Status(dev);
    if ( status & (1 << RX_DR) ) return 1;
    return 0;
}
 
esp_err_t set_R_Add(uint8_t * adrs);//Nrf24_setRADDR
{
 esp_err_t ret = ESP_OK;
 Write_reg(RX_ADDR_P1,adrs,mirf_ADDR_LEN)//Doubt
 uint8_t buffer[5];
 Read_reg(, RX_ADDR_P1, buffer, sizeof(buffer));
    for (int i=0;i<5;i++) 
  {
        ESP_LOGD(TAG3, "address[%d]=0x%x buffer[%d]=0x%x", i, adrs[i], i, buffer[i]);
        if (adrs[i] != buffer[i]) ret = ESP_FAIL;
    }
    return ret;
}
 
esp_err_t set_T_Add(uint8_t * adrs);//Nrf24_setTADDR
{
 esp_err_t ret = ESP_OK;
 Nrf24_writeRegister(dev, RX_ADDR_P0, adr, mirf_ADDR_LEN); //RX_ADDR_P0 must be set to the sending addr for auto ack to work.
 Nrf24_writeRegister(dev, TX_ADDR, adr, mirf_ADDR_LEN);
 uint8_t buffer[5];
 Nrf24_readRegister(dev, RX_ADDR_P0, buffer, sizeof(buffer));
 for (int i=0;i<5;i++) 
  {
        ESP_LOGD(TAG, "adr[%d]=0x%x buffer[%d]=0x%x", i, adr[i], i, buffer[i]);
        if (adr[i] != buffer[i]) ret = ESP_FAIL;
    }
    return ret;
}
 
void get_data(NRF24_t * dev, uint8_t * data);//Nrf24_getData
{
  rf24_datarate_e result;
    uint8_t dr;
    Read_reg(RF_SETUP, &dr, sizeof(dr));
    dr = dr & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
    if (dr == _BV(RF_DR_LOW))
  {
        result = RF24_250KBPS;
    } 
  else if (dr == _BV(RF_DR_HIGH)) {
        // '01' = 2MBPS
        result = RF24_2MBPS;
    } 
  else {
        // '00' = 1MBPS
        result = RF24_1MBPS;
    }
    return result;
}
 
uint8_t get_Status(NRF24_t * dev);//Nrf24_getStatus
{
  uint8_t rv;
    Nrf24_readRegister(dev, STATUS, &rv, 1);
    return rv;
}
 
void Config_reg(uint8_t reg, uint8_t value);//Nrf24_configRegister
{
  CSN_Low();
  transfer_data(W_REGISTER | (REGISTER_MASK & reg));
  transfer_data(value);
  CSN_High();
}
 
void Read_reg(uint8_t reg, uint8_t * value, uint8_t len);//Nrf24_readRegister
{
  CSN_Low();
  transfer_data(R_REGISTER | (REGISTER_MASK &reg));
  Read_reg(value,value,len);
  CSN_High();
}
 
void Write_reg(NRF24_t * dev, uint8_t reg, uint8_t * value, uint8_t len);//Nrf24_writeRegister
{
  CSN_Low();
  transfer_data(W_REGISTER | (REGISTER_MASK & reg));
  send_data(value,len);
  CSN_High();
}
 
void PWR_RX(NRF24_t * dev);//Nrf24_powerUpRx
{
  PTX=0;
  CE_Low();
  Config_reg(CONFIG,mirf_CONFIG | ( (1 << PWR_UP) | (0 << PRIM_RX) ));
}
 
void PWR_TX(NRF24_t * dev);//Nrf24_powerUpTx
{
  PTX = 1;
  Config_reg(CONFIG,mirf_CONFIG);
}
 
void PWR_DWN(NRF24_t * dev);//Nrf24_powerDown
{
  CE_Low();
  Config_reg(CONFIG, mirf_CONFIG);
}
