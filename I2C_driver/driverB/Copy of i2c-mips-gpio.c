/* ------------------------------------------------------------------------- */
/* i2c-mips-gpio.c i2c-hw access for WRT54G                                  */
/* ------------------------------------------------------------------------- */
/*  Based on i2c-philips-par.c by Simon G. Vogl
    It seems that this is very simalar to scx200_i2c.c
    Perhaps there should be a generic GPIO interface in the kernel
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/* ------------------------------------------------------------------------- */
 
/* $Id: $ */
 
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/stddef.h>
 
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
 
#ifndef __exit
#define __exit __init
#endif
 
typedef unsigned int uint32;
/* GPIO Defines taken from mmc.c */
static volatile uint32 *gpioaddr_input = (uint32 *)0xb8000060;
static volatile uint32 *gpioaddr_output = (uint32 *)0xb8000064;
static volatile uint32 *gpioaddr_enable = (uint32 *)0xb8000068;
static volatile uint32 *gpioaddr_control = (uint32 *)0xb800006c;
 
// should this use the sb_gpio* functions from sbutils.h (like gpio.c)?
 
#define GPIO_WHITE 2
#define GPIO_ORANGE 3
#define GPIO_CISCO 4
#define GPIO_RESET 6
 
#define GPIO_CLOCK (1 << GPIO_CISCO)
#define GPIO_DATA  (1 << GPIO_RESET)
 
/* ----- local functions ---------------------------------------------- */
 
// we probably also need to invert all the signals...
 
// we need to change gpioaddr_enable to change from Input to Output
 
static void bit_gpio_set(unsigned int mask) {
        unsigned int port_state;
 
        port_state = *gpioaddr_enable;
        port_state |= mask;
        *gpioaddr_enable = port_state;
 
        port_state = *gpioaddr_input;
        port_state |= mask;
        *gpioaddr_output = port_state;
}
 
static void bit_gpio_clear(unsigned int mask) {
        unsigned int port_state;
 
        port_state = *gpioaddr_enable;
        port_state |= mask;
        *gpioaddr_enable = port_state;
 
        port_state = *gpioaddr_input;
        port_state &= ~mask;
        *gpioaddr_output = port_state;
}
 
static int bit_gpio_get(int mask) {
        unsigned char port_state;
        port_state = *gpioaddr_enable; /* Read current config */
        port_state &= ~mask; /* Set to input */
        *gpioaddr_enable = port_state;
 
        port_state = *gpioaddr_input;
 
        return port_state & mask;
}
 
static void bit_gpio_setscl(void *data, int state) {
        if (state) {
                bit_gpio_set(GPIO_CLOCK);
        } else {
                bit_gpio_clear(GPIO_CLOCK);
        }
}
 
static void bit_gpio_setsda(void *data, int state) {
        if (state) {
                bit_gpio_set(GPIO_DATA);
        } else {
                bit_gpio_clear(GPIO_DATA);
        }
}
 
static int bit_gpio_getscl(void *data) {
        return bit_gpio_get(GPIO_CLOCK);
}
 
static int bit_gpio_getsda(void *data) {
        return bit_gpio_get(GPIO_DATA);
}
 
/*   */
 
static int bit_gpio_reg(struct i2c_client *client) {
        return 0;
}
 
static int bit_gpio_unreg(struct i2c_client *client) {
        return 0;
}
 
static void bit_gpio_inc_use(struct i2c_adapter *adap) {
        MOD_INC_USE_COUNT;
}
 
static void bit_gpio_dec_use(struct i2c_adapter *adap) {
        MOD_DEC_USE_COUNT;
}
 
/* ------------------------------------------------------------------------
 * Encapsulate the above functions in the correct operations structure.
 * This is only done when more than one hardware adapter is supported.
 */
static struct i2c_algo_bit_data bit_gpio_data = {
        NULL,
        bit_gpio_setsda,
        bit_gpio_setscl,
        bit_gpio_getsda,
        bit_gpio_getscl,
        80, 80, 100,              /*      waits, timeout */
};
 
static struct i2c_adapter bit_gpio_ops = {
        "WRT54G GPIO",
        I2C_HW_B_LP, // what to use here ???
        NULL,
        &bit_gpio_data,
        bit_gpio_inc_use,
        bit_gpio_dec_use,
        bit_gpio_reg,
 
        bit_gpio_unreg,
};
 
int __init i2c_bitgpio_init(void) {
        unsigned char gpio_outen;
 
        printk(KERN_INFO "i2c-mpis-gpio.o: i2c WRT54G GPIO module version %s (%s)\n", I2C_VERSION, I2C_DATE);
 
        /* do some init */
        bit_gpio_set(GPIO_CLOCK);
        bit_gpio_clear(GPIO_DATA);
 
        /* I don't know what it does, but if we use the reset button we need to set control to 0
           so I assume that is a good thing to do */
 
        gpio_outen = *gpioaddr_control;
        gpio_outen = gpio_outen & ~(GPIO_CLOCK | GPIO_DATA);
        *gpioaddr_control = gpio_outen;
 
        if(i2c_bit_add_bus(&bit_gpio_ops) < 0)
                return -ENODEV;
 
        return 0;
}void __exit i2c_bitgpio_exit(void) {
        /* do any clean up. we set them back to input */
        bit_gpio_get(GPIO_CLOCK);
        bit_gpio_get(GPIO_DATA);
 
        /* we should restore the control mode */
 
        i2c_bit_del_bus(&bit_gpio_ops);
}
 
EXPORT_NO_SYMBOLS;
 
MODULE_AUTHOR("John Newbigin <jn@it.swin.edu.au>");
MODULE_DESCRIPTION("I2C-Bus adapter routines for WRT54G GPIO");
MODULE_LICENSE("GPL");
 
#ifdef MODULE
int init_module(void) {
        return i2c_bitgpio_init();
}
 
void cleanup_module(void) {
        i2c_bitgpio_exit();
}
#endif