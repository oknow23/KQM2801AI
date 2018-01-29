/*
 * https://www.kernel.org/doc/Documentation/i2c/dev-interface
 * This depends on i2c dev lib
 * sudo apt-get install libi2c-dev
 *
 * Below is also a good one to have, but be careful i2cdump from the below cause the kqm2801 interface to become unstable for me
 * and requires a hard-reset to recover correctly.
 * sudo apt-get install i2c-tools
 * 
 * Author:  MiLi
 * Email:   oknow23@gmail.com
 */
 
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h> 
#include <fcntl.h>
#include <elf.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "KQM2801AI.h"


/*
 * delay:
 *	Wait for some number of milliseconds
 *********************************************************************************
 */

void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}


/*
 * kqm2801_open:
 *	Open a file pointer to the appropiate I2C buss and kqm2801 address
 *********************************************************************************
 */
int kqm2801_open(int i2c_address, uint8_t kqm2801_address)
{
  char filename[20];
  int fp;
  
  snprintf(filename, 19, "/dev/i2c-%d", i2c_address);
  fp = open(filename, O_RDWR);
  if (fp < 0) {
    printf("oknow[%s:%d,%s] open file fail\n", __FILE__, __LINE__, __func__);
    return fp;
  }

  if (ioctl(fp, I2C_SLAVE, kqm2801_address) < 0) {
    close(fp);
    printf("oknow[%s:%d,%s] iocntrl fail\n", __FILE__, __LINE__, __func__);
    return -1;
  }
  
  return fp;
}

int kqm2801_close(int fp)
{
  return close(fp);
}

/*
 * writeandread:
 *	Write I2C command and read return.  Pass 0 for readsize if you only want to write
 *********************************************************************************
 */

kqm2801rtn writeandread(int fp, uint16_t sndword, uint8_t *buffer, int readsize)
{
  int rtn;
  int sendsize = 2;
  uint8_t snd[sendsize];

  if (readsize > 0) {
    delay(10);
    rtn = read(fp, buffer, readsize);
    if ( rtn < readsize) {
      return KQM2801_READ_FAILED;
    }
    printf("RAW : 0x%02x 0x%02x 0x%02x 0x%02x\n",buffer[0],buffer[1],buffer[2],buffer[3]);
  }
  
  return KQM2801_OK;
}

/*
 * getserialnum:
 *	gets density values
 *********************************************************************************
 */
kqm2801rtn getDensity( float *density,char *level)
{
  uint8_t buf[10];
  int rtn;
  int file;

  file = kqm2801_open(KQM2801_INTERFACE_ADDR, KQM2801_DEFAULT_ADDR);
  
  rtn = writeandread(file, SHT32_DEFAULT_READ, buf, 8);
  kqm2801_close(file);

  if (rtn != KQM2801_OK)
    return rtn;
  else {
    /* PPM */
    *density = 0.1 * ((float) (buf[1]+buf[2]));
 
    if( *density < 2 )
      strcat(level,"Green"); //0
    else if( *density <= 8)
      strcat(level,"Yellow"); //1;
    else if( *density <= 15)
      strcat(level,"Orange");
    else if( *density > 15)
      strcat(level,"Red");

    /* check sum */
    if((buf[0]+buf[1]+buf[2]) != buf[3] )
      return KQM2801_CRC_CHECK_FAILED;
  }
  
  return KQM2801_OK;
}

void printusage(char *selfname)
{
  printf("%s (options)\n", selfname);
  printf("\ts - print status\n");
  printf("\tp - print temp & humid\n");
  printf("\t-h this\n\n");
  printf("Example to print temperaturehum & humidity, serial number & status\n  %s p n s\n\n", selfname);
}

int display()
{
  float density;
  char level[64] = "";
  int rtn = getDensity(&density,level);
  
  if ( rtn != KQM2801_OK && rtn != KQM2801_CRC_CHECK_FAILED) {
    printf("ERROR:- Get air failed\n!");
  }
  else{
    if ( rtn == KQM2801_CRC_CHECK_FAILED) {
      printf("WARNING:- Get status CRC check failed, don't trust results\n");
    }

    printf("Density = %.1lf PPM level:%s\n",density,level);
  }

  return rtn;
}

int main(int argc, char *argv[])
{
  int file;
  int i;

  if (argc <= 1) {
    sleep(1);

    display();

    exit (EXIT_SUCCESS);
  }
  
  for (i = 1; i < argc; i++)
  {
    if (strcmp (argv[i], "-h") == 0)
    {
      printusage(argv[0]);
      exit (EXIT_SUCCESS);
    }  else {
      printf("ERROR :- '%s' unknown option\n",argv[i]);
    }
      
    delay(30);
  }
  
  return 0;
}