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

/* @brief  ADC initialization
 * This function will initialize ADC ch0
 * @param  void
 * @return 0: on success
 *        -1: on error
 * */
int adc_ch0_init();

/* @brief  ADC initialization
 * This function will initialize ADC ch1
 * @param  void
 * @return 0: on success
 *        -1: on error
 * */
int adc_ch1_init();

/* @brief  Conversion (CO in ppm)
 * This function will convert the value from ADC to ppm
 * @param  val: value obtained from ADC
 * @return float: value of CO gas in ppm
 * */
float co_val(uint32_t val);
