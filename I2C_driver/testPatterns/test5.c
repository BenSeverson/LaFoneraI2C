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
#define CHIP2_ADDR				0x27


int main(int argc, char *argv[]) {
   char *end;
   int res,file;
   int e1;
   char filename[20] ;
   long funcs;
   unsigned char msg[16];
   unsigned char i;
   
   
   sprintf(filename,"/dev/i2c/0");
   if ((file = open(filename,O_RDWR)) < 0) {
     e1 = errno;
     if (e1 != ENOENT) {
       fprintf(stderr,"Error: Could not open file '%s' : %s\n",
               filename,strerror(e1));
       if(e1 == EACCES)
         fprintf(stderr,"Run as root?\n");
     }
   } 
   
   if (ioctl(file,I2C_FUNCS,&funcs) < 0) {
     fprintf(stderr,
             "Error: Could not get the adapter functionality maxtrix: %s\n",
             strerror(errno));
     exit(1);
   }
   if (! (funcs & I2C_FUNC_SMBUS_QUICK)) {
     fprintf(stderr,
             "Error: Can't use SMBus Quick Write command "
             "on this bus (ISA bus?)\n");
     exit(1);
   }
   fprintf(stderr, "I2C functionality: %08X\n", funcs);


   /////////////////////////////////////
   printf("CHIP1 Init:\n\n");
   if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
     if (errno == EBUSY) {
       printf("device is busy\n");
     }
     exit(0);
   }
   res = i2c_smbus_read_byte_data(file, CONFIG_ADDR);
   if (res < 0) {
     printf("Cannot read config register\n");
   } else {
     printf("Config register: %02x\n", res);
   }
   //res = i2c_smbus_write_byte_data(file, CONFIG_ADDR, 0x08);
   res = i2c_smbus_write_byte_data(file, CONFIG_ADDR, 0x00);
   if (res < 0) {
     printf("Cannot write command register\n");
   }
   res = i2c_smbus_read_byte_data(file, CONFIG_ADDR);
   i=res;
   if (res < 0) {
     printf("Cannot read config register\n");
   } else {
     printf("Config register: %02x\n", i);
   }
   
   msg[0]=0x06;
   msg[1]=0x00;
   msg[2]=0x00;
   res = write(file, msg, 3);
   //res = i2c_master_recv(file, msg, 3);
   //res = i2c_smbus_write_block_data(file, 0x06, 2, msg);
   //res = i2c_smbus_write_i2c_block_data(file, 0x06, 2, msg);
   if (res < 0) {
     printf("Cannot write port config register\n");
   }
   
   msg[0]=0x02;
   msg[1]=0xFF;
   msg[2]=0xFF;
   res = write(file,msg,3);
   //res = i2c_smbus_write_block_data(file, 0x02, 2, msg);
   //res = i2c_smbus_write_i2c_block_data(file, 0x02, 2, msg);
   if (res < 0) {
     printf("Cannot write blink phase 0 register\n");
   }

   res = i2c_smbus_write_byte_data(file, 0x0E, 0xFF);
   if (res < 0) {
     printf("Cannot write master intensity register\n");
   }


   msg[0]=0x10;
   msg[1]=0xFF;
   msg[2]=0xFF;
   msg[3]=0xFF;
   msg[4]=0xFF;
   msg[5]=0xFF;
   msg[6]=0xFF;
   msg[7]=0xFF;
   msg[8]=0xFF;
   res = write(file,msg,9);
   //res = i2c_smbus_write_block_data(file, 0x10, 8, msg);
   //res = i2c_smbus_write_i2c_block_data(file,0x10, 8, msg);
   if (res < 0) {
     printf("Cannot write intensity register\n");  
   }
///////////////////////////////
   printf("CHIP2 Init:\n\n");
   if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
     if (errno == EBUSY) {
       printf("device is busy\n");
     }
     exit(0);
   }
   res = i2c_smbus_read_byte_data(file, CONFIG_ADDR);
   if (res < 0) {
     printf("Cannot read config register\n");
   } else {
     printf("Config register: %02x\n", res);
   }
   //res = i2c_smbus_write_byte_data(file, CONFIG_ADDR, 0x08);
   res = i2c_smbus_write_byte_data(file, CONFIG_ADDR, 0x00);
   if (res < 0) {
     printf("Cannot write command register\n");
   }
   res = i2c_smbus_read_byte_data(file, CONFIG_ADDR);
   if (res < 0) {
     printf("Cannot read config register\n");
   } else {
     printf("Config register: %02x\n", res);
   }
   

   msg[0]=0x06;
   msg[1]=0x00;
   msg[2]=0x00;
   res = write(file,msg,3);
   //res = i2c_smbus_write_block_data(file, 0x06, 2, msg);
   //res = i2c_smbus_write_i2c_block_data(file, 0x06, 2, msg);
   if (res < 0) {
     printf("Cannot write port config register\n");
   }
   
   msg[0]=0x02;
   msg[1]=0xFF;
   msg[2]=0xFF;
   res = write(file,msg,3);
   //res = i2c_smbus_write_block_data(file, 0x02, 2, msg);
   //res = i2c_smbus_write_i2c_block_data(file, 0x02, 2, msg);
   if (res < 0) {
     printf("Cannot write blink phase 0 register\n");
   }

   res = i2c_smbus_write_byte_data(file, 0x0E, 0xFF);
   if (res < 0) {
     printf("Cannot write master intensity register\n");
   }

   msg[0]=0x10;
   msg[1]=0xFF;
   msg[2]=0xFF;
   msg[3]=0xFF;
   msg[4]=0xFF;
   msg[5]=0xFF;
   msg[6]=0xFF;
   msg[7]=0xFF;
   msg[8]=0xFF;
   res = write(file,msg,9);
   //res = i2c_smbus_write_block_data(file, 0x10, 8, msg);
   //res = i2c_smbus_write_i2c_block_data(file, 0x10, 8, msg);
   if (res < 0) {
     printf("Cannot write intensity register\n");  
   }

