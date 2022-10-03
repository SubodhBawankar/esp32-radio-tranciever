#include "driver/spi_master.h"
/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD       0x1C
#define FEATURE     0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define RF_DR_LOW   5
#define PLL_LOCK    4
#define RF_DR_HIGH  3
#define RF_PWR      1
#define LNA_HCURR   0
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

/* Non-P omissions */
#define LNA_HCURR   0

/* P model memory Map */
#define RPD                  0x09
#define W_TX_PAYLOAD_NO_ACK  0xB0

/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2

#define ADD_LEN 5
#define NEW_CONFIG ((1<<EN_CRC) | (0<<CRCO) )
//#define NEW_CONFIG 

#define RF24_PA_MIN = 0
#define RF24_PA_LOW
#define RF24_PA_HIGH
#define RF24_PA_MAX
#define RF24_PA_ERROR
#define RF24_1MBPS = 0
#define RF24_2MBPS
#define RF24_250KBPS
#define RF24_CRC_DISABLED = 0
#define RF24_CRC_8
#define RF24_CRC_16

//Pin Definitions
#define CE 16
#define CSN 17
#define MISO 19
#define MOSI 23
#define SCK 18 

//# define CE

void *memset(void *str, int c, size_t n);
void spi_config();//Congiguration of esp32 spi pins
bool write_data(uint8_t* data_s, uint8_t len);
bool read_data(uint8_t* Data_in, uint8_t* Data_out, size_t DLen);
uint8_t transfer_data(uint8_t address);//spi_transfer
void CSN_Low();//spi_csnLow
void CSN_High();//spi_csnHi
void CE_Low();
void CE_High();
void Config_nRF(uint8_t ch, uint8_t pl);//Nrf24_config
bool Sending();
void Send(uint8_t *value);//Nrf24_send
esp_err_t set_R_Add(uint8_t * adr);//Nrf24_setRADDR
esp_err_t set_T_Add( uint8_t * adr);//Nrf24_setTADDR
uint8_t def_datapipe();//Nrf24_getDataPipe
extern bool Data_Ready();
void get_data(uint8_t * data);//Nrf24_getData
uint8_t get_Status();//Nrf24_getStatus
void Config_reg(uint8_t reg, uint8_t value);//Nrf24_configRegister
void Read_reg(uint8_t reg, uint8_t * value, uint8_t len);//Nrf24_readRegister
void Write_reg(uint8_t reg, uint8_t * value, uint8_t len);//Nrf24_writeRegister
//void Print_Det();
void PWR_RX();//Nrf24_powerUpRx
void PWR_TX();//Nrf24_powerUpTx
void PWR_DWN();//Nrf24_powerDown
void Print_det();//Nrf24_printDetails

//config spi
// config reg
// receive data
