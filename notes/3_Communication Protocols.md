# Communication Protocols:
    Proper description of digital messages. Formats and Rules defines the message.

## There are two types:
* Intra System Protocol:- _microcontroller with other circuit components_
* Inter System Protocol:- _system with microcontroller_
---
## Protocols:
1. UART:
- Universal asynchronous transmitter and receiver.
- asynchrounous means no need of clock wire, instead parity bits are used.
- Two wire: Rx and Tx.
- serially bit by bit data transmission
- Sequential manner 
- Half duplex protocol:  
    Can`t recieve and transmit at same time.
- 8 bit data and 1-1 bit start stop bit
___
2. USART:
- universal synchronous and asynchronous transmitter and receiver.
___
3. USB:
- Universal Serial Bus
- Two wire: D+ and D-
- requires driver software
___
4. I2C:
- Inter integrated Circuit
- Two wires: SDA(Serial Data Line) and SCL(Serial Clock Line)
- Master to Slave, where each slave has unique address
- Master send the address of a slave and it is turn on. Rest slaves are off. 
- Master Send: 8 bit data and Recieves 1 Bit acknowledgment.
- _Two Wires connecting all components with the microcontroller_
- I2C requires Pull-up resistors on both lines

![I2C](https://thestempedia.com/wp-content/uploads/2017/06/I2C-Communication-How-It-Works.png)
___
5. SPI:
- Serial peripheral interface.
- For _ESP32-RADIO-TRANSCIEVER PROJECT_ SPI protocols is used for communication between microcontroller and Transmitter Module.
- 4 Wire: MOSI, MISO, SS, SCLK.
- SCLK: Serial Clock
- MISO: Master In Slave Out
- MOSI: Master Out Slave In
- CS/SS: ChipSelect/SlaveSelect 
- Full Duplex Communication
- serial and synchronous interface
> The Module used is : 
_NRF24L01 2.4GHz PA+LNA SMA Wireless Transceiver Module with Antenna_
![Module](https://www.electronicscomp.com/image/cache/catalog/nrf24l01-pa-lna-module-with-antenna-800x800.jpg)

___
### Working of SPI:
![SPI 1](images/Spi%201.png)
![SPI 2](images/Spi%202.png)
![SPI 3](images/Spi%203.png)

___
