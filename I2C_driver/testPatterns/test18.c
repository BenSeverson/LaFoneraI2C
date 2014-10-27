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
#include <linux/i2c.h>

//#define SRF08_ADDR      0x70
//#define SRF08_SOFT_REV  0x00
//#define SRF08_CMD_REG   0x00
//#define SRF08_ECHO_0L   0x03
//#define SRF08_ECHO_0H   0x02
#define CONFIG_ADDR				0x0F
#define CHIP1_ADDR				0x20


int main(int argc, char *argv[]) {
   char *end;
   int res,file;
   int e1;
   char filename[20] ;
   long funcs;
   char msg[16];   
   
   
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
   
   if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
     if (errno == EBUSY) {
       printf("device is busyn");
     }
     exit(0);
   }
   msg[0]=0x0F;
   msg[1]=0x08;
   if ( write(file,msg,2) != 2) {
    printf("error\n");
   }
  /* res = i2c_smbus_read_byte_data(file, CONFIG_ADDR);
   if (res < 0) {
     printf("Cannot read config register");
   } else {
     printf("Config register: %02xn", res);
   }
   res = i2c_smbus_write_byte_data(file, CONFIG_ADDR, 0x08);
   if (res < 0) {
     printf("Cannot write command registern");
   }
   res = i2c_smbus_read_byte_data(file, CONFIG_ADDR);
   if (res < 0) {
     printf("Cannot read config register");
   } else {
     printf("Config register: %02xn", res);
   }*/
   
   msg[0]=0x06;
   msg[1]=0x00;
   msg[2]=0x00;
   if ( write(file,msg,3) != 3) {
    printf("error\n");
   }
   //res = i2c_smbus_write_block_data(file, 0x06, 2, msg);
   //if (res < 0) {
   //  printf("Cannot write port config register");
   //}
   msg[0]=0x02;
   msg[1]=0xFF;
   msg[2]=0xFF;
   if ( write(file,msg,3) != 3) {
    printf("error\n");
   }

   //res = i2c_smbus_write_block_data(file, 0x02, 2, msg);
   //if (res < 0) {
   //  printf("Cannot write blink phase 0 register");
   //}
   msg[0]=0x0E;
   msg[1]=0xFF;
   if ( write(file,msg,2) != 2) {
    printf("error\n");
   }
   //res = i2c_smbus_write_byte_data(file, 0x0E, 0xFF);
   //if (res < 0) {
   //  printf("Cannot write master intensity register");
   //}
   msg[0]=0x10;
   msg[1]=0x00;
   msg[2]=0x00;
   msg[3]=0xFF;
   msg[4]=0xFF;
   msg[5]=0xFF;
   msg[6]=0xFF;
   msg[7]=0xFF;
   msg[8]=0xFF;
   if ( write(file,msg,9) != 9) {
    printf("error\n");
   }


   //res = i2c_smbus_write_block_data(file, 0x10, 8, msg);
   //if (res < 0) {
   //  printf("Cannot write intensity register");  
   //}
   //i2c_smbus_write_byte_data(file, 0x10, 0x00);
   //i2c_smbus_write_byte_data(file, 0x11, 0x00);  
   
   //while (1) { /* loop forever */
  /*   res = i2c_smbus_write_byte_data(file, SRF08_CMD_REG, 0x50);
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
     printf("Echo Hi %d inches n", res);*/
   //}
   while(1){}
   
}
