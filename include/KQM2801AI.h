#ifndef SHT31D_H_
#define SHT31D_H_

#define KQM2801_INTERFACE_ADDR       1
#define KQM2801_DEFAULT_ADDR         0x2f
#define KQM2801_READ_SERIALNO        0x3780
#define KQM2801_MEAS_HIGHREP_STRETCH 0x2C06 // Doesn't work on PI
#define KQM2801_MEAS_MEDREP_STRETCH  0x2C0D // Seems to work on PI but shouldn't
#define KQM2801_MEAS_LOWREP_STRETCH  0x2C10 // Seems to work on PI but shouldn't
#define KQM2801_MEAS_HIGHREP         0x2400 // Doesn't work on PI
#define KQM2801_MEAS_MEDREP          0x240B
#define KQM2801_MEAS_LOWREP          0x2416
#define KQM2801_READSTATUS           0xF32D
#define KQM2801_CLEARSTATUS          0x3041
#define KQM2801_SOFTRESET            0x30A2
#define KQM2801_HEATER_ENABLE        0x306D
#define KQM2801_HEATER_DISABLE       0x3066

#define SHT32_DEFAULT_READ         KQM2801_MEAS_MEDREP

// Tempoary measure to fix the first write fail
#define SHT31D_FIX_INITIAL_FAIL 1

typedef enum
{
  KQM2801_OK = 0,
  KQM2801_CRC_CHECK_FAILED = 1,
  KQM2801_BAD = 2,
  KQM2801_READ_FAILED = 3, 
  KQM2801_WRITE_FAILED = 4
} kqm2801rtn;

int kqm2801_open(int i2c_address, uint8_t kqm2801_address);
int kqm2801_close(int file);

void delay (unsigned int howLong);
uint8_t crc8(const uint8_t *data, int len);

kqm2801rtn writeandread(int fd, uint16_t sndword, uint8_t *buffer, int readsize);
kqm2801rtn getDensity(float *density,char *level);


#endif /* SHT31D_H_ */
