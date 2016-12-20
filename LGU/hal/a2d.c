#include "a2d.h"
#include <adc.h>

#ifdef A2D_USE_INTERRUPTS
#define A2C_DISABLEINTS() _disable_IRQ()
#define A2C_ENABLEINTS()  _enable_IRQ()
#else
#define A2C_DISABLEINTS()
#define A2C_ENABLEINTS()
#endif

#define ADC_CHANS_CNT 64
#define ADC_FILTER_CONST 0.125

int a2d_procFifo(adcBASE_t *adc, uint32 group);

// private arrays, these need to be accessed properly with appropriate APIs
static volatile uint16 adc_valRaw[ADC_CHANS_CNT];
static volatile char adc_valNew[ADC_CHANS_CNT];
static volatile float adc_valFiltered[ADC_CHANS_CNT];
volatile uint32_t adc_conversionCount;

void a2d_init()
{
    int i;
    for (i = 0; i < ADC_CHANS_CNT; i++)
    {
        adc_valRaw[i] = 0;
        adc_valNew[i] = 0;
        adc_valFiltered[i] = 0;
    }
    adc_conversionCount = 0;
    adcInit();
#ifdef A2D_USE_INTERRUPTS
    adcEnableNotification(adcREG1, adcGROUP1);
#else
    adcStartConversion(adcREG2, adcGROUP2);
#endif
    adcStartConversion(adcREG1, adcGROUP1);
}

void a2d_task()
{
	#ifndef A2D_USE_INTERRUPTS
	int cnt;
	int total = 0;
	if (adcIsConversionComplete(adcREG1, adcGROUP1) != 0)
	{
    	adc_conversionCount++;
		do
		{
			cnt = a2d_procFifo(adcREG1, adcGROUP1);
			total += cnt;
		}
		while (cnt > 0 && total < 64);
    	adcStartConversion(adcREG1, adcGROUP1);
    }
	/*
	do
	{
		cnt = a2d_procFifo(adcREG2, adcGROUP1);
	}
	while (cnt > 0);
	if (adcIsConversionComplete(adcREG2, adcGROUP1) != 0) {
    	adc_conversionCount++;
    }
    */
	#endif
}

int a2d_procFifo(adcBASE_t *adc, uint32 group)
{
	uint32 cnt;
    uint32 i, j;
    j = adc == adcREG1 ? 0 : 32;
    static adcData_t data[16]; // WARNING, I lack the understanding of how large this array needs to be
    cnt = adcGetData(adc, group, data);
    for (i = 0; i < cnt; i++)
    {
        adcData_t* ptr = &(data[i]);
        int id = ptr->id + j;
        if (id < ADC_CHANS_CNT) // range check
        {
            float old, x;

            // force the compiler into doing the exact instructions I need it to do
            // the copy must be a 32 bit copy, the best way to guarantee that the compiler does this
            // is to copy from one pointer to another
            // this makes the copy an atomic operation
            volatile float y;
            //volatile float* yptr;
            //volatile float* fptr;

            // this will always be an atomic 16 bit copy
            adc_valRaw[id] = ptr->value;

            // perform filtering
            x = (float)ptr->value;
            old = adc_valFiltered[id];
            y = (old * (1.0 - ADC_FILTER_CONST)) + (x * ADC_FILTER_CONST);
            adc_valFiltered[id] = y;

            // do the atomic 32 bit copy
            //yptr = (float*)(&y);
            //fptr = (float*)(&adc_valFiltered[id]);
            //*fptr = *yptr;

            // this will always be an atomic 8 bit write
            adc_valNew[id] = 1;
        }
    }
}

uint16_t a2d_getRaw(uint8 chan)
{
    volatile uint16_t r;
    a2d_task();
    A2C_DISABLEINTS();
    r = adc_valRaw[chan];
    A2C_ENABLEINTS();
    return r;
}

char a2d_checkNew(uint8 chan)
{
    volatile char r;
    a2d_task();
    A2C_DISABLEINTS();
    r = adc_valNew[chan];
    A2C_ENABLEINTS();
    return r;
}

float a2d_getFiltered(uint8 chan)
{
    volatile float r;
    if (chan >= ADC_CHANS_CNT) {
        // handle parameter exception
        return -1.0;
    }
    A2C_DISABLEINTS();
    r = adc_valFiltered[chan];
    A2C_ENABLEINTS();
    return r;
}

void adcNotification(adcBASE_t *adc, uint32 group)
{
	a2d_procFifo(adc, group);
    if (adcIsConversionComplete(adc, group) != 0) {
    	adc_conversionCount++;
    }
}
