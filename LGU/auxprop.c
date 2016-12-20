#include <auxprop.h>
#include <main.h>
#include <pin_assignments.h>
#include <gpin.h>
#include <pwm.h>
#include <term.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

char auxprop_lookup(signed int speed, int* div, int* ticks);
void auxprop_moveSpeed(signed int speed);
void auxprop_moveSpeedSingle(char which, signed int speed);

auxprop_state_t auxprop_state;

// speed lower, speed higher, div, ticks lower, ticks higher
// speed is given in multiples of 10cm/s, where 1000 is the maximum at 10 m/s
const int speed_table[][5] = {
	{ 1, 1, 224, 28912, 28912, },
	{ 2, 2, 112, 28912, 28912, },
	{ 3, 3, 80, 26984, 26984, },
	{ 4, 4, 56, 28912, 28912, },
	{ 5, 6, 40, 32381, 26984, },
	{ 7, 7, 32, 28912, 28912, },
	{ 8, 8, 28, 28912, 28912, },
	{ 9, 9, 24, 29983, 29983, },
	{ 10, 12, 20, 32381, 26984, },
	{ 13, 14, 16, 31136, 28912, },
	{ 15, 16, 14, 30839, 28912, },
	{ 17, 19, 12, 31746, 28405, },
	{ 20, 24, 10, 32381, 26984, },
	{ 25, 32, 8, 32381, 25298, },
	{ 33, 49, 6, 32708, 22028, },
	{ 50, 98, 4, 32381, 16521, },
	{ 99, 197, 2, 32708, 16437, },
	{ 198, 297, 1, 32708, 21806, },
	{ 298, 397, 1, 21732, 16313, },
	{ 398, 497, 1, 16272, 13031, },
	{ 498, 597, 1, 13005, 10848, },
	{ 598, 697, 1, 10830, 9292, },
	{ 698, 797, 1, 9278, 8126, },
	{ 798, 897, 1, 8116, 7220, },
	{ 898, 997, 1, 7212, 6496, },
	{ 998, 1000, 1, 6489, 6476, },
	{ 0, 0, 0, 0, 0, },
};

char auxprop_lookup(signed int speed, int* div, int* ticks)
{
	int* ptr;
	int i;
	if (speed <= 0)
	{
		*div = 256;
		*ticks = 0xFFFF;
		return 1;
	}
	else if (speed >= 1000) {
		speed = 1000;
	}
	for (i = 0; ; i++)
	{
		ptr = (int*)&speed_table[i][0];
		if (ptr[0] == 0) {
			return 0;
		}
		if (ptr[0] <= speed && ptr[1] >= speed)
		{
			*div = ptr[2];
			if (ptr[0] == ptr[1] || ptr[0] == speed)
			{
				*ticks = ptr[3];
			}
			else if (ptr[1] == speed)
			{
				*ticks = ptr[4];
			}
			else
			{
				double h = ptr[1] - ptr[0];
				double x = speed - ptr[0];
				double d = ptr[3] - ptr[4];
				int t;
				d *= x / h;
				t = lround(d);
				*ticks = ptr[3] - t;
			}
			return 1;
		}
	}
	return 0;
}

void auxprop_moveSpeed(signed int speed)
{
	int spd = speed;
	int div, ticks;
	if (spd < 0) {
		spd *= -1;
	}
	if (auxprop_lookup(spd, &div, &ticks) == 0) {
		return;
	}
	pwm_setClkDiv(PIN_DRIVE_STEP_L, div);
	pwm_setClkDiv(PIN_DRIVE_STEP_R, div);
	if (spd > 0) {
		auxprop_state.speed = speed;
		auxprop_enable(1);
		pwm_setRaw(PIN_DRIVE_STEP_L, ticks, ticks / 2);
		pwm_setRaw(PIN_DRIVE_STEP_R, ticks, ticks / 2);
	}
	else {
		auxprop_state.speed = 0;
		auxprop_enable(0);
		pwm_setRaw(PIN_DRIVE_STEP_L, ticks, 0);
		pwm_setRaw(PIN_DRIVE_STEP_R, ticks, 0);
	}
	if (speed >= 0) {
		gpin_write(PIN_DRIVE_DIR_L, 1);
		gpin_write(PIN_DRIVE_DIR_R, 1);
	}
	else {
		gpin_write(PIN_DRIVE_DIR_L, 0);
		gpin_write(PIN_DRIVE_DIR_R, 0);
	}
}

