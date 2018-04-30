/*reference: https://github.com/akobyl/TM4C129_FreeRTOS_Demo
 *
 * main.c in which we create different tasks for sensors, logger and alert task.
 * */

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
#include "startup.h"
#include "unit_test.h"
//#include "logger.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#define BAUD_RATE 115200
#define SysClock 120000000

// Demo Task declarations
void Gas_Task(void *pvParameters);
void Flame_Task(void *pvParameters);
void Humidity_Task(void *pvParameters);
void Temp_Task(void *pvParameters);
void Log_Task(void *pvParameters);
void Alert_Task(void *pvParameters);
void UART_read(void *pvParameters);

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
TaskHandle_t UARTTaskHandle;

xQueueHandle queue_handle;

SemaphoreHandle_t my_sem;
SemaphoreHandle_t sem_uart;

QueueHandle_t myQueue;
volatile char buff1[400] = {0};
volatile char buff2[10] = {0};
volatile char *ptr;
volatile char *ptr2;
volatile char abuff[400] = {0};
volatile char *aptr;

typedef struct{
    float data;
    int TaskID;
    int alert;
    //char * string_msg
}message;

typedef enum
{
    Gas_task = 1,
    Flame_task = 2,
    Temperature_task = 3,
    Humidity_task = 4,
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

volatile message msg_struct;

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


void ssi_init()
{
    // reference example code spi_master.c

    int NUM_SSI_DATA =3;

        uint32_t pui32DataTx[3];
        uint32_t pui32DataRx[3];
        uint32_t ui32Index;



        // The SSI0 peripheral must be enabled for use.
        SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);


        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);


        GPIOPinConfigure(GPIO_PA2_SSI0CLK);
        GPIOPinConfigure(GPIO_PA3_SSI0FSS);
        GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
        GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

        //
        // Configure the GPIO settings for the SSI pins.  This function also gives
        // control of these pins to the SSI hardware.  Consult the data sheet to
        // see which functions are allocated per pin.
        // The pins are assigned as follows:
        //      PA5 - SSI0Tx
        //      PA4 - SSI0Rx
        //      PA3 - SSI0Fss
        //      PA2 - SSI0CLK
        // TODO: change this to select the port/pin you are using.
        //
        GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                       GPIO_PIN_2);


        // Configure and enable the SSI port for SPI master mode.
        SSIConfigSetExpClk(SSI3_BASE, g_ui32SysClock, SSI_FRF_MOTO_MODE_0,
                           SSI_MODE_MASTER, 1000000, 8);

        SSIAdvModeSet(SSI3_BASE, SSI_ADV_MODE_READ_WRITE);
        SSIAdvFrameHoldEnable(SSI3_BASE);

        // Enable the SSI0 module.
        SSIEnable(SSI3_BASE);

        while(SSIDataGetNonBlocking(SSI3_BASE, &pui32DataRx[0]))
        {
        }

        //
        // Initialize the data to send.
        //
        pui32DataTx[0] = 's';
        pui32DataTx[1] = 'p';
        pui32DataTx[2] = 'i';


        for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
        {
            SSIDataPut(SSI3_BASE, pui32DataTx[ui32Index]);
        }


        // Wait until SSI0 is done transferring all the data in the transmit FIFO.
        while(SSIBusy(SSI3_BASE))
        {
        }


        // Receive 3 bytes of data.
        for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
        {
            SSIDataGet(SSI3_BASE, &pui32DataRx[ui32Index]);
        }
}

