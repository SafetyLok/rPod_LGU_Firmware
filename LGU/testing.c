#include <stdio.h>
#include <main.h>
#include <sys_core.h>
#include <systmr.h>
#include <term.h>
#include <ser.h>
#include <a2d.h>
#include <pwm.h>
#include <gpin.h>
#include <linear_actuator.h>
#include <lift.h>
#include <auxprop.h>
#include <picomm.h>

extern void picomm_hal_tx(uint8_t x);
extern int16_t term_hal_read(void);

char test_termHandle(char* s)
{
	int i;
	char* pstart;
	char* pend;
	if (TERM_STRNCMP("adc ", s, -1, &i) == 0)
	{
		int j;
		j = strtol(&s[i], NULL, 0);
		if (j >= 0) {
			TERM_PRINTF("adc[%u] = %u\r\n", j, a2d_getRaw(j));
		}
		else {
			for (j = 0; j < 32; j++) {
				TERM_PRINTF("adc[%u] = %u\r\n", j, a2d_getRaw(j));
			}
		}
		return 1;
	}
	if (TERM_STRNCMP("adcc", s, 0, NULL) == 0)
	{
		TERM_PRINTF("adc c = %u\r\n", adc_conversionCount);
		return 1;
	}
	if (TERM_STRNCMP("pwm ", s, -1, &i) == 0)
	{
		int which, duty;
		pstart = &s[i];
		which = strtol(pstart, &pend, 0);
		duty = strtol(pend, &pend, 0);
		if (duty < 0 || duty > 100) {
			TERM_PRINTF("invalid duty value\r\n");
			return 1;
		}
		if (which >= 0) {
			pwm_set(which, (uint8_t)duty);
		}
		else {
			for (which = 0; which < 14; which++) {
				pwm_set(which, (uint8_t)duty);
			}
		}
		return 1;
	}
	if (TERM_STRNCMP("gpin ", s, -1, &i) == 0)
	{
		int pin, dir, val;
		pstart = &s[i];
		pin = strtol(pstart, &pend, 0);
		dir = strtol(pend, &pend, 0);
		val = strtol(pend, &pend, 0);
		gpin_initPin(pin, dir, val);
		return 1;
	}
	if (TERM_STRNCMP("gpinread ", s, -1, &i) == 0)
	{
		int pin;
		char val;
		pstart = &s[i];
		pin = strtol(pstart, &pend, 0);
		val = gpin_read(pin);
		TERM_PRINTF("pin[0x%04X] = %02X\r\n", pin, val);
		return 1;
	}
	if (TERM_STRNCMP("pitx ", s, -1, &i) == 0)
	{
		int len;
		pstart = &s[i];
		len = strlen(pstart);
		for (i = 0; i < len; i++) {
			picomm_hal_tx((uint8_t)pstart[i]);
		}
		picomm_hal_tx((uint8_t)'\r');
		picomm_hal_tx((uint8_t)'\n');
		return 1;
	}
	if (TERM_STRNCMP("picommtx ", s, -1, &i) == 0)
	{
		int len;
		int data[8 + 2];
		uint8_t data8[8];
		pstart = &s[i];
		len = term_parseInts(pstart, data, 8 + 2);
		if (len > (8 + 2)) {
			TERM_PRINTF("too many bytes\r\n");
			return 1;
		}
		for (i = 0; i < 8; i++) {
			data8[i] = (uint8_t)data[i + 2];
		}
		picomm_tx((uint16_t)data[0], (uint8_t)data[1], (uint8_t)len, data8);
		return 1;
	}
	if (TERM_STRNCMP("liftreport", s, 0, NULL) == 0)
	{
		lift_report(CMDSRC_SERTERM, -1);
		return 1;
	}
	if (TERM_STRNCMP("liftreport ", s, -1, &i) == 0)
	{
		int m;
		pstart = &s[i];
		m = strtol(pstart, &pend, 0);
		lift_report(CMDSRC_SERTERM, m);
		return 1;
	}
	if (TERM_STRNCMP("driveleft ", s, -1, &i) == 0)
	{
		int speed, dist;
		pstart = &s[i];
		speed = strtol(pstart, &pend, 0);
		dist = strtol(pend, &pend, 0);
		if (speed < -1000 || speed > 1000) {
			TERM_PRINTF("invalid speed value\r\n");
			return 1;
		}
		if (dist < 0) {
			TERM_PRINTF("invalid dist value\r\n");
			return 1;
		}
		auxprop_moveSpeedDistSingle(LGUDRIVE_LEFT, speed, dist);
		return 1;
	}
	if (TERM_STRNCMP("driveright ", s, -1, &i) == 0)
	{
		int speed, dist;
		pstart = &s[i];
		speed = strtol(pstart, &pend, 0);
		dist = strtol(pend, &pend, 0);
		if (speed < -1000 || speed > 1000) {
			TERM_PRINTF("invalid speed value\r\n");
			return 1;
		}
		if (dist < 0) {
			TERM_PRINTF("invalid dist value\r\n");
			return 1;
		}
		auxprop_moveSpeedDistSingle(LGUDRIVE_RIGHT, speed, dist);
		return 1;
	}
	if (TERM_STRNCMP("auxpropreport", s, 0, NULL) == 0)
	{
		auxprop_report(CMDSRC_SERTERM);
		return 1;
	}
	return 0;
}

void manlift(uint8_t src, int motor, signed int speed, unsigned int time)
{
	systmr_t start = systmr_nowMillis();
	systmr_t msgTime = 0;
	systmr_t now;
	TERM_PRINTF("manual lift %d %d %d\r\n", motor, speed, time);
	linearmotor_movei(motor, speed);
	while (((now = systmr_nowMillis()) - start) < time)
	{
		if (term_hal_read() >= 0) {
			TERM_PRINTF("emergency ");
			break;
		}
		if ((now - msgTime) > 250)
		{
			lift_report(src, motor);
			msgTime = now;
		}
		picomm_task();
		gpin_task();
		a2d_task();
	}
	linearmotor_movei(motor, 0);
	TERM_PRINTF("stop\r\n");
	if (src != CMDSRC_SERTERM) {
		lift_report(src, motor);
	}
}