void auxprop_moveSpeedSingle(char which, signed int speed)
{
	int spd = speed;
	int div, ticks;
	int pin_step, pin_dir;
	if (spd < 0) {
		spd *= -1;
	}
	if (auxprop_lookup(spd, &div, &ticks) == 0) {
		return;
	}
	if (which == LGUDRIVE_LEFT) {
		pin_dir = PIN_DRIVE_DIR_L;
		pin_step = PIN_DRIVE_STEP_L;
	}
	else if (which == LGUDRIVE_RIGHT) {
		pin_dir = PIN_DRIVE_DIR_R;
		pin_step = PIN_DRIVE_STEP_R;
	}
	else {
		return; // TODO: error report
	}
	pwm_setClkDiv(pin_step, div);
	if (spd > 0) {
		auxprop_state.speed = speed;
		auxprop_enable(1);
		pwm_setRaw(pin_step, ticks, ticks / 2);
	}
	else {
		auxprop_state.speed = 0;
		auxprop_enable(0);
		pwm_setRaw(pin_step, ticks, 0);
	}
	if (speed >= 0) {
		gpin_write(pin_dir, 1);
	}
	else {
		gpin_write(pin_dir, 0);
	}
}

void auxprop_clutch(char clutch)
{
	gpin_write(PIN_DRIVE_CLUTCH, clutch);
	auxprop_state.clutch = clutch;
}

void auxprop_enable(char enable)
{
	gpin_write(PIN_DRIVE_ENABLE, enable);
	auxprop_state.enable = enable;
}

void auxprop_moveSpeedDist(signed int speed, unsigned int dist)
{
	double d = dist; // 0.01m
	double s = speed; // 0.01m/s
	double t;
	systmr_t now;
	t = d / s; // t is in seconds
	t *= 1000; // now in milliseconds
	if (t < 0) {
		t *= -1;
	}

	if (speed == 0) {
		auxprop_state.state = AUXPROPSTATE_IDLE;
		auxprop_moveSpeed(0);
		return;
	}

	if (d == 0) {
		auxprop_state.state = AUXPROPSTATE_GO_UNLIMITED;
		auxprop_moveSpeed(speed);
		return;
	}

	auxprop_state.state = AUXPROPSTATE_GO_TIMED;
	now = systmr_nowMillis();
	auxprop_state.stop_time = now + lround(t);
	auxprop_moveSpeed(speed);
}

void auxprop_moveSpeedDistSingle(char which, signed int speed, unsigned int dist)
{
	double d = dist; // 0.01m
	double s = speed; // 0.01m/s
	double t;
	systmr_t now;
	t = d / s; // t is in seconds
	t *= 1000; // now in milliseconds
	if (t < 0) {
		t *= -1;
	}

	auxprop_moveSpeedSingle(which == LGUDRIVE_RIGHT ? LGUDRIVE_LEFT : LGUDRIVE_RIGHT, 0);

	if (speed == 0) {
		auxprop_state.state = AUXPROPSTATE_IDLE;
		auxprop_moveSpeedSingle(which, 0);
		return;
	}

	if (d == 0) {
		auxprop_state.state = AUXPROPSTATE_GO_SINGLE_UNLIMITED;
		auxprop_moveSpeedSingle(which, speed);
		return;
	}

	auxprop_state.state = AUXPROPSTATE_GO_SINGLE_TIMED;
	now = systmr_nowMillis();
	auxprop_state.stop_time = now + lround(t);
	auxprop_moveSpeedSingle(which, speed);
}

void auxprop_init()
{
	gpin_initPin(PIN_DRIVE_STEP_L, 1, 0);
	gpin_initPin(PIN_DRIVE_DIR_L, 1, 0);
	gpin_initPin(PIN_DRIVE_STEP_R, 1, 0);
	gpin_initPin(PIN_DRIVE_DIR_R, 1, 0);
	gpin_initPin(PIN_DRIVE_ENABLE, 1, 0);
	gpin_initPin(PIN_DRIVE_CLUTCH, 1, 0);
}

void auxprop_task()
{
	if (auxprop_state.state == AUXPROPSTATE_GO_TIMED || auxprop_state.state == AUXPROPSTATE_GO_SINGLE_TIMED)
	{
		systmr_t now = systmr_nowMillis();
		if (now > auxprop_state.stop_time) {
			auxprop_moveSpeed(0);
			auxprop_state.state = AUXPROPSTATE_IDLE;
		}
	}
}

void auxprop_report(uint8_t src)
{
	if (src == CMDSRC_SERTERM)
	{
		TERM_PRINTF("auxprop report %02X %02X %d\r\n"
				, auxprop_state.state
				, auxprop_state.clutch
				, auxprop_state.speed);
	}
	if (src == CMDSRC_PICOMM)
	{
		//PICOMM_TX_U8(AUXPROP_PIIDX_STATE, auxprop_state.state);
		//PICOMM_TX_U8(AUXPROP_PIIDX_CLUTCH, auxprop_state.clutch);
		//PICOMM_TX_S32(AUXPROP_PIIDX_SPEED, auxprop_state.speed);
	}
}
