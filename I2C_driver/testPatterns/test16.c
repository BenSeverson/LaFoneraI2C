/*---------------------------------------------------------------------------*/
/*                                                                           */
/* srf08.c -- example program to exercise Devantech SRF08 sonar ranger with  */
/*            linux i2c drivers                                              */
/*                                                                           */
/*  Author: Terry Parks                                                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

#define SRF08_ADDR      0x70
#define SRF08_SOFT_REV  0x00
#define SRF08_CMD_REG   0x00
#define SRF08_ECHO_0L   0x03
#define SRF08_ECHO_0H   0x02


int main(int argc, char *argv[]) {
   char *end;
   int res,file;
   int e1;
   char filename[20] ;
   long funcs;
   
   sprintf(filename,"/dev/i2c/0");
   if ((file = open(filename,O_RDWR)) < 0) {
     e1 = errno;
     if (e1 != ENOENT) {
       fprintf(stderr,"Error: Could not open file '%s' : %sn",
               filename,strerror(e1));
       if(e1 == EACCES)
         fprintf(stderr,"Run as root?n");
     }
   } 
   
   if (ioctl(file,I2C_FUNCS,&funcs) < 0) {
     fprintf(stderr,
             "Error: Could not get the adapter functionality maxtrix: %sn",
             strerror(errno));
     exit(1);
   }
   if (! (funcs & I2C_FUNC_SMBUS_QUICK)) {
     fprintf(stderr,
             "Error: Can't use SMBus Quick Write command "
             "on this bus (ISA bus?)n");
     exit(1);
   }
   fprintf(stderr, "I2C functionality: %08Xn", funcs);
   
   if (ioctl(file,I2C_SLAVE,SRF08_ADDR) < 0) {
     if (errno == EBUSY) {
       printf("device is busyn");
     }
     exit(0);
   }
   
   res = i2c_smbus_read_byte_data(file, SRF08_SOFT_REV);
   if (res < 0) {
     printf("Cannot read software revision leveln");
   } else {
     printf("Software revision level: %02xn", res);
   }
   while (1) { /* loop forever */
     res = i2c_smbus_write_byte_data(file, SRF08_CMD_REG, 0x50);
     if (res < 0) {
       printf("Cannot write command registern");
     }
     usleep(70000);
     while (1) {
       res = i2c_smbus_read_byte_data(file, SRF08_SOFT_REV);
       if (res != 0xff)
         break;
     }
     res = i2c_smbus_read_byte_data(file, SRF08_ECHO_0L);
     printf("Echo Lo %d inches ", res);
     res = i2c_smbus_read_byte_data(file, SRF08_ECHO_0H);
     printf("Echo Hi %d inches n", res);
   }
}
