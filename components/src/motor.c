#include "motor.h"

/*
mcpwm_gpio_init()
structure config 
mcpwm_init()


Pin Outs:
TB6612FNG
- Vm -- 12v
- Vcc -- 5v
- GND --- esp GND
- Aout1 - motor wires +ve
- Aout2 - motor wires -ve
- INA1 - high or low for forward/backward 
- INA2 - high or low for forward/backward
- PWMA - pwm signal from esp32 gpio pin  
*/


#define INA1 4
#define INA2 5
#define PWMA 13
static const char *TAG = "Motor.h";

esp_err_t config_MotorA(){

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PWMA,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
    
    // Forward Condition
    gpio_set_level(INA1, 1);
    gpio_set_level(INA2, 0);
    return ESP_OK;
}

esp_err_t set_MotorA(int direction, uint32_t dutycycle){

    // direction: 
    // 0 --> Forward 1 --> Backward
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, dutycycle);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    vTaskDelay(3000 / portTICK_PERIOD_MS);
    return ESP_OK;
}


/*
esp_err_t err;

    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, MDA_NORMAL_IN_1);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, PWMA);
    
    esp_err_t mcpwm_gpio_init(mcpwm_unit_tmcpwm_num, mcpwm_io_signals_tio_signal, int gpio_num)
    
    mcpwm_num: set MCPWM Channel(0-1)
    io_signal: set MCPWM signals, each MCPWM unit has 6 output(MCPWMXA, MCPWMXB) and 9 input(SYNC_X, FAULT_X, CAP_X) ‘X’ is timer_num(0-2)
    gpio_num: set this to configure gpio for MCPWM, if you want to use gpio16, gpio_num = 16

    


    mcpwm_config_t pwm_config;
    // sets the pwm frequency = 20000
    pwm_config.frequency = 20000;
    // sets the initial duty cycle of PWMxA = 0
    pwm_config.cmpr_a = 0;
    // sets the initial duty cycle of PWMxB = 0
    pwm_config.cmpr_b = 0;
    // sets the pwm counter mode
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    // sets the pwm duty mode
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;


    esp_err_t err1_0 = mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);
    esp_err_t err1_1 = mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &pwm_config);
*/