/* Si7021 Task file */

#include "si7021.h"

 i2c_init()
{
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
    int count = 0;
           I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);

           //specify register to be read
           I2CMasterDataPut(I2C0_BASE, RegAddr);

           //send control byte and register address byte to slave device
           I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

           //wait for MCU to finish transaction
           while(I2CMasterBusy(I2C0_BASE))
           {
           }
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

           uint32_t r = (return_val[0] *256) + return_val[1];
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
