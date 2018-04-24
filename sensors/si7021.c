#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "drivers/pinout.h"
#include "driverlib/gpio.h"
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include <stdio.h>
#include <stdlib.h>
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"

#define LED_TOGGLE 0x00000001
#define LOG_STRING 0x00000002

#define ULONG_MAX 0xFFFFFFFF

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/i2c.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define BAUD_RATE 115200
#define SysClock 120000000

// Demo Task declarations
void LED1Task(void *pvParameters);
void LED2Task(void *pvParameters);
void Task3(void *pvParameters);
void TimerCallback1(TimerHandle_t xTimer1);
void TimerCallback2(TimerHandle_t xTimer2);

uint32_t ADC0Value[1];
uint16_t samplePeriod;
uint32_t sequence;
volatile uint32_t rh, tp;
double humidity_val, temp_val;

#define SLAVE_ADDRESS 0x40
#define RH_ADDR 0xE5
#define TEMP_ADDR 0xE3


int a, b, c, d;

TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;
xQueueHandle queue_handle;

uint32_t g_ui32SysClock;

typedef struct notifying_data
{
    char message[50];
    TickType_t current_ticks;
}notifying_data;

void ConfigureUART(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    //Enable GPIO

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);    //Enable UART0

    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);                //Configure UART pins
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTStdioConfig(0, BAUD_RATE, g_ui32SysClock);         //Initialize UART
}

void i2c_init()
{
    //enable I2C module 0
                //SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

                //reset module
                //SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

                //enable GPIO peripheral that contains I2C 0
                SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

                // Configure the pin muxing for I2C0 functions on port B2 and B3.
                GPIOPinConfigure(GPIO_PB2_I2C0SCL);
                GPIOPinConfigure(GPIO_PB3_I2C0SDA);

                // Select the I2C function for these pins.
                GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
                GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

                SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);
                SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
                SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
                while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));

                I2CMasterInitExpClk(I2C0_BASE, g_ui32SysClock, false);

    }

uint32_t i2cRead(int RegAddr)
{
    uint16_t return_val[2];

       I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);

       //specify register to be read
       I2CMasterDataPut(I2C0_BASE, RegAddr);

       //send control byte and register address byte to slave device
       I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

       //wait for MCU to finish transaction
       while(I2CMasterBusy(I2C0_BASE));
       //while(!(I2CSlaveStatus(I2C0_BASE) & I2C_SLAVE_ACT_TREQ));
       //specify that we are going to read from slave device
       I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, true);

       //send control byte and read from the register we
       //specified
       I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

       //wait for MCU to finish transaction
       while(I2CMasterBusy(I2C0_BASE));

       //return data pulled from the specified register
       return_val[0] =  I2CMasterDataGet(I2C0_BASE);
       I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

       //wait for MCU to finish transaction
       while(I2CMasterBusy(I2C0_BASE));
       return_val[1] =  I2CMasterDataGet(I2C0_BASE);

       uint32_t r = (return_val[0] << 8 | return_val[1]);

       return r;

    }

double humidity(uint32_t rh)
{
    double final;
    final = rh*125;
    final = final/65536;
    final = final - 6;
    return final;
    }

double temp(uint32_t rh)
{
    double final;
    final = rh*175.72;
    final = final/65536;
    final = final - 46.85;
    return final;
    }


// Main function
int main(void)
{
        g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                    SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                    SYSCTL_CFG_VCO_480), SysClock);

        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    //Enable GPIO

        ConfigureUART();        //Initialize UART

        UARTprintf("This is Nikhil");

        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);    //Enable UART0
        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
        GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_3);
        ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
        ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH0|ADC_CTL_IE|ADC_CTL_END);
        MAP_ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);
        //ROM_ADCHardwareOversampleConfigure(ADC0_BASE,0);
        ADCSequenceEnable(ADC0_BASE, 3);

        PinoutSet(false, false);

        ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

        GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
        GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

        a = 0x00;
        b = 0x00;
        c = 0x00;
        d = 0x00;


       // SysCtlDelay(1000);
        //}


        // Create demo tasks

       // xTaskCreate(LED1Task, (const portCHAR *)"LED1",
       //         configMINIMAL_STACK_SIZE, NULL, 1, &Task1Handle);

        xTaskCreate(LED2Task, (const portCHAR *)"LED2",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task2Handle);

        xTaskCreate(Task3, (const portCHAR *)"Task_3",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task3Handle);

        vTaskStartScheduler();

        return 0;
}

void TimerCallback1(TimerHandle_t xTimer1)
{
    //ADCIntClear(ADC0_BASE, 3);
    ADCProcessorTrigger(ADC0_BASE, 3);
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }
    ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);

    //xTaskNotify(Task3Handle, LED_TOGGLE, eSetBits);
    if(*ADC0Value > 1000)
    {
        UARTprintf("CO detected");
    }
}

void TimerCallback2(TimerHandle_t xTimer2)
{
    i2c_init();

    rh = i2cRead(RH_ADDR);
    tp = i2cRead(TEMP_ADDR);
    humidity_val = humidity(rh);
    temp_val = temp(tp);

    UARTprintf("RH= %d, Tp = %d\n", humidity_val, temp_val);
}

// Flash the LEDs on the launchpad
void LED1Task(void *pvParameters)
{
    TimerHandle_t xTimer1 = NULL;
    xTimer1 = xTimerCreate("MyTimer1", pdMS_TO_TICKS(500), pdTRUE, (void *)pvTimerGetTimerID(xTimer1), TimerCallback1);
    xTimerStart(xTimer1, 500);
    while(1);
}


// Write text over the Stellaris debug interface UART port
void LED2Task(void *pvParameters)
{
    TimerHandle_t xTimer2 = NULL;
    xTimer2 = xTimerCreate("MyTimer2", pdMS_TO_TICKS(500), pdTRUE, (void *)pvTimerGetTimerID(xTimer2), TimerCallback2);
    xTimerStart(xTimer2, 250);
    while(1);
}

void Task3(void *pvParameters)
{
    BaseType_t returned_notification;
    int returned_val;
    notifying_data received_data;
    while(1)
    {
        returned_notification = xTaskNotifyWait(0, 0xFF, &returned_val, portMAX_DELAY);
        if(returned_notification == pdTRUE)
        {
            if(returned_val & LED_TOGGLE)
            {
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, a);
                a ^= GPIO_PIN_0;
                UARTprintf("Task1 notified \n");
            }
            if(returned_val & LOG_STRING)
            {
                xQueueReceive(queue_handle, &received_data, 500);
                UARTprintf("Message received: %s, Current ticks: %d \n", received_data.message, received_data.current_ticks);
            }
        }
    }
}

/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
