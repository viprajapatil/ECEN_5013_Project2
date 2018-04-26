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
#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"

#define LED_TOGGLE 0x00000001
#define LOG_STRING 0x00000002
#define myQueueLength 10

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
#include "semphr.h"

#include "si7021.h"
#include "uart.h"
#include "gas_flame.h"
#include "logger.h"

#define BAUD_RATE 115200
#define SysClock 120000000

// Demo Task declarations
void Gas_Task(void *pvParameters);
void Flame_Task(void *pvParameters);
void Task3(void *pvParameters);
void loggerFuncTimer(TimerHandle_t xTimer3);
void Gas_Task_timer(TimerHandle_t xTimer1);
void Flame_Task_timer(TimerHandle_t xTimer2);
void Queue_init();
void loggerFunc(void *pvParameters);

uint32_t ADC0Value[1];
uint16_t samplePeriod;
uint32_t sequence;
volatile uint32_t rh, tp;
double humidity_val, temp_val;

#define SLAVE_ADDRESS 0x40
#define RH_ADDR 0xE5
#define TEMP_ADDR 0xE3


TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;
TaskHandle_t Task4Handle;
TaskHandle_t LogTaskHandle;
xQueueHandle queue_handle;
SemaphoreHandle_t my_sem;

QueueHandle_t myQueue;

typedef struct{
    float data;
    int data_len;
    int TaskID;
    int LogLevel;
}message;

typedef enum
{
    Gas_task = 0,
    Flame_task = 1,
    Humidity_task = 2,
    Temperature_task = 3,
}task_id;

typedef enum
{
    DATA,
    ERROR,
}log_level;

message msg_struct;


// Main function
int main(void)
{
        g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                    SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                    SYSCTL_CFG_VCO_480), SysClock);

        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    //Enable GPIO

        ConfigureUART2();        //Initialize UART

        Queue_init();

        //adc_init();
        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

            while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

            GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_2);
            ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

            ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH1|ADC_CTL_IE|ADC_CTL_END);
            //MAP_ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);

            //ROM_ADCHardwareOversampleConfigure(ADC0_BASE,0);
            ADCSequenceEnable(ADC0_BASE, 3);

        PinoutSet(false, false);

        ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

        GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
        GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

        i2c_init();
        int s = 7;
        char a[7] = "Vipraja";

        char buff[16];

//        sprintf(buff, "Gas value: %d", s);
//        char * aptr = &buff;
//        while(aptr != '\0')
//        {
//        UARTCharPutNonBlocking(UART2_BASE, *aptr);
//        aptr++;
//        }
        // Create tasks

        xTaskCreate(Gas_Task_timer, (const portCHAR *)"Gas task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task1Handle);

        xTaskCreate(Flame_Task_timer, (const portCHAR *)"Flame task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task2Handle);

        /*xTaskCreate(Humidity_Task_timer, (const portCHAR *)"Humidity Task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task3Handle);

        xTaskCreate(Temp_Task_timer, (const portCHAR *)"Temperature Task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task4Handle);*/

        //SysCtlDelay(pdMS_TO_TICKS(300));

        xTaskCreate(loggerFuncTimer, (const portCHAR *)"Logger task",
                        configMINIMAL_STACK_SIZE, NULL, 1, &LogTaskHandle);

        vTaskStartScheduler();

        while(1);

        return 0;
}

void Gas_Task(TimerHandle_t xTimer1)
{
    //ADCIntClear(ADC0_BASE, 3);
    ADCProcessorTrigger(ADC0_BASE, 3);
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }
    ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);

