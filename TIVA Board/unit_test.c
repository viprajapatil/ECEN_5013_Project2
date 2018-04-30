/* File: unit_test.c
 * Description: We test various APIs and functions in this file
 * */
#include "unit_test.h"

SemaphoreHandle_t my_sem;
SemaphoreHandle_t sem_uart;

int test_gas()
{
    uint32_t ADC0Value[1];
    float f;

        ADCProcessorTrigger(ADC0_BASE, 3);
        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
        }
        ADCIntClear(ADC0_BASE, 3);
        ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);

        f = co_val(ADC0Value[0]);

        if (f >= 3.5)
            return SUCCESS;
        else return FAIL;

    }

int test_flame()
{
    uint32_t ADC1Value[1];
            ADCProcessorTrigger(ADC1_BASE, 3);
            while(!ADCIntStatus(ADC1_BASE, 3, false))
            {
            }
            ADCIntClear(ADC1_BASE, 3);
            ADCSequenceDataGet(ADC1_BASE, 3, ADC1Value);

        if (200 <= ADC1Value[0] && ADC1Value[0] <= 5000)
        {
            return SUCCESS;
        }
        return FAIL;
    }

int test_temp()
{
    tp = i2cRead(TEMP_ADDR);
    temp_val = temp(tp);

     if (temp_val > 20)
         return SUCCESS;
     else return FAIL;
    }

int alert_gas()
{
    uint32_t ADC0Value[1];
        float f;

            ADCProcessorTrigger(ADC0_BASE, 3);
            while(!ADCIntStatus(ADC0_BASE, 3, false))
            {
            }
            ADCIntClear(ADC0_BASE, 3);
            ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);

            f = co_val(ADC0Value[0]);

            if (f>9)
                return SUCCESS;
            else return FAIL;
    }

int alert_gas_disconnected()
{
    uint32_t ADC0Value[1];
        float f;

            ADCProcessorTrigger(ADC0_BASE, 3);
            while(!ADCIntStatus(ADC0_BASE, 3, false))
            {
            }
            ADCIntClear(ADC0_BASE, 3);
            ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);

            f = co_val(ADC0Value[0]);

            if (f<3.5)
                return SUCCESS;
            else return FAIL;
    }

int alert_flame()
{
    uint32_t ADC1Value[1];
               ADCProcessorTrigger(ADC1_BASE, 3);
               while(!ADCIntStatus(ADC1_BASE, 3, false))
               {
               }
               ADCIntClear(ADC1_BASE, 3);
               ADCSequenceDataGet(ADC1_BASE, 3, ADC1Value);

           if (200 <= ADC1Value[0] && ADC1Value[0] <= 500)
           {
               return SUCCESS;
           }
           else return FAIL;
    }

int alert_flame_disconnected()
{
    uint32_t ADC1Value[1];
    ADCProcessorTrigger(ADC1_BASE, 3);
    while(!ADCIntStatus(ADC1_BASE, 3, false))
    {
    }
    ADCIntClear(ADC1_BASE, 3);
    ADCSequenceDataGet(ADC1_BASE, 3, ADC1Value);

    if (ADC1Value[0] < 50)
    {
        return SUCCESS;
     }
    else return FAIL;
    }

int alert_temp()
{
    tp = i2cRead(TEMP_ADDR);
        temp_val = temp(tp);

         if (temp_val > 35)
             return SUCCESS;
         else return FAIL;

    }
