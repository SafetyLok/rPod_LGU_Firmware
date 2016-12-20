#include <stdio.h>
#include <stdint.h>

char* version_string()
{
	static char buff[256];
	int idx = 0;
	idx += sprintf(&buff[idx], "Landing Gear Unit Firmware, ");
	idx += sprintf(&buff[idx], "Compiled on " __DATE__ ", " __TIME__", ");
	#ifdef __GNUC__
	idx += sprintf(&buff[idx], "GNU C %d", __GNUC__);
	#ifdef __GNUC_MINOR__
	idx += sprintf(&buff[idx], ".%d", __GNUC_MINOR__);
	#ifdef __GNUC_PATCHLEVEL__
	idx += sprintf(&buff[idx], ".%d", __GNUC_PATCHLEVEL__);
	#endif
	#endif
	#elif defined(__ARMCC_VERSION)
	idx += sprintf(&buff[idx], "ARMCC %u", __ARMCC_VERSION);
	#elif defined(__TI_COMPILER_VERSION__)
	idx += sprintf(&buff[idx], "TI ARM Compiler %u", __TI_COMPILER_VERSION__);
	#else
	idx += sprintf(&buff[idx], "unknown compiler");
	#endif
	return (char*)buff;
}