//    char buff[16];
//    char * b = &buff;
//    char bu = 'P';
//    snprintf(buff, "Gas value: %d", ADC0Value[0]);
//    while(*b != '\0')
//    {
//    UARTCharPutNonBlocking(UART2_BASE, b);
//    b++;
//    }
    /*char buff[16];
    sprintf(buff, "Gas value: %d", ADC0Value[0]);
    char * aptr = &buff;
    int n = 0;

    while(aptr != '\0')
    {
           UARTCharPutNonBlocking(UART2_BASE, *aptr);
           aptr++;
    }*/
    if(xSemaphoreTake(my_sem, portMAX_DELAY))
    {
        float f = co_val(ADC0Value[0]);

        msg_struct.data = f;
        msg_struct.data_len = sizeof(f);
        msg_struct.TaskID = Gas_task;
        msg_struct.LogLevel = DATA;

        xQueueSendToFront(myQueue, &msg_struct, portMAX_DELAY);
    }

    xSemaphoreGive(my_sem);

    /*char buff[sizeof(msg_struct)];
    char * ptr;
    ptr = &buff;
    ////What to send?///
    snprintf(buff, sizeof(msg_struct)), "%f", f);
    while (*ptr != '/0' )
    {
        UARTCharPutNonBlocking(UART2_BASE, *ptr);
        ptr++;
    }*/

    /*if(*ADC0Value > 1000)
    {
        UARTprintf("CO detected");
    }*/
}

void Flame_Task(TimerHandle_t xTimer2)
{
    ADCProcessorTrigger(ADC0_BASE, 3);
        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
        }
        ADCIntClear(ADC0_BASE, 3);
        ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);

    }

void Humidity_Task(TimerHandle_t xTimer3)
{
    rh = i2cRead(RH_ADDR);
    humidity_val = humidity(rh);

    msg_struct.data = humidity_val;
    msg_struct.data_len = sizeof(humidity_val);
    msg_struct.TaskID = Humidity_task;

    UARTprintf("RH= %d\n", humidity_val);
}

void Temp_Task(TimerHandle_t xTimer4)
{
    tp = i2cRead(TEMP_ADDR);
    temp_val = temp(tp);

    msg_struct.data = temp_val;
    msg_struct.data_len = sizeof(temp_val);
    msg_struct.TaskID = Temperature_task;

    UARTprintf("Tp = %d\n",temp_val);
}

void loggerFuncTimer(void *pvParameters)
{
    TimerHandle_t xTimer3 = NULL;
    xTimer3 = xTimerCreate("MyTimer1", pdMS_TO_TICKS(600), pdTRUE, (void *)pvTimerGetTimerID(xTimer3), loggerFunc);
    xTimerStart(xTimer3, 600);
    while(1);
}

void Gas_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer1 = NULL;
    xTimer1 = xTimerCreate("MyTimer1", pdMS_TO_TICKS(500), pdTRUE, (void *)pvTimerGetTimerID(xTimer1), Gas_Task);
    xTimerStart(xTimer1, 500);
    while(1);
}


void Flame_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer2 = NULL;
    xTimer2 = xTimerCreate("MyTimer2", pdMS_TO_TICKS(500), pdTRUE, (void *)pvTimerGetTimerID(xTimer2), Flame_Task);
    xTimerStart(xTimer2, 250);
    while(1);
}

void Humidity_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer3 = NULL;
    xTimer3 = xTimerCreate("MyTimer3", pdMS_TO_TICKS(500), pdTRUE, (void *)pvTimerGetTimerID(xTimer3), Humidity_Task);
    xTimerStart(xTimer3, 250);
    while(1);
}

void Temp_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer4 = NULL;
    xTimer4 = xTimerCreate("MyTimer4", pdMS_TO_TICKS(500), pdTRUE, (void *)pvTimerGetTimerID(xTimer4), Temp_Task);
    xTimerStart(xTimer4, 250);
    while(1);
}

void Queue_init()
{
    myQueue = xQueueCreate(myQueueLength, sizeof(message));
    if(myQueue == NULL)
    {
        perror("Queue not created");
    }

    my_sem = xSemaphoreCreateMutex();
}

void loggerFunc(void *pvParameters)
{
    //while(1)
    {
        //while(uxQueueSpacesAvailable(myQueue) != 10)
        {
            if(xSemaphoreTake(my_sem, 250))
            {
                 xQueueReceive(myQueue, &msg_struct, portMAX_DELAY);
            }
        }

        xSemaphoreGive(my_sem);
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
