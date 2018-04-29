#include "si7021.h"
#include "uart.h"
#include "gas_flame.h"

int startup()
{
    //call init functions

            if(ConfigureUART2() == -1)
            {
                return -1;
            }

            Queue_init();

            if(adc_ch0_init() == -1) // ADC for gas sensor
            {
                return -1;
            }

            if(adc_ch1_init() == -1) // ADC for flame sensor
            {
                return -1;
            }

            if(i2c_init() == -1)
            {
                return -1;
            }

            return 0;
    }
