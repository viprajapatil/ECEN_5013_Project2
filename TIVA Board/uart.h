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




/* @brief  UART send
 * This function will send data to the BBG via UART
 * @param  ptr: pointer which points to the struct to be send
 *         struct_len: length of the struct
 * @return void
 * */
//void UART_send(char* ptr, int struct_len);


/* @brief  COnfigure UART0
 * This function will configure UART0
 * @param  void
 * @return 0: on success
 *        -1: on error
 * */
int ConfigureUART0(void);


/* @brief  COnfigure UART2
 * This function will configure UART2
 * @param  void
 * @return 0: on success
 *        -1: on error
 * */
int ConfigureUART2(void);

void UARTIntHandler();