// Main function
int main(void)
{
        g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                    SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                    SYSCTL_CFG_VCO_480), SysClock);

        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    //Enable GPIO
       // ssi_init();

        if (startup() == -1)
            exit(-1);

        PinoutSet(false, false);

        ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

        int testGas, testFlame, testTemp, alertGas, alertFlame, alertTemp, alertGasDis, alertFlameDis;

        testGas = test_gas();
        testFlame = test_flame();
        testTemp = test_temp();
        alertGas = alert_gas();
        alertTemp = alert_temp();
        alertFlame = alert_flame();
        alertGasDis = alert_gas_disconnected();
        alertFlameDis = alert_flame_disconnected();


        // Create tasks

        xTaskCreate(Gas_Task_timer, (const portCHAR *)"Gas task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task1Handle);

        xTaskCreate(Flame_Task_timer, (const portCHAR *)"Flame task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task2Handle);

        /*xTaskCreate(Humidity_Task_timer, (const portCHAR *)"Humidity Task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task3Handle);*/

        xTaskCreate(Temp_Task_timer, (const portCHAR *)"Temperature Task",
                configMINIMAL_STACK_SIZE, NULL, 1, &Task4Handle);

        xTaskCreate(Log_Task_timer, (const portCHAR *)"Logger task",
                        configMINIMAL_STACK_SIZE, NULL, 1, &LogTaskHandle);

        xTaskCreate(Alert_Task, (const portCHAR *)"Alert task",
                                        configMINIMAL_STACK_SIZE, NULL, 1, &AlertTaskHandle);

        /*xTaskCreate(UART_read, (const portCHAR *)"UART read task",
                                                configMINIMAL_STACK_SIZE, NULL, 1, &UARTTaskHandle);*/


        vTaskStartScheduler();

        while(1);

        return 0;
}

/**
 * Gas_Task_timer, Flame_Task_timer, Humidity_Task_timer, Temp_Task_timer and Log_Task_timer triggers the timer after every 1 second
 * and calls the Gas_Task, Flame_Task, Humidity_Task and Temp_Task respectively.
 */

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
    TimerHandle_t xTimer3 = NULL;
    xTimer3 = xTimerCreate("MyTimer4", pdMS_TO_TICKS(1000), pdTRUE, (void *)pvTimerGetTimerID(xTimer3), Temp_Task);
    xTimerStart(xTimer3, portMAX_DELAY);
    while(1);
}

void Log_Task_timer(void *pvParameters)
{
    TimerHandle_t xTimer5 = NULL;
    xTimer5 = xTimerCreate("MyTimer5", pdMS_TO_TICKS(1000), pdTRUE, (void *)pvTimerGetTimerID(xTimer5), Log_Task);
    xTimerStart(xTimer5, portMAX_DELAY);
    while(1);
}

/* Function: Gas_Task
 * Description: Reads CO gas value, stores it in a struct and sends it to the Log_Task via IPC message queue.
 */
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
        msg_struct.TaskID = Gas_task;
        msg_struct.alert = 0;

        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);

        if (f>9)
                {

                    xTaskNotify( AlertTaskHandle, Co_alert, eSetBits);
                }
        if (f<3.3)
                {

                     xTaskNotify( AlertTaskHandle, Sensor_disconnected, eSetBits);
                }

    }

    xSemaphoreGive(my_sem);

}

/* Function: Flame_Task
 * Description: Reads Flame sensor value, stores it in a struct and sends it to the Log_Task via IPC message queue.
 */
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
                msg_struct.TaskID = Flame_task;
                msg_struct.alert = 0;


        if (200 <= ADC1Value[0] && ADC1Value[0] <= 500)
        {

                xTaskNotify( AlertTaskHandle, Flame_alert, eSetBits);
        }
        if (ADC1Value[0] < 150)
                        {

                             xTaskNotify( AlertTaskHandle, Sensor_disconnected, eSetBits);
                        }
        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);
    }

            xSemaphoreGive(my_sem);
    }

/* Function: Humidity_Task
 * Description: Reads Humidity value, stores it in a struct and sends it to the Log_Task via IPC message queue.
 */
