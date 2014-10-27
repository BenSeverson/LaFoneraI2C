/* read input from keyboard and output to LEDs */


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <i2c-dev.h>
/*#include <linux/i2c.h>*/

#define CONFIG_ADDR				0x0F
#define INTENSITY_ADDR			0x10
#define BLINK0_ADDR				0x02
#define PORT_CONFIG_ADDR		0x06
#define MASTER_INTENSITY_ADDR	0x0E

#define CHIP1_ADDR				0x20
#define CHIP2_ADDR				0x27

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} rgb;

int write_colors(int file, rgb intensity[3][3])
{
	int res;
	unsigned char msg[16];
	if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
		if (errno == EBUSY) {
			printf("device is busy\n");
		}
		exit(0);
	}
	msg[0] = INTENSITY_ADDR;
	msg[1] = ((15-intensity[0][0].red)&0x0F)|(((15-intensity[0][0].green)&0x0F)<<4);  //led 1
	msg[2] = ((15-intensity[0][0].blue)&0x0F)|(((15-intensity[0][1].red)&0x0F)<<4);  //led 1,2
	msg[3] = ((15-intensity[0][1].green)&0x0F)|(((15-intensity[0][1].blue)&0x0F)<<4);  //led 2
	msg[4] = ((15-intensity[0][2].red)&0x0F)|(((15-intensity[0][2].green)&0x0F)<<4);  //led 3
	msg[5] = ((15-intensity[0][2].blue)&0x0F)|(((15-intensity[1][0].red)&0x0F)<<4);  //led 3,4
	msg[6] = ((15-intensity[1][0].green)&0x0F)|(((15-intensity[1][0].blue)&0x0F)<<4);  //led 4
	msg[7] = ((15-intensity[1][1].red)&0x0F)|(((15-intensity[1][1].green)&0x0F)<<4);  //led 5
	msg[8] = ((15-intensity[1][1].blue)&0x0F);				  //led 5
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
	msg[1] = ((15-intensity[1][2].red)&0x0F)|(((15-intensity[1][2].green)&0x0F)<<4);  //led 6
	msg[2] = ((15-intensity[1][2].blue)&0x0F)|(((15-intensity[2][0].red)&0x0F)<<4);  //led 6,7
	msg[3] = ((15-intensity[2][0].green)&0x0F)|(((15-intensity[2][0].blue)&0x0F)<<4);  //led 7
	msg[4] = ((15-intensity[2][1].red)&0x0F)|(((15-intensity[2][1].green)&0x0F)<<4);  //led 8
	msg[5] = ((15-intensity[2][1].blue)&0x0F)|(((15-intensity[2][2].red)&0x0F)<<4);  //led 8,9
	msg[6] = ((15-intensity[2][2].green)&0x0F)|(((15-intensity[2][2].blue)&0x0F)<<4);  //led 9
	res = write(file,msg,7);
	if (res < 0) {
		printf("ERROR\n");  
	}
}

void init_colors(rgb intensity[3][3])
{
	int r,c;
	for(r=0;r<3;r++)
		for(c=0;c<3;c++)
			intensity[r][c].red=intensity[r][c].green=intensity[r][c].blue=0;
}


int main(int argc, char *argv[]) {
	char *end;
	int res,file;
	int e1;
	int temp;
	char filename[20] ;
	long funcs;
	unsigned char msg[16];
	unsigned char i;
	unsigned char r,g,b;
	rgb intensity[3][3];
	int row;
	int column;

	/*** Begin I2C Init ***/
	sprintf(filename,"/dev/i2c-0");
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
	/*** End I2C Init ***/

	// Start with chip 1
	if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
		if (errno == EBUSY) {
			printf("device is busy\n");
		}
		exit(0);
	}
	/*** Begin Chip 1,2 Init ***/
	for(temp=0;temp<2;temp++)
	{
		printf("CHIP%d Init:\n",temp+1);

		res = i2c_smbus_read_byte_data(file, CONFIG_ADDR);
		if (res < 0) {
			printf("Cannot read config register\n");
		} else {
			printf("Config register: %02x\n", res);
		}
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

		msg[0]=PORT_CONFIG_ADDR;
		msg[1]=0x00;
		msg[2]=0x00;
		res = write(file, msg, 3);
		if (res < 0) {
			printf("Cannot write port config register\n");
		}

		msg[0]=BLINK0_ADDR;
		msg[1]=0xFF;
		msg[2]=0xFF;
		res = write(file,msg,3);
		if (res < 0) {
			printf("Cannot write blink phase 0 register\n");
		}

		res = i2c_smbus_write_byte_data(file, MASTER_INTENSITY_ADDR, 0xFF);
		if (res < 0) {
			printf("Cannot write master intensity register\n");
		}
		msg[0]=INTENSITY_ADDR;
		msg[1]=0xFF;
		msg[2]=0xFF;
		msg[3]=0xFF;
		msg[4]=0xFF;
		msg[5]=0xFF;
		msg[6]=0xFF;
		msg[7]=0xFF;
		msg[8]=0xFF;
		res = write(file,msg,9);
		if (res < 0) {
			printf("Cannot write intensity register\n");  
		}
		// Switch to 2nd chip
		if (ioctl(file,I2C_SLAVE,CHIP2_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
	}

	
	init_colors(intensity);

	while(1)
	{
		printf("Row (1-3): ");
		scanf("%u",&row);
		printf("Column (1-3): ");
		scanf("%u",&column);
		printf("Color values (1-15):\n");
		printf("RED: ");
		scanf("%u",&r);
		printf("GREEN: ");
		scanf("%u",&g);
		printf("BLUE: ");
		scanf("%u",&b);
		intensity[row-1][column-1].red = r;
		intensity[row-1][column-1].green = g;
		intensity[row-1][column-1].blue = b;
		write_colors(file,intensity);

		//printf("R: 0x%X\tG: 0x%X\tB:%X\n",r&0x0F,g&0x0F,b&0x0F);

		/*
		if (ioctl(file,I2C_SLAVE,CHIP1_ADDR) < 0) {
			if (errno == EBUSY) {
				printf("device is busy\n");
			}
			exit(0);
		}
		msg[0] = INTENSITY_ADDR;
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
		}*/
	}

}
