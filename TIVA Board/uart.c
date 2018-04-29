#include "main.h"
#include "uart.h"
#include "si7021.h"
#include "gas_flame.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

int ConfigureUART0(void)
{
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    //Enable GPIO

        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);    //Enable UART0

        ROM_GPIOPinConfigure(GPIO_PA0_U0RX);                //Configure UART pins
        ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
        ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

        //UARTStdioConfig(2, BAUD_RATE, g_ui32SysClock);         //Initialize UART
        ROM_UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 115200,
                                    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                                UART_CONFIG_PAR_NONE));
        return 0;

}

int ConfigureUART2(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    //Enable GPIO

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);    //Enable UART0

    ROM_GPIOPinConfigure(GPIO_PA6_U2RX);                //Configure UART pins
    ROM_GPIOPinConfigure(GPIO_PA7_U2TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    ROM_UARTConfigSetExpClk(UART2_BASE, g_ui32SysClock, 115200,
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                            UART_CONFIG_PAR_NONE));

    //UARTFIFOEnable(UART2_BASE)0

    IntMasterEnable();

//    ROM_IntEnable(UART2_BASE);

    IntEnable(INT_UART2);
    ROM_UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_RT);
    return 0;
}

void UARTIntHandler()
{
    char c;
    uint32_t status = ROM_UARTIntStatus(UART2_BASE, true);
    ROM_UARTIntClear(UART2_BASE, status);
    while(UARTCharsAvail(UART2_BASE))
    {
        c = ROM_UARTCharGet(UART2_BASE);
        //UARTprintf("%c", c);
    }

}