void Humidity_Task(TimerHandle_t xTimer3)
{
    if(xSemaphoreTake(my_sem, portMAX_DELAY))
        {
        rh = i2cRead(RH_ADDR);
        humidity_val = humidity(rh);

        msg_struct.data = humidity_val;
        msg_struct.TaskID = Humidity_task;

        msg_struct.alert = 1;
        //strcpy(msg_struct.a, "humidity");

        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);

        if (humidity_val < 20)

                {
                        //xTaskNotify( AlertTaskHandle, Flame_alert, eSetBits);
                }

        }

        xSemaphoreGive(my_sem);

}


/* Function: Temp_Task
 * Description: Reads temperature value, stores it in a struct and sends it to the Log_Task via IPC message queue.
 */
void Temp_Task(TimerHandle_t xTimer4)
{
    if(xSemaphoreTake(my_sem, portMAX_DELAY))
    {
        tp = i2cRead(TEMP_ADDR);
        temp_val = temp(tp);

        msg_struct.data = temp_val;
        msg_struct.TaskID = Temperature_task;
        msg_struct.alert = 0;

        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);
        if (temp_val > 35)
                {

                        xTaskNotify( AlertTaskHandle, Flame_alert, eSetBits);
                }

     }

    xSemaphoreGive(my_sem);

    //UARTprintf("Tp = %d\n",temp_val);
}


/* Function: Alert_Task
 * Description: Receives notification from Gas_Task, Flame_Task, Humidity_Task and Temp_Task and sends the Alert message to BBG through UART
 */
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
                        msg_struct.TaskID = Gas_task;
                        msg_struct.alert = 1;
                        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);
                     }
                    xSemaphoreGive(my_sem);

                }
                if (NotifValue & Flame_alert)
                {
                    if(xSemaphoreTake(my_sem, 250))
                    {
                        msg_struct.TaskID = Flame_task;
                        msg_struct.alert = 1;
                        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);

                     }
                     xSemaphoreGive(my_sem);
                }
                if (NotifValue & Humidity_alert)
                {
                    if(xSemaphoreTake(my_sem, 250))
                    {
                        msg_struct.TaskID = Humidity_task;
                        msg_struct.alert = 1;
                        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);
                    }
                    xSemaphoreGive(my_sem);
                }
                if (NotifValue & Temp_alert)
                {
                    if(xSemaphoreTake(my_sem, 250))
                    {
                        msg_struct.TaskID = Temperature_task;
                        msg_struct.alert = 1;
                        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);
                     }
                     xSemaphoreGive(my_sem);
                }
                if (NotifValue & Sensor_disconnected)
                                {
                                    if(xSemaphoreTake(my_sem, 250))
                                    {

                                        msg_struct.alert = 2;
                                        xQueueSendToBack(myQueue, &msg_struct, portMAX_DELAY);
                                     }
                                     xSemaphoreGive(my_sem);
                                }
            }
            }
}

// Queue initialization

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

/*void UART_read(void *pvParameters)
{
    while(1)
    {
        while(UARTCharsAvail(UART2_BASE))
        {
               perror("Done");
        }
    }
}*/

/* Function: Log_task
 * Description: Receives data struct via IPC message queue from Gas_task, Flame_Task, Humidity_Task and Temp_Task and sends it to BBG via UART
 */
void Log_Task(void *pvParameters)
{
    sprintf(buff2, "%s", "\n");
    ptr2 = &buff2;

    while(uxQueueSpacesAvailable(myQueue) != myQueueLength)
    {
            if(xSemaphoreTake(my_sem, 250))
            {
                 xQueueReceive(myQueue, &msg_struct, portMAX_DELAY);
            }

        //sprintf(buff1, "Data:%f, Length:%i, TaskId:%i, LogLevel:%i\n\0", msg_struct.data, msg_struct.data_len, msg_struct.TaskID, msg_struct.LogLevel);
        //ptr = &buff1;
        ptr = (uint8_t *)&msg_struct;

        UART_send(ptr, sizeof(message));
        //UART_send(ptr, sizeof(msg_struct));

        /*if(msg_struct.TaskID == Temperature_task)
            UART_send(ptr2, strlen(buff2));*/
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
