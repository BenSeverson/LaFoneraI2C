//proc_gpio: AR5315 GPIO pins in /proc/gpio/
// by olg 
// GPL'ed
// some code stolen from Yoshinori Sato <ysato@users.sourceforge.jp>

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /* for copy_from_user */
#include "ar531xlnx.h"

#define PROCFS_MAX_SIZE 64

struct proc_dir_entry *proc_gpio, *gpio_dir;

//Masks for data exchange through "void *data" pointer
#define GPIO_IN (1<<5)
#define GPIO_OUT (1<<6)
#define GPIO_DIR (1<<7)
#define PIN_MASK 0x1f

/*
#define AR5315_GPIO_DI          (AR5315_DSLBASE + 0x0088)
#define AR5315_GPIO_DO          (AR5315_DSLBASE + 0x0090)
#define AR5315_GPIO_CR          (AR5315_DSLBASE + 0x0098)
#define AR5315_GPIO_INT         (AR5315_DSLBASE + 0x00a0)

#define GPIO_CR_M(x)                (1 << (x))                  mask for i/o
#define GPIO_CR_O(x)                (1 << (x))                  output 
#define GPIO_CR_I(x)                (0 << (x))                   input 

#define AR5315_RESET_GPIO       5
#define AR5315_NUM_GPIO         22
*/
		
static void cleanup_proc(void);
		
//The buffer used to store the data returned by the proc file
static char procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;


static int gpio_proc_read(char *buf, char **start, off_t offset, 
                           int len, int *eof, void *data)
{
		u32 reg=0;
		
		if ( (unsigned int) data & GPIO_IN)
			reg = sysRegRead(AR5315_GPIO_DI);
		if ( (unsigned int) data & GPIO_OUT)
			reg = sysRegRead(AR5315_GPIO_DO);
		if ( (unsigned int) data & GPIO_DIR)
			reg = sysRegRead(AR5315_GPIO_CR);
			
		//printk (KERN_NOTICE "gpio_proc: read: value of reg ... %#08X   .. value of data %#08X\n",
		//																	reg,(unsigned int)data);
			
		if ( GPIO_CR_M( ((unsigned int)data) & PIN_MASK ) & reg)
			buf[0]='1';
		else
			buf[0]='0';
		buf[1]=0;
		
		*eof = 1;
		
		return(2);

 }

static int gpio_proc_info_read(char *buf, char **start, off_t offset, 
                           int len, int *eof, void *data)
{
		*eof = 1;
		return (
				sprintf(buf,"GPIO_IN   %#08X \nGPIO_OUT  %#08X \nGPIO_DIR  %#08X \n",				
				sysRegRead(AR5315_GPIO_DI),
				sysRegRead(AR5315_GPIO_DO),
				sysRegRead(AR5315_GPIO_CR)
						));
 }
  
 
static int gpio_proc_write(struct file *file, const char *buffer, unsigned long count,
					void *data)
{
	u32 reg=0;
	
	/* get buffer size */
	procfs_buffer_size = count;
	if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
			procfs_buffer_size = PROCFS_MAX_SIZE;
			}
	/* write data to the buffer */
	if ( copy_from_user(procfs_buffer, buffer, procfs_buffer_size) ) {
			return -EFAULT;
			}
		
	procfs_buffer[procfs_buffer_size]=0;
	//printk (KERN_NOTICE "you wrote \"%c\" to GPIO %i\n",procfs_buffer[0], ((int)data) & 0xff );
	
	//printk (KERN_NOTICE "GPIO PROCID %#08X\n",(int) data );
	
	if ( (unsigned int) data & GPIO_IN)
		reg = sysRegRead(AR5315_GPIO_DI);
	if ( (unsigned int) data & GPIO_OUT)
		reg = sysRegRead(AR5315_GPIO_DO);
	if ( (unsigned int) data & GPIO_DIR)
		reg = sysRegRead(AR5315_GPIO_CR);
		
	//printk (KERN_NOTICE "value before ... %#08X\n",reg);
	
	if (procfs_buffer[0]=='0' || procfs_buffer[0]=='i')
		reg = reg & ~( GPIO_CR_M( ((unsigned int)data) & PIN_MASK ) );
	if (procfs_buffer[0]=='1' || procfs_buffer[0]=='o')
		reg = reg  | GPIO_CR_M( ((unsigned int)data) & PIN_MASK ) ;

	//printk (KERN_NOTICE ".. and after write %#08X \n",reg);
	
	if ( (unsigned int) data & GPIO_IN)	{
		sysRegWrite(AR5315_GPIO_DI,reg);
		(void)sysRegRead(AR5315_GPIO_DI); /* flush write to hardware */
		}
	if ( (unsigned int) data & GPIO_OUT) {
		sysRegWrite(AR5315_GPIO_DO,reg);
		(void)sysRegRead(AR5315_GPIO_DO); /* flush write to hardware */
		}
	if ( (unsigned int) data & GPIO_DIR) {
		sysRegWrite(AR5315_GPIO_CR,reg);
		(void)sysRegRead(AR5315_GPIO_CR); /* flush write to hardware */
		}
		
	return procfs_buffer_size;
}
 
