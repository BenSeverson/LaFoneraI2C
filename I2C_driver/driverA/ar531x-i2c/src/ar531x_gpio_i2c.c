/* linux/drivers/i2c/busses/scx200_i2c.c

   Copyright (c) 2001,2002 Christer Weinigel <wingel@nano-system.com>

   National Semiconductor SCx200 I2C bus on GPIO pins

   Based on i2c-velleman.c Copyright (C) 1995-96, 2000 Simon G. Vogl

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.		     
*/

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <asm/io.h>
#include "ar531x.h"

#define NAME "ar531x_gpio_i2c"

MODULE_AUTHOR("Ben Severson");
MODULE_DESCRIPTION("AR531X GPIO I2C Driver");
MODULE_LICENSE("GPL");

/* default to SW1 pins 1 and 2 on Fonera */
static int scl = 4;
static int sda = 3;

module_param(scl, int, 0);
MODULE_PARM_DESC(scl, "GPIO line for SCL");
module_param(sda, int, 0);
MODULE_PARM_DESC(sda, "GPIO line for SDA");

static void ar531x_gpio_i2c_setscl(void *data, int state)
{
	u32 reg;

	if(state)
	{
		reg = sysRegRead(AR5315_GPIO_CR);
		reg &= ~(AR531X_GPIO_CR_M(scl));
		sysRegWrite(AR5315_GPIO_CR,reg);
		(void)sysRegRead(AR5315_GPIO_CR);
	}
	else
	{
		reg = sysRegRead(AR5315_GPIO_CR);
		reg |= AR531X_GPIO_CR_M(scl);
		sysRegWrite(AR5315_GPIO_CR,reg);
		(void)sysRegRead(AR5315_GPIO_CR);
		reg = sysRegRead(AR5315_GPIO_DO);
		reg &= ~(AR531X_GPIO_CR_M(scl));
		sysRegWrite(AR5315_GPIO_DO,reg);
		(void)sysRegRead(AR5315_GPIO_DO);
	}
}

static void ar531x_gpio_i2c_setsda(void *data, int state)
{
	u32 reg;

	if(state)
	{
		reg = sysRegRead(AR5315_GPIO_CR);
		reg &= ~(AR531X_GPIO_CR_M(sda));
		sysRegWrite(AR5315_GPIO_CR,reg);
		(void)sysRegRead(AR5315_GPIO_CR);
	}
	else
	{
		reg = sysRegRead(AR5315_GPIO_CR);
		reg |= AR531X_GPIO_CR_M(sda);
		sysRegWrite(AR5315_GPIO_CR,reg);
		(void)sysRegRead(AR5315_GPIO_CR);
		reg = sysRegRead(AR5315_GPIO_DO);
		reg &= ~(AR531X_GPIO_CR_M(sda));
		sysRegWrite(AR5315_GPIO_DO,reg);
		(void)sysRegRead(AR5315_GPIO_DO);
	}
}

static int ar531x_gpio_i2c_getscl(void *data)
{
	u32 reg;

	reg = sysRegRead(AR5315_GPIO_CR);
	reg &= ~(AR531X_GPIO_CR_M(scl));
	sysRegWrite(AR5315_GPIO_CR,reg);
	(void)sysRegRead(AR5315_GPIO_CR);

	reg = sysRegRead(AR5315_GPIO_DI);

	if (AR531X_GPIO_CR_M(scl) & reg)
		return 1;
	else
		return 0;
}

static int ar531x_gpio_i2c_getsda(void *data)
{
	u32 reg;

	reg = sysRegRead(AR5315_GPIO_CR);
	reg &= ~(AR531X_GPIO_CR_M(sda));
	sysRegWrite(AR5315_GPIO_CR,reg);
	(void)sysRegRead(AR5315_GPIO_CR);

	reg = sysRegRead(AR5315_GPIO_DI);

	if (AR531X_GPIO_CR_M(sda) & reg)
		return 1;
	else
		return 0;
}

/* ------------------------------------------------------------------------
 * Encapsulate the above functions in the correct operations structure.
 * This is only done when more than one hardware adapter is supported.
 */

static struct i2c_algo_bit_data ar531x_gpio_i2c_data = {
	.setsda		= ar531x_gpio_i2c_setsda,
	.setscl		= ar531x_gpio_i2c_setscl,
	.getsda		= ar531x_gpio_i2c_getsda,
	.getscl		= ar531x_gpio_i2c_getscl,
	.udelay		= 5,
	.timeout	= 100,
};

static struct i2c_adapter ar531x_gpio_i2c_ops = {
	.owner		   = THIS_MODULE,
	.algo_data	   = &ar531x_gpio_i2c_data,
	.name	= "AR531X GPIO I2C",
};

static int ar531x_gpio_i2c_init(void)
{
	u32 reg;
	pr_debug(NAME ": AR531X GPIO I2C Driver\n");

	pr_debug(NAME ": SCL=GPIO%02u, SDA=GPIO%02u\n", scl, sda);

	if (scl == -1 || sda == -1 || scl == sda) {
		printk(KERN_ERR NAME ": scl and sda must be specified\n");
		return -EINVAL;
	}

	/* make inputs */
	reg = sysRegRead(AR5315_GPIO_CR);
	reg &= ~(AR531X_GPIO_CR_M(sda) | AR531X_GPIO_CR_M(scl));
	sysRegWrite(AR5315_GPIO_CR,reg);
	(void)sysRegRead(AR5315_GPIO_CR);

	if (i2c_bit_add_bus(&ar531x_gpio_i2c_ops) < 0) {
		printk(KERN_ERR NAME ": adapter %s registration failed\n", 
		       ar531x_gpio_i2c_ops.name);
		return -ENODEV;
	}
	
	return 0;
}

static void ar531x_gpio_i2c_cleanup(void)
{
	i2c_bit_del_bus(&ar531x_gpio_i2c_ops);
}

module_init(ar531x_gpio_i2c_init);
module_exit(ar531x_gpio_i2c_cleanup);

/*
    Local variables:
        compile-command: "make -k -C ../.. SUBDIRS=drivers/i2c modules"
        c-basic-offset: 8
    End:
*/
