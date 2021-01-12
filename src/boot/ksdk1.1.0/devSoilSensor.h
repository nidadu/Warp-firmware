extern volatile uint32_t		gWarpI2cBaudRateKbps;
extern volatile uint32_t		gWarpI2cTimeoutMilliseconds;
extern volatile uint32_t		gWarpMenuPrintDelayMilliseconds;


i2c_status_t	status_SOIL_w, status_SOIL_r;
uint8_t read_cmd[2] = {0x0F, 0x10};

void read_moisture(void);