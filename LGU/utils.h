#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

#define DEBUG_PRINTF TERM_PRINTF
#define ERROR_PRINTF TERM_PRINTF

extern char* version_string(void);
extern char test_termHandle(char* s);

#endif
