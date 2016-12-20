#ifndef _TERM_H_
#define _TERM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

#ifndef __AVR__
#define strlen_P strlen
#define strcmp_P strcmp
#define strncmp_P strncmp
#define vsprintf_P vsprintf
#define vsnprintf_P vsnprintf
#endif

#ifdef __AVR__
	#define TERM_STRNCMP(x, y, n, i) term_strncmp(PSTR(x), (char*)(y), (signed int)(n), (int*)(i))
	#define TERM_PRINTF(fmt, args...) term_printf(PSTR(fmt), ##args)
	#error
#else
	#define TERM_STRNCMP(x, y, n, i) term_strncmp((const char *)(x), (char*)(y), (signed int)(n), (int*)(i))
	#define TERM_PRINTF term_printf
#endif

// implemented elsewhere
extern char app_handleTerm(char*); // returns 1 if successful, 0 if the command is not understood
extern int16_t term_hal_read(void); // returns the next character from the buffer if available, returns -1 if no character in buffer
extern void term_hal_tx(char); // sends a character out to the terminal
// a file named term_hal.c should be the one that contains interfaces to either a physical or virtual serial port

// public functions, see code for detailed comments
void term_task(void);
void term_printHex(uint8_t* data, int length);
int term_parseInts(char* s, int32_t* data, int limit);
int term_parseBytes(char* s, uint8_t* data, int limit);
int term_parseDoubles(char* s, double* data, int limit);

// if on the AVR platform, do not use these below directly, use the capitalized macro versions instead
signed int term_strncmp(const char* x, char* y, signed int n, int* i);
int term_printf(const char* fmt, ...);

#endif
