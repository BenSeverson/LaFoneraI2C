#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

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
   unsigned char r,g,b;
   
   
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
	printf("RED: ");
	scanf("%u",&r);
	printf("GREEN: ");
	scanf("%u",&g);
	printf("BLUE: ");
	scanf("%u",&b);

	printf("R: 0x%X\tG: 0x%X\tB:%X\n",r&0x0F,g&0x0F,b&0x0F);
	
	if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
	}
	msg[0] = 0x10;
	msg[1] = (r&0x0F)|((g&0x0F)<<4);  //led 1
	msg[2] = (b&0x0F)|((r&0x0F)<<4);  //led 1,2
	msg[3] = (g&0x0F)|((b&0x0F)<<4);  //led 2
	msg[4] = (r&0x0F)|((g&0x0F)<<4);  //led 3
	msg[5] = (b&0x0F)|((r&0x0F)<<4);  //led 3,4
	msg[6] = (g&0x0F)|((b&0x0F)<<4);  //led 4
	msg[7] = (r&0x0F)|((g&0x0F)<<4);  //led 5
	msg[8] = (b&0x0F);				  //led 5
	res = write(file,msg,9);
	if (res < 0) {
			printf("ERROR\n");  
		}

	if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
	}
	res = write(file,msg,7);
	if (res < 0) {
			printf("ERROR\n");  
		}

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
	/*for(i=1;i<16;i++)
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
	}*/
}
   
}
