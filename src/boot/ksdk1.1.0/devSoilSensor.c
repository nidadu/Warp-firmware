#include <stdlib.h>

#include "fsl_i2c_master_driver.h"
#include "fsl_clock_manager.h"
#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"
#include "devSoilSensor.h"


void read_moisture(void)
{
    uint8_t SOIL_data[2];

    i2c_device_t devSOIL =
    {
        .address = 0x36,
        .baudRate_kbps = gWarpI2cBaudRateKbps
    };


    /* Get SOIL reading */
    status_SOIL_w = I2C_DRV_MasterSendDataBlocking(
                            0 /* I2C instance */,
                            &devSOIL,
                            NULL,
                            0,
                            (uint8_t *)read_cmd,
                            2,
                            gWarpI2cTimeoutMilliseconds);

    if (status_SOIL_w != kStatus_I2C_Success)
    {
        SEGGER_RTT_WriteString(0, " SOIL sensor communication failed\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }
    else 
    {
        SEGGER_RTT_WriteString(0, " SOIL sensor writing succeeded\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }

    status_SOIL_r = I2C_DRV_MasterReceiveDataBlocking(
                            0 /* I2C peripheral instance */,
                            &devSOIL,
                            NULL,
                            0,
                            (uint8_t *)SOIL_data,
                            2,
                            gWarpI2cTimeoutMilliseconds);
    uint16_t soil_MSB = SOIL_data[0];
    uint16_t soil_LSB = SOIL_data[1];
    uint16_t soil_reading = ((soil_MSB & 0xFF) << 8) | (soil_LSB);

    if (status_SOIL_r != kStatus_I2C_Success)
    {
        SEGGER_RTT_WriteString(0, " SOIL sensor communication failed\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }
    else 
    {
        SEGGER_RTT_WriteString(0, " SOIL sensor read succeeded\n");
        SEGGER_RTT_printf(0, " read: 0x%02x 0x%02x,", SOIL_data[0], SOIL_data[1]);
        SEGGER_RTT_printf(0, " %d\n", soil_reading);
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
        
    }

}
