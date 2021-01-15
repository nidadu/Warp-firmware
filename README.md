Author: Nida Duobaite, nd435, Peterhouse college.

This contains the embedded system code for Coursework 5 on the plant health monitor providing live data on temperature, relative humidity and soil moisture with its forecast.

In comparison with the original Warp firmware, this also has 2 sensor drivers: 
  Warp-firmware/src/boot/ksdk1.1.0/devSI7021_new.c for a Si7021 temperature and humidity sensor and 
  Warp-firmware/src/boot/ksdk1.1.0/devSoilSensor.c for an Adafruit capacitive sensor.
  
The oled display driver (Warp-firmware/src/boot/ksdk1.1.0/devSSD1331.c) is also heavily modified so it could write text, the base for that is the mbed driver from https://os.mbed.com/users/star297/code/ssd1331/

The main program is in the Warp-firmware/src/boot/ksdk1.1.0/warp-kl03-ksdk1.1-boot.c file. Firstly, it initialises the oled display, then defines the moving average for soil moistur
The program then goes to the while loop where it measures temperature, relative humidity and soil moisture every 20s.
