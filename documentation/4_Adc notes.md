# ADC Notes

### Analog to Digital Converter

- Esp32 has ADC on it which can be used to convert the analog signals which are continous, in digital form of 0s and 1s.

- The ADC driver API supports ADC1 (8 channels, attached to GPIOs 32 - 39).
This means 32 - 39 are the pins that can be used for ADC conversion.

- Also pins 0, 2, 4, 12 - 15 and 25 - 27 can be used but for ADC2, a 10 channel type.

## Confrigration:
- adc1_config_width()
- adc1_config_channel_atten()
- adc2_config_channel_atten()

atten stands for _attenuation_, which means lose in any quantity.


## Reading:
- int value = adc1_get_raw()
- int value = adc2_get_raw()

Stores the adc conversion results in value.

## Examples:

``` c
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);
    
    int val = adc1_get_raw(ADC1_CHANNEL_0);
```

``` c
    int read_raw;
    adc2_config_channel_atten( ADC2_CHANNEL_7, ADC_ATTEN_0db );

    esp_err_t r = adc2_get_raw( ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &read_raw);
    if(r == ESP_OK) 
    {
        printf("%d\n", read_raw );
    }
```

> Note: _All those functions are given in_ **#include <driver/adc.h>**