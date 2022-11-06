From Mike (Springlake)

"38ksensorI2Cgen.ino which is my 38k ir sensor pgm. It generates the 38k signal for the irsender, detects the receiver signal and sends it back to the CS (control station) as a mcp23017 sensor signal. it is defined with MCP23017::create(300, 16, 0x08, 40); in myHal.cpp. when a sensor changes state the pgm hits the int signal and hal goes out and does a read of all i2c devices. When this device is read it turns off int."



