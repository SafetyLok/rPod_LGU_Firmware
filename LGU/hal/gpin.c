#include <gpin.h>
#include <gio.h>
#include <a2d.h>
#include <pwm.h>

#define GPIN_ADC_THRESH 1024

#define ERROR_PRINTF TERM_PRINTF
#ifdef ERROR_PRINTF
#include <term.h>
#endif

const pinnum_t gpin_initTable[] = {
	-1, -1, -1
};

void gpin_initAll()
{
	int i;
	for (i = 0; ;i++)
	{
		pinnum_t p;
		pinnum_t d;
		pinnum_t v;
		p = gpin_initTable[(i * 3) + 0];
		d = gpin_initTable[(i * 3) + 1];
		v = gpin_initTable[(i * 3) + 2];
		if (p < 0) {
			break;
		}
		gpin_initPin(p, d, v);
	}
}

void gpin_initPin(pinnum_t p, signed char dir, signed char x)
{
	gioPORT_t* g;
	if ((p & 0xFFF8) == 0) {
		g = gioPORTA;
	}
	else if ((p & PIN_GIO_B) != 0) {
		g = gioPORTB;
	}

	if (g != NULL && dir != 0) {
		g->DIR |= (1 << (p & 0x7));
		if (x >= 0) {
			gpin_write(p, x);
		}
	}
	else if (g != NULL && dir == 0) {
		g->DIR &= ~(1 << (p & 0x7));
		if (x == 0) {
			g->PULDIS |= (1 << (p & 0x7));
		}
		else {
			if (x > 0) {
				g->PSL |= (1 << (p & 0x7));
			}
			else if (x < 0) {
				g->PSL &= ~(1 << (p & 0x7));
			}
			g->PULDIS &= ~(1 << (p & 0x7));
		}
	}
	else if ((p & PIN_ADC_1) != 0 || (p & PIN_ADC_2) != 0)
	{
		if (dir != 0)
		{
			ERROR_PRINTF("pin 0x%08X is ADC trying to be an output\r\n", p);
		}
		else if (x != 0)
		{
			ERROR_PRINTF("pin 0x%08X is ADC trying to be pulled 0x%02X\r\n", p, x);
		}
	}
	else if ((p & PIN_PWM) != 0)
	{
		if (dir == 0)
		{
			ERROR_PRINTF("pin 0x%08X is PWM trying to be an input\r\n", p);
		}
		else if (x < 0)
		{
			ERROR_PRINTF("pin 0x%08X is PWM trying to output negative 0x%02X\r\n", p, x);
		}
		else if (x != 0)
		{
			pwm_set(p & 0xFF, 100);
		}
		else if (x == 0)
		{
			pwm_set(p & 0xFF, 0);
		}
	}
}

void gpin_initHw()
{
	gioInit();
}

void gpin_task()
{
	a2d_task();
}

void gpin_enableNotification(pinnum_t p)
{
	gioPORT_t* g = NULL;
	if (p == PIN_NONE) {
		return;
	}
	if ((p & 0xFFF8) == 0) {
		g = gioPORTA;
	}
	else if ((p & PIN_GIO_B) != 0) {
		g = gioPORTB;
	}
	if (g != NULL) {
		gioEnableNotification(g, p & 0x07);
	}
	else {
		ERROR_PRINTF("bad pin 0x%08X attempted to enable notifications\r\n", p);
	}
}

char gpin_read(pinnum_t p)
{
	uint32_t x;
	if ((p & 0xFFF8) == 0) {
		return gioGetBit(gioPORTA, (uint32_t)(p & (uint32_t)0x7));
	}
	else if ((p & PIN_GIO_B) != 0) {
		return gioGetBit(gioPORTB, (uint32_t)(p & (uint32_t)0x7));
	}
	else if ((p & PIN_ADC_1) != 0) {
		x = a2d_getRaw(PIN2ADCCHAN(p));
		return x > GPIN_ADC_THRESH;
	}
	else if ((p & PIN_ADC_2) != 0) {
		x = a2d_getRaw(PIN2ADCCHAN(p));
		return x > GPIN_ADC_THRESH;
	}
	ERROR_PRINTF("invalid pin 0x%08X used for gpin_read\r\n", p);
	return 0; // ERROR
}

void gpin_write(pinnum_t p, char x)
{
	if ((p & 0xFFF8) == 0) {
		gioSetBit(gioPORTA, (uint32_t)(p & (uint32_t)0x7), (uint32_t)x);
	}
	else if ((p & PIN_GIO_B) != 0) {
		gioSetBit(gioPORTB, (uint32_t)(p & (uint32_t)0x7), (uint32_t)x);
	}
	else if ((p & PIN_PWM) != 0) {
		pwm_setDigital((p & 0xF), x);
	}
	else {
		ERROR_PRINTF("invalid pin 0x%08X used for gpin_write\r\n", p);
	}
}

extern void linearmotor_gioNotification(void);
void gioNotification(gioPORT_t *port, uint32 bit)
{
	linearmotor_gioNotification();
}
