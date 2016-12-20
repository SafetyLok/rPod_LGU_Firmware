#include <main.h>
#include <utils.h>
#include <stdio.h>
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
#include <lgu_commands.h>
#include <pin_assignments.h>
#include <gio.h>

int main(void)
{
	systmr_t timestamp, now;
	gioInit();
	ser_init();
	systmr_init();
	a2d_init();
	pwm_init();
	linearmotor_init();
	auxprop_init();
	_enable_interrupt_();
	_enable_IRQ();

	TERM_PRINTF("Hello World, %s\r\n", version_string());

	timestamp = 0;

	while (1)
	{
		term_task();
		//lift_task();
		auxprop_task();
		picomm_task();
		gpin_task();
		a2d_task();
		now = systmr_nowMillis();
		if ((now - timestamp) > 1000)
		{
			timestamp = now;
		}
	}

	
	return 0;
}

void cmdcomm_handler(char src, uint16_t cmd, uint8_t* data, uint8_t length)
{
	static uint32_t seq = 1;

	uint64_t valu64 = *((uint64_t*)data);
	int32_t valu32 = *((int32_t*)data);
	uint64_t ret;

	seq++;

	ret = seq;
	ret |= ((uint64_t)cmd << (uint64_t)32);

	switch (cmd)
	{
		case LGUCMD_STOP_ALL:
			if (src == CMDSRC_PICOMM) {
				PICOMM_TX_U64(LGUREPORT_ACK, ret);
			}
			stop_all();
			break;
		case LGUCMD_LIFT_STOP_ALL:
			if (src == CMDSRC_PICOMM) {
				PICOMM_TX_U64(LGUREPORT_ACK, ret);
			}
			stop_allLifts();
			break;
		case LGUCMD_DRIVE_STOP_ALL:
			if (src == CMDSRC_PICOMM) {
				PICOMM_TX_U64(LGUREPORT_ACK, ret);
			}
			stop_allAuxprop();
			break;
		case LGUCMD_LIFT_REPORT_ALL:
			lift_report(src, -1);
			break;
		case LGUCMD_LIFT_REPORT_SINGLE:
			lift_report(src, valu32);
			break;
		case LGUCMD_LIFT_MANUAL_RAISE_ALL:
		case LGUCMD_LIFT_MANUAL_LOWER_ALL:
			{
				int16_t* data16 = (int16_t*)data;
				if (data16[0] > 0 && data16[0] < 6000 && data16[1] >= 0 && data16[1] <= 100) {
					manlift(src, -1, data16[1] * (cmd == LGUCMD_LIFT_MANUAL_LOWER_ALL) ? -1 : 1, data16[0]);
				}
				else {
					if (src == CMDSRC_PICOMM) {
						PICOMM_TX_U64(LGUREPORT_NACK, ret);
					}
				}
			}
			break;
		case LGUCMD_DRIVE_MOVE_ALL:
			{
				int32_t* data32 = (int32_t*)data;
				if (data32[0] >= -1000 && data32[1] <= 1000 && data32[1] >= 0) {
					auxprop_moveSpeedDist(data32[0], data32[1]);
					auxprop_report(src);
				}
				else {
					if (src == CMDSRC_PICOMM) {
						PICOMM_TX_U64(LGUREPORT_NACK, ret);
					}
				}
			}
			break;
		case LGUCMD_DRIVE_MOVE_LEFT:
			{
				int32_t* data32 = (int32_t*)data;
				if (data32[0] >= -1000 && data32[1] <= 1000 && data32[1] >= 0) {
					auxprop_moveSpeedDistSingle(LGUDRIVE_LEFT, data32[0], data32[1]);
					auxprop_report(src);
				}
				else {
					if (src == CMDSRC_PICOMM) {
						PICOMM_TX_U64(LGUREPORT_NACK, ret);
					}
				}
			}
			break;
		case LGUCMD_DRIVE_MOVE_RIGHT:
			{
				int32_t* data32 = (int32_t*)data;
				if (data32[0] >= -1000 && data32[1] <= 1000 && data32[1] >= 0) {
					auxprop_moveSpeedDistSingle(LGUDRIVE_RIGHT, data32[0], data32[1]);
					auxprop_report(src);
				}
				else {
					if (src == CMDSRC_PICOMM) {
						PICOMM_TX_U64(LGUREPORT_NACK, ret);
					}
				}
			}
			break;
		case LGUCMD_DRIVE_CLUTCH:
			auxprop_clutch(data[0]);
			auxprop_report(src);
			break;
		case LGUCMD_DRIVE_REPORT_ALL:
			auxprop_report(src);
			break;
		default:
			ERROR_PRINTF("\r\ncmdcomm unhandled cmd 0x%04X\r\n", cmd);
			if (src == CMDSRC_PICOMM) {
				PICOMM_TX_U64(LGUREPORT_NACK, ret);
			}
			break;
	}
}

void picomm_appHandler(uint16_t idx, uint8_t type, uint8_t length, uint8_t* data)
{
#ifdef DEBUG_PRINTF
	DEBUG_PRINTF("\r\nPiComm RX idx 0x%04X type %02X length %u data", idx, type, (uint32_t)length);
	term_printHex(data, length);
	DEBUG_PRINTF("\r\n");
#endif
	cmdcomm_handler(CMDSRC_PICOMM, idx, data, length);
}

char app_handleTerm(char* s)
{
	int i;
	char* pstart;
	char* pend;
	if (test_termHandle(s) != 0) {
		return 1;
	}
	if (TERM_STRNCMP("linearmotor ", s, -1, &i) == 0)
	{
		int which, duty;
		pstart = &s[i];
		which = strtol(pstart, &pend, 0);
		duty = strtol(pend, &pend, 0);
		if (duty < -100 || duty > 100) {
			TERM_PRINTF("invalid duty value\r\n");
			return 1;
		}
		linearmotor_brake(0);
		if (which >= 0) {
			linearmotor_movei(which, (signed int)duty);
		}
		else {
			for (which = 0; which < 4; which++) {
				linearmotor_movei(which, (signed int)duty);
			}
		}
		return 1;
	}
	if (TERM_STRNCMP("manlift ", s, -1, &i) == 0)
	{
		int which, duty, time;
		pstart = &s[i];
		which = strtol(pstart, &pend, 0);
		duty = strtol(pend, &pend, 0);
		time = strtol(pend, &pend, 0);
		if (duty < -100 || duty > 100) {
			TERM_PRINTF("invalid duty value\r\n");
			return 1;
		}
		linearmotor_brake(0);
		if (time < 0 || (time >= 5000 && duty >= 100)) {
			TERM_PRINTF("invalid time value\r\n");
			return 1;
		}
		manlift(CMDSRC_SERTERM, which, (signed int)duty, time);
		return 1;
	}
	if (TERM_STRNCMP("drive ", s, -1, &i) == 0)
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
		auxprop_moveSpeedDist(speed, dist);
		return 1;
	}
	if (TERM_STRNCMP("clutch ", s, -1, &i) == 0)
	{
		int clutch;
		pstart = &s[i];
		clutch = strtol(pstart, &pend, 0);
		auxprop_clutch(clutch);
		return 1;
	}
	if (TERM_STRNCMP("stop", s, 0, NULL) == 0)
	{
		stop_all();
	}
	return 0;
}

void stop_all()
{
	stop_allLifts();
	stop_allAuxprop();
}

void stop_allLifts()
{
	linearmotor_movei(-1, 0);
	lift_cancelAll();
}

void stop_allAuxprop()
{
	auxprop_moveSpeedDist(0, 0);
	auxprop_clutch(0);
}
