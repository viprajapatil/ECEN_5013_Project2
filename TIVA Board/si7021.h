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


volatile uint32_t rh, tp;
double humidity_val, temp_val;
uint32_t g_ui32SysClock;
static uint16_t r;

TaskHandle_t AlertTaskHandle;



#define SLAVE_ADDRESS 0x40
#define RH_ADDR 0xE5
#define TEMP_ADDR 0xE3

/* @brief  I2C initialization function
 * This function will initialize i2c
 * @param  void
 * @return 0: on success
 *        -1: on error
 * */
int i2c_init();

/* @brief  I2C read
 * This function will read values from Si7021 sensor
 * @param  RegAddr: address of the register from whcih values will be read
 * @return uint32_t: value read
 * */
uint32_t i2cRead(int RegAddr);

/* @brief  Conversion function(%RH)
 * This function will convert the value read from the registers to humidity in percentage
 * @param  rh: value read from registers
 * @return double: converted value
 * */
double humidity(uint32_t rh);

/* @brief  Conversion function(Temperature)
 * This function will convert the value read from the registers to Temperature in Celsius
 * @param  rh: value read from registers
 * @return double: converted value
 * */
double temp(uint32_t rh);
