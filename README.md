# KQM2801AI
KQM2801AI is a air quality I2C sensor module.
This middleware through I2C to connect KQM2801AI for Raspberry PI.

### Environment:
- sudo apt-get install libi2c-dev
- sudo apt-get install i2c-tools

### How to build
Make

### How to use:
./KQM2801AI

### Result:
RAW : 0x5f 0x00 0x03 0x62
Density = 0.3 PPM level:Clean

### Density
- 00 level <2PPM  		Clean
- 01 level 2 ~ 8 PPM 	Good
- 02 level 8 ~ 15PPM 	Bad
- 03 level > 15PPM 		Danger