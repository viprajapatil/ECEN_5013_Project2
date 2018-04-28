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
#define myQueueLength 500

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
//#include "logger.h"

#define BAUD_RATE 115200
#define SysClock 120000000

// Demo Task declarations
void Gas_Task(void *pvParameters);
void Flame_Task(void *pvParameters);
void Humidity_Task(void *pvParameters);
void Temp_Task(void *pvParameters);
void Log_Task(void *pvParameters);
void Alert_Task(void *pvParameters);

// Timer task declarations
void Log_Task_timer(TimerHandle_t xTimer5);
void Gas_Task_timer(TimerHandle_t xTimer1);
void Flame_Task_timer(TimerHandle_t xTimer2);
void Temp_Task_timer(void *pvParameters);
void Humidity_Task_timer(void *pvParameters);
void Humidity_Task(TimerHandle_t xTimer3);
void Temp_Task(TimerHandle_t xTimer4);

void Queue_init();


volatile uint32_t ADC0Value[1];
volatile uint32_t ADC1Value[1];
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
TaskHandle_t AlertTaskHandle;

xQueueHandle queue_handle;

SemaphoreHandle_t my_sem;
SemaphoreHandle_t sem_uart;

QueueHandle_t myQueue;
volatile char buff1[400] = {0};
volatile char *ptr;
volatile char abuff[400] = {0};
volatile char *aptr;

typedef struct{
    float data;
    int data_len;
    int TaskID;
    int LogLevel;
    int alert;
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

typedef enum
{
    Co_alert = 1,
    Flame_alert = 2,
    Humidity_alert = 4,
    Temp_alert = 8,
    Sensor_disconnected = 16,
    }alert;

message msg_struct;

void UART_send(char* ptr, int len)
{

    if(xSemaphoreTake(sem_uart, portMAX_DELAY))
        {
               while(len != 0)
               {
                   UARTCharPut(UART2_BASE, *ptr);
                   ptr++;
                   len--;
               }
        }

        xSemaphoreGive(sem_uart);
    }

// Main function
int main(void)
{
        g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                    SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                    SYSCTL_CFG_VCO_480), SysClock);

        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    //Enable GPIO


        ConfigureUART2();        //Initialize UART

        Queue_init();

        adc_ch0_init(); // ADC for gas sensor
        adc_ch1_init(); // ADC for flame sensor

        PinoutSet(false, false);

        ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

        GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
        GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

        i2c_init();

        // Create tasks

        xTaskCreate(Gas_Task_timer, (const portCHAR *)"Gas task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task1Handle);

        xTaskCreate(Flame_Task_timer, (const portCHAR *)"Flame task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task2Handle);

        xTaskCreate(Humidity_Task_timer, (const portCHAR *)"Humidity Task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task3Handle);

        xTaskCreate(Temp_Task_timer, (const portCHAR *)"Temperature Task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task4Handle);

        xTaskCreate(Log_Task_timer, (const portCHAR *)"Logger task",
                        configMINIMAL_STACK_SIZE, NULL, 1, &LogTaskHandle);

        xTaskCreate(Alert_Task, (const portCHAR *)"Alert task",
                                        configMINIMAL_STACK_SIZE, NULL, 1, &AlertTaskHandle);

        vTaskStartScheduler();

        while(1);

        return 0;
}

void Gas_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer1 = NULL;
    xTimer1 = xTimerCreate("MyTimer1", pdMS_TO_TICKS(1000), pdTRUE, (void *)pvTimerGetTimerID(xTimer1), Gas_Task);
    xTimerStart(xTimer1, portMAX_DELAY);
    while(1);
}


void Flame_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer2 = NULL;
    xTimer2 = xTimerCreate("MyTimer2", pdMS_TO_TICKS(1000), pdTRUE, (void *)pvTimerGetTimerID(xTimer2), Flame_Task);
    xTimerStart(xTimer2, portMAX_DELAY);
    while(1);
}

void Humidity_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer3 = NULL;
    xTimer3 = xTimerCreate("MyTimer3", pdMS_TO_TICKS(1000), pdTRUE, (void *)pvTimerGetTimerID(xTimer3), Humidity_Task);
    xTimerStart(xTimer3, portMAX_DELAY);
    while(1);
}

void Temp_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer4 = NULL;
    xTimer4 = xTimerCreate("MyTimer4", pdMS_TO_TICKS(1000), pdTRUE, (void *)pvTimerGetTimerID(xTimer4), Temp_Task);
    xTimerStart(xTimer4, portMAX_DELAY);
    while(1);
}

void Log_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer5 = NULL;
    xTimer5 = xTimerCreate("MyTimer5", pdMS_TO_TICKS(1000), pdTRUE, (void *)pvTimerGetTimerID(xTimer5), Log_Task);
    xTimerStart(xTimer5, portMAX_DELAY);
    while(1);
}


void Gas_Task(TimerHandle_t xTimer1)
{
    if(xSemaphoreTake(my_sem, portMAX_DELAY))
        {
    ADCProcessorTrigger(ADC0_BASE, 3);
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }
    ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);


        float f = co_val(ADC0Value[0]);

        msg_struct.data = f;
        msg_struct.data_len = sizeof(f);
        msg_struct.TaskID = Gas_task;
        msg_struct.LogLevel = DATA;
        msg_struct.alert = 0;
        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);

        if (f>9)
                {
                    xTaskNotify( AlertTaskHandle, Co_alert, eSetBits);
                            //UARTprintf("CO gas exceeds the threshold!\n");
                }

    }

    xSemaphoreGive(my_sem);

}

