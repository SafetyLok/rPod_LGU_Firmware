#include "systmr.h"
#include <rti.h>
#include <sys_vim.h>

static volatile systmr_t systmr_msCnt;
static volatile char systmr_10msFlag;

void systmr_init()
{
    systmr_msCnt = 0;
    systmr_10msFlag = 0;
    rtiInit();
    //vimEnableInterrupt(2, SYS_IRQ);
    //vimEnableInterrupt(5, SYS_IRQ);
    rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
    rtiEnableNotification(rtiNOTIFICATION_COMPARE3);
    rtiStartCounter(rtiCOUNTER_BLOCK0);
}

systmr_t systmr_nowMillis()
{
    return systmr_msCnt;
}

char systmr_check10msFlag()
{
    return systmr_10msFlag;
}

void systmr_clear10msFlag()
{
    systmr_10msFlag = 0;
}

void rtiNotification(uint32_t notification)
{
    if (notification == rtiNOTIFICATION_COMPARE0) // every 1ms
    {
        systmr_msCnt++;
    }
    else if (notification == rtiNOTIFICATION_COMPARE3) // every 10ms
    {
        systmr_10msFlag = 1;
    }
}
