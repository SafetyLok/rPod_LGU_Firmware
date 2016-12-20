#include "term.h"
#include <stdlib.h>
#include <string.h>
#include <systmr.h>

#ifdef __AVR__
#define TERM_BUFF_SIZE 64
#else
#define TERM_BUFF_SIZE 1024
#endif

systmr_t term_loginTime = 0;
char term_loggedIn = 0;

char term_handle(char* s); // private, defined below

char* version_string(void);

/*
this needs to be called frequently but it is low priority if the system has other more important tasks
you only really need to respond as fast as a human can type
the job of this function is to collect a single line of input and then submit it to the handler
pressing backspace, delete, or ESC will cause the entire line to be cancelled
pressing tab will recall the previous command
pressing enter will execute the command
the previous command will be remembered if it executed correctly, otherwise it will not be remembered
*/
void term_task()
{
	int16_t c;

	static char buffer[TERM_BUFF_SIZE];
	static char history[TERM_BUFF_SIZE];
	static int buff_idx = 0;

	c = term_hal_read();

	if (c < 0) { // no char received
		if (term_loggedIn != 0) {
			if ((systmr_nowMillis() - term_loginTime) > 60000) {
				term_loggedIn = 0;
				TERM_PRINTF("\r\nlogin has expired\r\n>");
			}
		}
		return;
	}

	if (c == 0x08 || c == 0x18 || c == 0x1B || c == 0x7F || c == '>')
	{
		// backspace, cancel, or ESC
		// clear the buffer and start over
		TERM_PRINTF("\r\ncancelled\r\n>");
		buff_idx = 0;
		buffer[0] = 0;
		return;
	}

	if (term_loggedIn != 0) {
		term_loginTime = systmr_nowMillis();
	}

	if (c == '\0' || c == '\r' || c == '\n')
	{
		if (buff_idx <= 0) { // already blank
			// reset and do nothing
			buff_idx = 0;
			buffer[0] = 0;
			TERM_PRINTF("\r\n>");
			return;
		}

		TERM_PRINTF("\r\nexecute %s\r\n", buffer); // unnecessary but it is nice to have a visisual confirmation

		// submit
		if (term_handle(buffer) != 0) {
			// true means copy into history
			strncpy(history, buffer, TERM_BUFF_SIZE);
		}

		// reset
		buff_idx = 0;
		buffer[0] = 0;
		TERM_PRINTF("\r\n>");
	}
	else if (c == '\t')
	{
		// tab key means previous command
		strncpy(buffer, history, TERM_BUFF_SIZE); // load previous string
		TERM_PRINTF("\r\n>%s", buffer); // show user previous string
		buff_idx = strlen(buffer); // start typing at the end
	}
	else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == ' ' || c == '.')
		// check against allowable characters
	{
		// save one character
		buffer[buff_idx] = (char)c;
		buff_idx += 1;
		buffer[buff_idx] = 0; // null terminate
		TERM_PRINTF("%c", c); // local echo
	}
}

/*
handles just a sanity "ping" sanity check for the terminal
anything else is passed to the application layer for handling
returns 1 if command should be remembered in history buffer, otherwise return 0
*/
char term_handle(char* s)
{
	int i;
	if (TERM_STRNCMP("ping", s, 0, NULL) == 0) {
		TERM_PRINTF("pong\r\n");
		return 0;
	}
	if (TERM_STRNCMP("parseints ", s, -1, &i) == 0) {
		int j;
		int32_t data[32];
		j = term_parseInts(&s[i], data, 32);
		term_printHex(data, j * sizeof(int32_t));
		return 0;
	}
	if (TERM_STRNCMP("time", s, 0, NULL) == 0) {
		TERM_PRINTF("%u\r\n", systmr_nowMillis());
		return 0;
	}
	if (TERM_STRNCMP("version", s, 0, NULL) == 0) {
		TERM_PRINTF("%s\r\n", version_string());
		return 0;
	}
	if (TERM_STRNCMP("login", s, 0, NULL) == 0) {
		if (term_loggedIn == 0) {
			TERM_PRINTF("welcome\r\n");
		}
		else {
			TERM_PRINTF("repeated login\r\n");
		}
		term_loginTime = systmr_nowMillis();
		term_loggedIn = 1;
		return 0;
	}
	if (TERM_STRNCMP("logout", s, 0, NULL) == 0) {
		if (term_loggedIn != 0) {
			TERM_PRINTF("goodbye\r\n");
		}
		else {
			TERM_PRINTF("already logged out\r\n");
		}
		term_loggedIn = 0;
		return 0;
	}
	if (term_loggedIn == 0)
	{
		TERM_PRINTF("permission denied, please log in\r\n");
		return 0;
	}
	if (app_handleTerm(s) == 0) {
		TERM_PRINTF("no such command\r\n");
		return 0;
	}
	return 1;
}

