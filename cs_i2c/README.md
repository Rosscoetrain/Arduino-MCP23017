From Mike (Springlake)

"cs_i2c_ltc.ino it is converted from a CMRI pgm that ran 8 servo turnouts and a local control panel pushbutton to request a turnout toggle. I am still debugging this pgm so it has lots of print statements. It uses the hi 8 bits as outputs to control the turnouts and the low 8bits for the pushbutton inputs,  I also wrote some myFilter.cpp code that programs the servo endpoints, reads the servo endpoints, and writes them to eeprom. The turnout pgm is defined with MCP23017::create(324, 16, 0x18, 40  ); in myHal.cpp"


