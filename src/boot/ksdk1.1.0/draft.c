enableI2Cpins(menuI2cPullupValue);

i2c_status_t	status_SOIL_w, status_SOIL_r;
uint8_t read_SOIL[2] = {0x0F, 0x10};
i2c_device_t devSOIL =
{
    .address = 0x36,
    .baudRate_kbps = gWarpI2cBaudRateKbps
};

i2c_status_t	status_SI7021_hum_w, status_SI7021_hum_r1, status_SI7021_hum_r2, \
                status_SI7021_temp_w, status_SI7021_temp_r1, status_SI7021_temp_r2;
uint8_t read_humidity[2] = {0xE5, 0x00};
uint8_t read_temperature[2] = {0xE3, 0x00};
i2c_device_t devSI7021 =
{
    .address = 0x40,
    .baudRate_kbps = gWarpI2cBaudRateKbps
};

while(1)
{
    uint8_t SOIL_data[2];
    uint8_t humidity_data[2];
    uint8_t temperature_data[2];
    uint8_t i2c_buffer[2];


    /* Get SI7021 humidity reading */
    status_SI7021_hum_w = I2C_DRV_MasterSendDataBlocking(
                            0 /* I2C instance */,
                            &devSI7021,
                            NULL,
                            0,
                            (uint8_t *)read_humidity,
                            2,
                            gWarpI2cTimeoutMilliseconds);

    if (status_SI7021_hum_w != kStatus_I2C_Success)
    {
        SEGGER_RTT_WriteString(0, " SI7021 humidity write communication failed\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }
    else 
    {
        SEGGER_RTT_WriteString(0, " SI7021 humidity writing succeeded\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }
/*
    status_SI7021_hum_r1 = I2C_DRV_MasterReceiveDataBlocking(
                            0,
                            &devSI7021,
                            NULL,
                            0,
                            (uint8_t *)i2c_buffer,
                            2,
                            gWarpI2cTimeoutMilliseconds);*/
    status_SI7021_hum_r2 = I2C_DRV_MasterReceiveDataBlocking(
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

    if (status_SI7021_hum_r2 != kStatus_I2C_Success)
    {
        SEGGER_RTT_WriteString(0, " SI7021 humidity read communication failed\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }
    else 
    {
        SEGGER_RTT_WriteString(0, " SI7021 humidity read succeeded\n");
        SEGGER_RTT_printf(0, " read: 0x%02x 0x%02x,", humidity_data[0], humidity_data[1]);
        SEGGER_RTT_printf(0, " %d\n", humidity_reading);
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }

    /* Get SI7021 temperature reading */
    status_SI7021_temp_w = I2C_DRV_MasterSendDataBlocking(
                            0 /* I2C instance */,
                            &devSI7021,
                            NULL,
                            0,
                            (uint8_t *)read_temperature,
                            2,
                            gWarpI2cTimeoutMilliseconds);

    if (status_SI7021_temp_w != kStatus_I2C_Success)
    {
        SEGGER_RTT_WriteString(0, " SI7021 temp write communication failed\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }
    else 
    {
        SEGGER_RTT_WriteString(0, " SI7021 temperature writing succeeded\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }
/*
    status_SI7021_temp_r1 = I2C_DRV_MasterReceiveDataBlocking(
                            0,
                            &devSI7021,
                            NULL,
                            0,
                            (uint8_t *)i2c_buffer,
                            2,
                            gWarpI2cTimeoutMilliseconds);*/
    status_SI7021_temp_r2 = I2C_DRV_MasterReceiveDataBlocking(
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

    if ((status_SI7021_temp_r2 != kStatus_I2C_Success) || (status_SI7021_temp_r2 != kStatus_I2C_Success))
    {
        SEGGER_RTT_WriteString(0, " SI7021 temperature read communication failed\n");
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }
    else 
    {
        SEGGER_RTT_WriteString(0, " SI7021 temperature read succeeded\n");
        SEGGER_RTT_printf(0, " read: 0x%02x 0x%02x,", temperature_data[0], temperature_data[1]);
        SEGGER_RTT_printf(0, " %d\n\n", temperature_reading);
        OSA_TimeDelay(gWarpMenuPrintDelayMilliseconds);
    }

    OSA_TimeDelay(3000);
}
//printSensorDataINA219(1);
disableI2Cpins();