/*
this function is an intelligent string comparison made useful for a command line interpreter
do not use this function directly, use the capitalized macro instead: TERM_STRNCMP

param x: constant string or command header, should be in PGMSPACE if on AVR platform
param y: user input string
param n: 
			n > 0 means number of chars to check
			n == 0 means use strcmp without length limit
			n < 0 means use the length of x
output *i: if i is not null, then *i will contain the length of x
returns: 0 if strings matches, otherwise, the result from the strcmp function is returned
*/
signed int term_strncmp(const char* x, char* y, signed int n, int* i)
{
	int sl = (int)strlen_P(x);
	if (i != NULL) {
		*i = sl;
	}
	if (n == 0) {
		return -(signed int)strcmp_P((const char *)y, (const char *)x);
	}
	else if (n < 0) {
		return -(signed int)strncmp_P((const char *)y, (const char *)x, sl);
	}
	return -(signed int)strncmp_P((const char *)y, (const char *)x, n);
}

/*
printf for the terminal
do not use this function directly, use the capitalized macro instead: TERM_PRINTF
param fmt: the format string, should be in PGMSPACE if on AVR platform, otherwise, do not care
returns: length of string printed
*/
int term_printf(const char* fmt, ...)
{
	char buff[TERM_BUFF_SIZE];
	int i, j;
	va_list args;
	va_start(args, fmt);
	i = vsnprintf_P(buff, TERM_BUFF_SIZE, fmt, args);
	va_end(args);
	for (j = 0; j < i; j++) {
		term_hal_tx(buff[j]);
	}
	return j;
}

// prints out a string of hex
void term_printHex(uint8_t* data, int length)
{
	int i;
	for (i = 0; i < length; i++) {
		TERM_PRINTF(" %02X", data[i]);
	}
}

int term_parseInts(char* s, int32_t* data, int limit)
{
	int i, j;
	char* pstart = s;
	char* pend;
	j = strlen(s);
	for (i = 0; ; i++)
	{
		int32_t x;
		x = strtol(pstart, &pend, 0);
		if (pstart == pend) {
			break;
		}
		pstart = pend;
		if (i < limit) {
			data[i] = x;
		}
	}
	return i;
}

int term_parseBytes(char* s, uint8_t* data, int limit)
{
	int i, j;
	char* pstart = s;
	char* pend;
	j = strlen(s);
	for (i = 0; ; i++)
	{
		int32_t x;
		x = strtol(pstart, &pend, 0);
		if (pstart == pend) {
			break;
		}
		pstart = pend;
		if (i < limit) {
			data[i] = (uint8_t)x;
		}
	}
	return i;
}

int term_parseDoubles(char* s, double* data, int limit)
{
	int i, j;
	char* pstart = s;
	char* pend;
	j = strlen(s);
	for (i = 0; ; i++)
	{
		double x;
		x = strtod(pstart, &pend);
		if (pstart == pend) {
			break;
		}
		pstart = pend;
		if (i < limit) {
			data[i] = x;
		}
	}
	return i;
}
