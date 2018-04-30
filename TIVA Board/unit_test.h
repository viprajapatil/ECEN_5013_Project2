
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "si7021.h"
#include "gas_flame.h"
#include "uart.h"
#include "semphr.h"

typedef enum{
    FAIL,
    SUCCESS,

}result;


/* @brief  Test gas sensor
 * This function will test Gas sensor values
 * @param  void
 * @return SUCCESS
 *         FAIL
 * */
int test_gas();

/* @brief  Test flame sensor
 * This function will test flame sensor values
 * @param  void
 * @return SUCCESS
 *         FAIL
 * */
int test_flame();

/* @brief  Test Si7021 sensor
 * This function will test Si7021 sensor values
 * @param  void
 * @return SUCCESS
 *         FAIL
 * */
int test_temp();

/* @brief  ALert gas sensor
 * This function will test the alert notification for gas sensor
 * @param  void
 * @return SUCCESS
 *         FAIL
 * */
int alert_gas();

/* @brief  ALert Si7021 sensor
 * This function will test the alert notification for Si7021 sensor
 * @param  void
 * @return SUCCESS
 *         FAIL
 * */
int alert_temp();

/* @brief  ALert Flame sensor
 * This function will test the alert notification for flame sensor
 * @param  void
 * @return SUCCESS
 *         FAIL
 * */
int alert_flame();

/* @brief  Gas sensor disconnected
 * This function will test if the notification is send when the gas sensor is disconnected
 * @param  void
 * @return SUCCESS
 *         FAIL
 * */
int alert_gas_disconnected();

/* @brief  Flame sensor disconnected
 * This function will test if the notification is send when the flame sensor is disconnected
 * @param  void
 * @return SUCCESS
 *         FAIL
 * */
int alert_flame_disconnected();
