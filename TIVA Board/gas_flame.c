#include "main.h"
#include "gas_flame.h"
#include "si7021.h"
#include "uart.h"
#include "math.h"

void adc_init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH0|ADC_CTL_IE|ADC_CTL_END);
    MAP_ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);

    //ROM_ADCHardwareOversampleConfigure(ADC0_BASE,0);
    ADCSequenceEnable(ADC0_BASE, 3);
    }

float co_val(uint32_t val)
{
    double e = 2.718;
    double f;
    f = (val*3.3)/4095;
    f = (1.0698*f);
    f = pow(e, f);
    f = 3.027 * f;
    f = (float)f;
    return f;
    }
