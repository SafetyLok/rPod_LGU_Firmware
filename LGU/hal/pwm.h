#ifndef _PWM_H_
#define _PWM_H_

#include <stdint.h>
#include <hal_stdtypes.h>

enum
{
	PWM_PIN_1A,
	PWM_PIN_1B,
	PWM_PIN_2A,
	PWM_PIN_2B,
	PWM_PIN_3A,
	PWM_PIN_3B,
	PWM_PIN_4A,
	PWM_PIN_4B,
	PWM_PIN_5A,
	PWM_PIN_5B,
	PWM_PIN_6A,
	PWM_PIN_6B,
	PWM_PIN_7A,
	PWM_PIN_7B,
};

void pwm_init(void);
void pwm_set(int which, uint8 duty);
void pwm_setRaw(int which, uint32_t period, uint32_t duty);
void pwm_setDigital(int which, char data);
void pwm_setClkDiv(int which, uint16_t div);

#endif