if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
     if (errno == EBUSY) {
       printf("device is busy\n");
     }
     exit(0);
   }
while(1)
{
	/*for(i=0;i<16;i++)
	{

		if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		msg[0] = 0x10;
		msg[1] = i<<4;
		msg[2] = i|0xF0;
		res = write(file,msg,3);
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 2, msg);
		//res = i2c_smbus_write_byte_data(file, 0x10, i<<4);

		if (res < 0) {
			printf("ERROR\n");  
		}
		//res = i2c_smbus_write_byte_data(file, 0x11, i);
		//if (res < 0) {
		//	printf("ERROR\n");  
		//}
		

		if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 2, msg);
		//res = i2c_smbus_write_byte_data(file, 0x10, i<<4);
		res = write(file,msg,3);
		if (res < 0) {
			printf("ERROR\n");  
		}
		printf("R: 0x%X\tG: 0x%X\tB:%X\n",i,0,i);
		usleep(50000);
	}*/
	for(i=1;i<16;i++)
	{
		if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//msg[0] = (0x0F-i)<<4;
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, (0x0F-i)<<4);
		if (res < 0) {
			printf("ERROR\n");  
		}
		if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, (0x0F-i)<<4);
		if (res < 0) {
			printf("ERROR\n");  
		}
		printf("R: 0x%X\tG: 0x%X\tB:%X\n",0x0F-i,0,0x0F);
		usleep(50000);
	}
	for(i=1;i<16;i++)
	{
		if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//msg[0] = i;
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, i);
		if (res < 0) {
			printf("ERROR\n");  
		}
		if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, i);
		if (res < 0) {
			printf("ERROR\n");  
		}
		printf("R: 0x%X\tG: 0x%X\tB:%X\n",0,i,0x0F);
		usleep(50000);
	}
	/*for(i=1;i<16;i++)
	{
		if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//msg[0] = 0x0F-i;
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x11, (0x0F-i)|0xF0);
		if (res < 0) {
			printf("ERROR\n");  
		}
		if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x11, (0x0F-i)|0xF0);
		if (res < 0) {
			printf("ERROR\n");  
		}
		printf("R: 0x%X\tG: 0x%X\tB:%X\n",0,0x0F,0x0F-i);
		usleep(50000);
	}*/
	for(i=1;i<16;i++)
	{
		if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//msg[0] = (i<<4) | 0x0F;
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, (i<<4) | 0x0F);
		if (res < 0) {
			printf("ERROR\n");  
		}
		if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, (i<<4) | 0x0F);
		if (res < 0) {
			printf("ERROR\n");  
		}
		printf("R: 0x%X\tG: 0x%X\tB:%X\n",i,0x0F,0);
		usleep(50000);
	}
	for(i=1;i<16;i++)
	{
		if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//msg[0] = 0xF0 | (0x0F-i);
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, 0xF0 | (0x0F-i));
		if (res < 0) {
			printf("ERROR\n");  
		}
		if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, 0xF0 | (0x0F-i));
		if (res < 0) {
			printf("ERROR\n");  
		}
		printf("R: 0x%X\tG: 0x%X\tB:%X\n",0x0F,0x0F-i,0);
		usleep(50000);
	}
	for(i=1;i<16;i++)
	{
		if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//msg[0] = (0x0F-i)<<4;
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, (0x0F-i)<<4);
		if (res < 0) {
			printf("ERROR\n");  
		}
		if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		//res = i2c_smbus_write_i2c_block_data(file,0x10, 1, msg);
		res = i2c_smbus_write_byte_data(file, 0x10, (0x0F-i)<<4);
		if (res < 0) {
			printf("ERROR\n");  
		}
		printf("R: 0x%X\tG: 0x%X\tB:%X\n",0x0F-i,0,0);
		usleep(50000);
	}
}
   
}
