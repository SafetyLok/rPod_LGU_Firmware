#ifndef _GPIN_H_
#define _GPIN_H_

#include <stdint.h>

typedef int32_t pinnum_t;

#define PIN_GIO_A    0x000
#define PIN_GIO_B    0x008
#define PIN_IS_MUXED 0x010
#define PIN_ADC_1    0x040
#define PIN_ADC_2    0x080
#define PIN_PWM      0x100
#define PIN_OTHER    0x800
#define PIN_NONE     (-1)

enum
{
	PINHEAD_J2_P2   = PIN_ADC_1 | 16,
	PINHEAD_J2_P3   = PIN_OTHER,
	PINHEAD_J2_P4   = PIN_OTHER,
	PINHEAD_J2_P5   = PIN_GIO_A | 7,
	PINHEAD_J2_P6   = PIN_ADC_1 | 17,
	PINHEAD_J2_P8   = PIN_PWM   | 1,
	PINHEAD_J2_P9   = PIN_PWM   | 7,
	PINHEAD_J2_P10  = PIN_PWM   | 12,
	PINHEAD_J3_P3   = PIN_ADC_1 | 0,
	PINHEAD_J3_P4   = PIN_ADC_1 | 7,
	PINHEAD_J3_P5   = PIN_ADC_1 | 18,
	PINHEAD_J3_P6   = PIN_ADC_1 | 19,
	PINHEAD_J3_P7   = PIN_ADC_1 | 20,
	PINHEAD_J3_P8   = PIN_ADC_1 | 21,
	PINHEAD_J4_P1   = PIN_PWM   | 11,
	PINHEAD_J4_P2   = PIN_PWM   | 10,
	PINHEAD_J4_P7   = PIN_GIO_A | 5, // muxed
	PINHEAD_J4_P8   = PIN_GIO_A | 2, // muxed
	PINHEAD_J4_P9   = PIN_GIO_A | 1, // muxed
	PINHEAD_J4_P10  = PIN_GIO_A | 0, // muxed
	PINHEAD_J5_P4   = PIN_GIO_B | 3, // muxed
	PINHEAD_J5_P10  = PIN_GIO_B | 2, // muxed
	PINHEAD_J6_P2   = PIN_ADC_1 | 9,
	PINHEAD_J6_P3   = PIN_OTHER,
	PINHEAD_J6_P4   = PIN_OTHER,
	PINHEAD_J6_P5   = PIN_GIO_B | 2, // muxed
	PINHEAD_J7_P3   = PIN_ADC_1 | 10,
	PINHEAD_J7_P4   = PIN_ADC_1 | 2,
	PINHEAD_J7_P5   = PIN_ADC_1 | 3,
	PINHEAD_J7_P6   = PIN_ADC_1 | 11,
	PINHEAD_J7_P7   = PIN_ADC_1 | 4,
	PINHEAD_J7_P8   = PIN_ADC_1 | PIN_ADC_2 | 12,
	PINHEAD_J8_P7   = PIN_GIO_B | 1,
	PINHEAD_J8_P8   = PIN_GIO_B | 0,
	PINHEAD_J10_P8  = PIN_GIO_B | 1, // muxed
	PINHEAD_J10_P15 = PIN_GIO_B | 0, // muxed
	PINHEAD_J11_P5  = PIN_GIO_B | 2,
	PINHEAD_J11_P6  = PIN_GIO_B | 3, // muxed
	PINHEAD_J11_P7  = PIN_GIO_A | 0, // muxed
	PINHEAD_J11_P10 = PIN_GIO_A | 1, // muxed
	PINHEAD_J11_P12 = PIN_GIO_A | 2, // muxed
	PINHEAD_J11_P16 = PIN_GIO_A | 5, // muxed
	PINHEAD_J11_P18 = PIN_GIO_A | 6, // muxed
	PINHEAD_J11_P19 = PIN_GIO_A | 7, // muxed
	PINHEAD_LED2    = PIN_GIO_B | 1,
	PINHEAD_LED3    = PIN_GIO_B | 2,
	PINHEAD_S3      = PIN_GIO_B | 2,
	PINHEAD_J10_P41 = PIN_ADC_1 | PIN_ADC_2 | 15,
	PINHEAD_J10_P42 = PIN_ADC_1 | 23, // also AD2IN7
	PINHEAD_J10_P43 = PIN_ADC_1 | 8,  // also AD2IN8
	PINHEAD_J10_P44 = PIN_ADC_1 | PIN_ADC_2 | 14,
	PINHEAD_J10_P45 = PIN_ADC_1 | 22, // also AD2IN6
	PINHEAD_J10_P46 = PIN_ADC_1 | PIN_ADC_2 | 13,
	PINHEAD_J10_P47 = PIN_ADC_1 | 5,
	PINHEAD_J10_P48 = PIN_ADC_1 | 6,
	PINHEAD_J10_P49 = PIN_ADC_1 | PIN_ADC_2 | 12,
	PINHEAD_J10_P50 = PIN_ADC_1 | PIN_ADC_2 | 11,
	PINHEAD_J11_P41 = PIN_ADC_1 | 16, // also AD2IN0
	PINHEAD_J11_P42 = PIN_ADC_1 | 17, // also AD2IN1
	PINHEAD_J11_P43 = PIN_ADC_1 | 18, // also AD2IN2
	PINHEAD_J11_P44 = PIN_ADC_1 | 19, // also AD2IN3
	PINHEAD_J11_P45 = PIN_ADC_1 | 20, // also AD2IN4
	PINHEAD_J11_P46 = PIN_ADC_1 | 21, // also AD2IN5
	PINHEAD_J11_P49 = PIN_ADC_1 | PIN_ADC_2 | 9,
	PINHEAD_J11_P50 = PIN_ADC_1 | PIN_ADC_2 | 10,
	PINHEAD_J10_P5  = PIN_PWM   | 11,
	PINHEAD_J10_P6  = PIN_PWM   | 10,
	PINHEAD_J11_P20 = PIN_PWM   | 3,
	PINHEAD_J11_P24 = PIN_PWM   | 4,
	PINHEAD_J11_P25 = PIN_PWM   | 5,
	PINHEAD_J11_P26 = PIN_PWM   | 6,
	PINHEAD_J11_P27 = PIN_PWM   | 7,
	PINHEAD_J11_P28 = PIN_PWM   | 13,
	PINHEAD_J11_P29 = PIN_PWM   | 12,
	PINHEAD_J11_P33 = PIN_OTHER,
	PINHEAD_J11_P34 = PIN_OTHER,
};

void gpin_initHw(void);
void gpin_initAll(void);
void gpin_initPin(pinnum_t, signed char dir, signed char x);
void gpin_task(void);
char gpin_read(pinnum_t);
void gpin_write(pinnum_t, char);
void gpin_enableNotification(pinnum_t p);

#endif
