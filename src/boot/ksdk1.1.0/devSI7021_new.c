#include <stdlib.h>

#include "fsl_i2c_master_driver.h"
#include "fsl_clock_manager.h"

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"
#include "devSI7021_new.h"


void
read_SI7021(void)
{
	i2c_status_t	status1, status2, status3, status4;
	uint8_t humidity_data[2];
	uint8_t temperature_data[2];
	uint8_t cmd_read_humidity[2] = {0xE5, 0x00};
	uint8_t cmd_read_temperature[2] = {0xE3, 0x00};

	i2c_device_t devSI7021 =
	{
		.address = 0x40,
		.baudRate_kbps = gWarpI2cBaudRateKbps
	};

	// HUMIDITY
	status1 = I2C_DRV_MasterSendDataBlocking(
							0 /* I2C instance */,
							&devSI7021,
							NULL,
							0,
							(uint8_t *)cmd_read_humidity,
							2,
							gWarpI2cTimeoutMilliseconds);

	OSA_TimeDelay(15);

	status2 = I2C_DRV_MasterReceiveDataBlocking(
							0 /* I2C peripheral instance */,
							&devSI7021,
							NULL,
							0,
							(uint8_t *)humidity_data,
							2,
							gWarpI2cTimeoutMilliseconds);

	uint16_t hum_MSB = humidity_data[0];
	uint16_t hum_LSB = humidity_data[1];
	uint16_t humidity_reading = ((hum_MSB & 0xFF) << 8) | (hum_LSB);
	humidity_reading = 125 * humidity_reading / 65536 - 6;

	if (	(status1 == kStatus_I2C_Success) || (status2 == kStatus_I2C_Success) )
	{
		SEGGER_RTT_WriteString(0, " SI7021 humidity read succeeded\n");
		SEGGER_RTT_printf(0, " read: 0x%02x 0x%02x,", humidity_data[0], humidity_data[1]);
		SEGGER_RTT_printf(0, " %d\n", humidity_reading);
		OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
	}

	// TEMPERATURE

    status3 = I2C_DRV_MasterSendDataBlocking(
                            0 /* I2C instance */,
                            &devSI7021,
                            NULL,
                            0,
                            (uint8_t *)cmd_read_temperature,
                            2,
                            gWarpI2cTimeoutMilliseconds);

	OSA_TimeDelay(15);

    status4 = I2C_DRV_MasterReceiveDataBlocking(
                            0 /* I2C peripheral instance */,
                            &devSI7021,
                            NULL,
                            0,
                            (uint8_t *)temperature_data,
                            2,
                            gWarpI2cTimeoutMilliseconds);

    uint16_t temp_MSB = temperature_data[0];
    uint16_t temp_LSB = temperature_data[1];
    uint16_t temperature_reading = ((temp_MSB & 0xFF) << 8) | (temp_LSB);
    temperature_reading = 175 * temperature_reading / 65536 - 46;

	if (	(status3 == kStatus_I2C_Success) ||	(status4 == kStatus_I2C_Success))
	{
        SEGGER_RTT_WriteString(0, " SI7021 temperature read succeeded\n");
        SEGGER_RTT_printf(0, " read: 0x%02x 0x%02x,", temperature_data[0], temperature_data[1]);
        SEGGER_RTT_printf(0, " %d\n\n", temperature_reading);
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
	}

}
