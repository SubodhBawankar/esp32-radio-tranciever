#include "motor.h"

/*
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


#define INA1 25 // MCPWM0A
#define INA2 26 // MCPWM0B
#define PWMA 27 // MCPWM1A

static const char *TAG = "Motor.h";

esp_err_t config_MotorA(){
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, INA1);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, INA2);

    mcpwm_config_t motorA;
    motorA.frequency = 20000;
    motorA.cmpr_a = 0;
    motorA.cmpr_b = 0;
    motorA.duty_mode = MCPWM_DUTY_MODE_0;
    motorA.counter_mode = MCPWM_UP_COUNTER;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &motorA);
    return ESP_OK;
}

esp_err_t set_MotorA(int direction, float dutycycle){

    if(direction == 0){
        // Forward -------------
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, dutycycle);
        mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
        // ---------------------
    }
    else{
        // Backward -------------
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, dutycycle);
        mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);

        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
        // ---------------------
    }
    return ESP_OK;
}