static __init int register_proc(void)
{
         unsigned char i,flag=0;
		 char  proc_name[16];
		 
		 /* create directory gpio*/
		gpio_dir = proc_mkdir("gpio", NULL);
		if(gpio_dir == NULL) 
			goto fault;
		gpio_dir->owner = THIS_MODULE;
		 
		for (i=0; i < AR5315_NUM_GPIO * 3; i++) //create for every GPIO "x_in"," x_out" and "x_dir"
		{
			if ( i / AR5315_NUM_GPIO ==0){
				flag=GPIO_IN;
				sprintf(proc_name,"%i_in",i);
				}
			if ( i / AR5315_NUM_GPIO ==1){
				flag=GPIO_OUT;
				sprintf(proc_name,"%i_out",i % AR5315_NUM_GPIO);
				}
			if ( i / AR5315_NUM_GPIO ==2){
				flag=GPIO_DIR;
				sprintf(proc_name,"%i_dir",i % AR5315_NUM_GPIO);
				}
				
			proc_gpio = create_proc_entry(proc_name, S_IRUGO, gpio_dir);
			if (proc_gpio)	{
				proc_gpio->read_proc = gpio_proc_read;
				proc_gpio->write_proc = gpio_proc_write;
				proc_gpio->owner = THIS_MODULE;
				proc_gpio->data =( (i % AR5315_NUM_GPIO) | flag);
				} else
				goto fault;
				
		}
		
		proc_gpio = create_proc_entry("info", S_IRUGO, gpio_dir);
			if (proc_gpio)	{
				proc_gpio->read_proc = gpio_proc_info_read;
				proc_gpio->owner = THIS_MODULE;
				} else
				goto fault;	
		
		printk (KERN_NOTICE "gpio_proc: module loaded and /proc/gpio/ created\n");
			return 0;
		
		fault:
			cleanup_proc();
			return -EFAULT;	
}

static void cleanup_proc(void)
{
	unsigned char i;
	char  proc_name[16];
	
	for (i=0;i<AR5315_NUM_GPIO;i++)
	{
		sprintf(proc_name,"%i_in",i);
		remove_proc_entry(proc_name,gpio_dir);
		sprintf(proc_name,"%i_out",i);
		remove_proc_entry(proc_name,gpio_dir);
		sprintf(proc_name,"%i_dir",i);
		remove_proc_entry(proc_name,gpio_dir);
	}
	remove_proc_entry("info",gpio_dir);
	remove_proc_entry("gpio", NULL);
	printk(KERN_INFO "gpio_proc: unloaded and /proc/gpio/ removed\n");
	
}


module_init(register_proc);
module_exit(cleanup_proc);

MODULE_AUTHOR("olg");
MODULE_DESCRIPTION("AR5315 GPIO pins in /proc/gpio/");
MODULE_LICENSE("GPL");
