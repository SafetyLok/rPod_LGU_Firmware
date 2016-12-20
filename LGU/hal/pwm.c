#include <pwm.h>
#include <etpwm.h>

#define PWM_CLK_DIV 1

const etpwmBASE_t* etpwmList[] = {
    etpwmREG1,
    etpwmREG2,
    etpwmREG3,
    etpwmREG4,
    etpwmREG5,
    etpwmREG6,
    etpwmREG7,
//    etpwmREG8,
};

#define ETPWM(x) ((etpwmBASE_t*)etpwmList[x / 2])

void pwm_init()
{
	int i;
	etpwmInit();
	for (i = 0; i < 7; i++) {
		etpwmSetClkDiv((etpwmBASE_t*) etpwmList[i], ClkDiv_by_1, HspClkDiv_by_1);
	}
	etpwmStartTBCLK();
	for (i = 0; i < 14; i++) {
		pwm_set(i, 0);
	}
}

void pwm_set(int which, uint8 duty)
{
    uint16 duty16;
    etpwmBASE_t* ptr;
    which &= 0xFF;
    if (which >= 14) {
    	return;
    }
    duty16 = duty;
    ptr = ETPWM(which);
    duty16 *= 200 / (PWM_CLK_DIV);
    ptr->TBPRD = 20000 / (PWM_CLK_DIV);
    if ((which & 1) == 0) {
        ptr->CMPA = duty16;
    }
    else {
        ptr->CMPB = duty16;
    }
}

void pwm_setDigital(int which, char data)
{
    etpwmBASE_t* ptr;
    which &= 0xFF;
    if (which >= 14) {
    	return;
    }
    ptr = ETPWM(which);
    if ((which & 1) == 0) {
        ptr->CMPA = ptr->TBPRD;
    }
    else {
        ptr->CMPB = ptr->TBPRD;
    }
}

void pwm_setRaw(int which, uint32_t period, uint32_t duty)
{
    etpwmBASE_t* ptr;
    which &= 0xFF;
    if (which >= 14) {
    	return;
    }
    ptr = ETPWM(which);
    ptr->TBPRD = period;
    if ((which & 1) == 0) {
        ptr->CMPA = duty;
    }
    else {
        ptr->CMPB = duty;
    }
}

/*
 * available dividers: 1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 640, 768, 896, 1024, 1280, 1536, 1792
 */
void pwm_setClkDiv(int which, uint16_t div)
{
	etpwmBASE_t* ptr;
	int i, j;
	uint16_t tmp;
	uint32_t m, closest;
	int32_t diff;
	int besti, bestj;
	which &= 0xFF;
    if (which >= 14) {
    	return;
    }
    ptr = ETPWM(which);
	closest = 0x10000;
	for (i = 0; i <= 7; i++)
	{
		for (j = 0; j <= 7; j++)
		{
			tmp = j * 2;
			if (j == 0) {
				tmp = 1;
			}
			m = j * (1 << i);
			diff = div - m;
			if (diff < 0) {
				diff *= -1;
			}
			if (diff < closest) {
				closest = diff;
				besti = i;
				bestj = j;
				if (diff == 0) {
					break;
				}
			}
		}
	}
	tmp = ptr->TBCTL;
	tmp &= 0xE07F;
	tmp |= (besti << 10) | (bestj << 7);
	ptr->TBCTL = tmp;
}
