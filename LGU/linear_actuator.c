#include <linear_actuator.h>
#include <pwm.h>
#include <gpin.h>
#include <a2d.h>
#include <pid.h>
#include <pin_assignments.h>
#include <systmr.h>
#include <gio.h>

#include <stdlib.h>
#include <math.h>

#define LINEARMOTOR_DIRECTION (1) // either positive 1 or negative 1

linearmotor_t linearmotors[LIFT_MOTORS_CNT];

static const pinnum_t pin_pwm[LIFT_MOTORS_CNT] = { PIN_LIFT_PWM_FL, PIN_LIFT_PWM_FR, PIN_LIFT_PWM_BL, PIN_LIFT_PWM_BR, };
static const pinnum_t pin_dir[LIFT_MOTORS_CNT] = { PIN_LIFT_DIR_FL, PIN_LIFT_DIR_FR, PIN_LIFT_DIR_BL, PIN_LIFT_DIR_BR, };
static const pinnum_t pin_sw_pos[LIFT_MOTORS_CNT] = { PIN_LIFT_SWP_FL, PIN_LIFT_SWP_FR, PIN_LIFT_SWP_BL, PIN_LIFT_SWP_BR, };
//static const pinnum_t pin_sw_pos[LIFT_MOTORS_CNT] = { PIN_NONE, PIN_NONE, PIN_NONE, PIN_NONE, };
static const pinnum_t pin_sw_neg[LIFT_MOTORS_CNT] = { PIN_LIFT_SWN_FL, PIN_LIFT_SWN_FR, PIN_LIFT_SWN_BL, PIN_LIFT_SWN_BR, };
//static const pinnum_t pin_sw_neg[LIFT_MOTORS_CNT] = { PIN_NONE, PIN_NONE, PIN_NONE, PIN_NONE, };
static const pinnum_t pin_adc[LIFT_MOTORS_CNT] = { PIN_LIFT_ADC_FL, PIN_LIFT_ADC_FR, PIN_LIFT_ADC_BL, PIN_LIFT_ADC_BR, };
//static const pinnum_t pin_adc[LIFT_MOTORS_CNT] = { PIN_NONE, PIN_NONE, PIN_NONE, PIN_NONE, };

static char brake = 0;

volatile char linearmotor_gpinInt = 0;

void linearmotor_setup()
{
}

void linearmotor_init()
{
	int i;
	linearmotor_setup();
	for (i = 0; i < 4; i++)
	{
		linearmotor_t* ptr = &linearmotors[i];
		ptr->id = i;
		gpin_initPin(pin_sw_pos[i], 0, 0);
		gpin_enableNotification(pin_sw_pos[i]);
		gpin_initPin(pin_sw_neg[i], 0, 0);
		gpin_enableNotification(pin_sw_neg[i]);
		gpin_initPin(pin_dir[i], 1, 0);
		pwm_set(pin_pwm[i], 0);
		gpin_initPin(PIN_LIFT_BRAKE, 1, 0);
	}
}

int8_t limitswitch_hit(pinnum_t pin)
{
	if (pin == PIN_NONE) {
		return 0;
	}
	return (gpin_read(pin) == 0);
}

void linearmotor_move(linearmotor_t* motor, signed int speed)
{
	motor->last_speed = speed;

	if (speed > 0 && limitswitch_hit(pin_sw_pos[motor->id]) != 0) {
		speed = 0;
	}
	else if (speed < 0 && limitswitch_hit(pin_sw_neg[motor->id]) != 0) {
		speed = 0;
	}
	else if (brake != 0) {
		speed = 0;
		linearmotor_brake(1);
	}

	speed *= LINEARMOTOR_DIRECTION;
	gpin_write(pin_dir[motor->id], speed < 0 ? 0 : 1);
	if (speed < 0) {
		speed *= -1;
	}
	pwm_set(pin_pwm[motor->id], speed);
}

void linearmotor_movei(int motor, signed int speed)
{
	linearmotors[motor].id = motor;
	if (motor >= 0) {
		linearmotor_move(&linearmotors[motor], speed);
	}
	else {
		int i;
		for (i = 0; i < LIFT_MOTORS_CNT; i++) {
			linearmotor_move(&linearmotors[i], speed);
		}
	}
}

char linearmotor_limitCheck(linearmotor_t* motor)
{
	char sw_pos = limitswitch_hit(pin_sw_pos[motor->id]);
	char sw_neg = limitswitch_hit(pin_sw_neg[motor->id]);

	if (sw_pos != 0) {
		motor->flags |= LINEARMOTOR_FLAG_SW_POS;
	}
	else {
		motor->flags &= ~LINEARMOTOR_FLAG_SW_POS;
	}

	if (sw_neg != 0) {
		motor->flags |= LINEARMOTOR_FLAG_SW_NEG;
	}
	else {
		motor->flags &= ~LINEARMOTOR_FLAG_SW_NEG;
	}

	if (motor->last_speed >= 0 && sw_pos != 0) {
		linearmotor_move(motor, 0);
		return 3;
	}
	else if (motor->last_speed <= 0 && sw_neg != 0) {
		linearmotor_move(motor, 0);
		return 1;
	}
	return 0;
}

char linearmotor_limitChecki(int motor)
{
	int i;
	char res = 0;
	if (motor >= 0) {
		linearmotors[motor].id = motor;
		return linearmotor_limitCheck(&linearmotors[motor]);
	}
	else {
		for (i = 0; i < LIFT_MOTORS_CNT; i++) {
			res |= linearmotor_limitChecki(i);
		}
	}
	return res;
}

float linearmotor_getPosition(linearmotor_t* motor)
{
	float adc, res;
	//adc = (float)a2d_getRaw(pin_adc[motor->id] & 0x3F);
	adc = (float)a2d_getFiltered(pin_adc[motor->id] & 0x3F);
	motor->adc = lroundf(adc);
	res = adc;
	res *= motor->nvm->m;
	res /= motor->nvm->d;
	res + motor->nvm->b;
	motor->position = lroundf(res);
	return res;
}

float linearmotor_getPositioni(int i)
{
	linearmotors[i].id = i;
	return linearmotor_getPosition(&linearmotors[i]);
}

void linearmotor_brake(char x)
{
	brake = x;
	gpin_write(PIN_LIFT_BRAKE, x == 0);
}

void linearmotor_gioNotification()
{
	linearmotor_gpinInt = 1;
	linearmotor_limitChecki(-1);
}