void Flame_Task(TimerHandle_t xTimer2)
{
    if(xSemaphoreTake(my_sem, portMAX_DELAY))
    {
        ADCProcessorTrigger(ADC1_BASE, 3);
        while(!ADCIntStatus(ADC1_BASE, 3, false))
        {
        }
        ADCIntClear(ADC1_BASE, 3);
        ADCSequenceDataGet(ADC1_BASE, 3, ADC1Value);


                msg_struct.data = ADC1Value[0];
                msg_struct.data_len = sizeof(ADC1Value[0]);
                msg_struct.TaskID = Flame_task;
                msg_struct.LogLevel = DATA;
                msg_struct.alert = 0;

        if (ADC1Value[0] < 350)
        {
                xTaskNotify( AlertTaskHandle, Flame_alert, eSetBits);
                //UARTprintf("CO gas exceeds the threshold!\n");
        }
        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);
    }

            xSemaphoreGive(my_sem);
    }

void Humidity_Task(TimerHandle_t xTimer3)
{
    if(xSemaphoreTake(my_sem, portMAX_DELAY))
        {
        rh = i2cRead(RH_ADDR);
        humidity_val = humidity(rh);

        msg_struct.data = humidity_val;
        msg_struct.data_len = sizeof(humidity_val);
        msg_struct.TaskID = Humidity_task;
        msg_struct.LogLevel = DATA;
        msg_struct.alert = 0;

        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);

        if (humidity_val < 20)

                {
                        xTaskNotify( AlertTaskHandle, Flame_alert, eSetBits);
                        //UARTprintf("CO gas exceeds the threshold!\n");
                }

        }

        xSemaphoreGive(my_sem);

   // UARTprintf("RH= %d\n", humidity_val);
}

void Temp_Task(TimerHandle_t xTimer4)
{
    if(xSemaphoreTake(my_sem, portMAX_DELAY))
    {
        tp = i2cRead(TEMP_ADDR);
        temp_val = temp(tp);

        msg_struct.data = temp_val;
        msg_struct.data_len = sizeof(temp_val);
        msg_struct.TaskID = Temperature_task;
        msg_struct.LogLevel = DATA;
        msg_struct.alert = 0;

        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);
        if (temp_val > 35)
                {
                        xTaskNotify( AlertTaskHandle, Flame_alert, eSetBits);
                        //UARTprintf("CO gas exceeds the threshold!\n");
                }

     }

    xSemaphoreGive(my_sem);

    //UARTprintf("Tp = %d\n",temp_val);
}

void Alert_Task(void *pvParameters)
{BaseType_t ret;
    int NotifValue = 0;
            while(1){
                ret = xTaskNotifyWait( 0, 0xFF, &NotifValue, portMAX_DELAY);
            // Notify wait
            if(ret == pdTRUE)
            {
                if (NotifValue & Co_alert)
                {
                    if(xSemaphoreTake(my_sem, 250))
                     {
                        sprintf(abuff, "AlERT! CO exceeds its threshold.\n\0");
                        aptr = &abuff;
                        UART_send(aptr, strlen(abuff));
                     }
                    xSemaphoreGive(my_sem);

                }
                if (NotifValue & Flame_alert)
                {
                    if(xSemaphoreTake(my_sem, 250))
                    {
                        sprintf(abuff, "AlERT! Flame sensor exceeds its threshold.\n\0");
                        aptr = &abuff;
                        UART_send(aptr, strlen(abuff));
                     }
                     xSemaphoreGive(my_sem);
                }
                if (NotifValue & Humidity_alert)
                {
                    if(xSemaphoreTake(my_sem, 250))
                    {
                        sprintf(abuff, "AlERT! %HUmidity exceeds its threshold.\n\0");
                        aptr = &abuff;
                        UART_send(aptr, strlen(abuff));
                    }
                    xSemaphoreGive(my_sem);
                }
                if (NotifValue & Temp_alert)
                {
                    if(xSemaphoreTake(my_sem, 250))
                    {
                        sprintf(abuff, "AlERT! Temperature exceeds its threshold.\n\0");
                        aptr = &abuff;
                        UART_send(aptr, strlen(abuff));
                     }
                     xSemaphoreGive(my_sem);
                }
            }
            }
}


void Queue_init()
{
    myQueue = xQueueCreate(myQueueLength, sizeof(message));
    if(myQueue == NULL)
    {
        perror("Queue not created");
    }

    my_sem = xSemaphoreCreateMutex();
    sem_uart = xSemaphoreCreateMutex();
}

void Log_Task(void *pvParameters)
{
    while(uxQueueSpacesAvailable(myQueue) != myQueueLength)
    {
            if(xSemaphoreTake(my_sem, 250))
            {
                 xQueueReceive(myQueue, &msg_struct, portMAX_DELAY);
            }

        sprintf(buff1, "Data:%f, Length:%i, TaskId:%i, LogLevel:%i\n\0", msg_struct.data, msg_struct.data_len, msg_struct.TaskID, msg_struct.LogLevel);
        ptr = &buff1;

        UART_send(ptr, strlen(buff1));
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
