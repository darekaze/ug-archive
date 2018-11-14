#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#define DEVICE_NAME "comp3438_helloworld"
#define N_D         1                   /*Number of Devices*/
#define S_N         1                   /*The start minor number*/

static char         msg[]= "Hello World!!!";
static int          major;
static dev_t        devno;
static struct cdev  dev_lab4;

static int zili_demo_char1_open(struct inode *inode, struct file *fp)
{
	printk("Device " DEVICE_NAME "open.\n");
	return 0;
}

static ssize_t zili_demo_char1_read(struct file *fp, char *buf, size_t count, loff_t *position)
{
	int  num;
	int  ret;

	if( count < strlen(msg) )
		num = count;
	else
		num = strlen(msg);

	ret = copy_to_user(buf, msg, num );
	if(ret)
	{
		printk("Fail to copy data from the kernel space to the user space!\n");
	}
	return num;
}

static int zili_demo_char1_release(struct inode *inode, struct file *fp)
{
	printk("Device " DEVICE_NAME " release.\n");
	return 0;
}

static struct file_operations zili_demo_fops = {
	owner:   THIS_MODULE,
	open:    zili_demo_char1_open,
	read:    zili_demo_char1_read,
	release: zili_demo_char1_release,
};

static int __init zili_demo_char1_init(void)
{
	int   ret;
	/*Register a major number*/
	ret = alloc_chrdev_region(&devno, S_N, N_D, DEVICE_NAME);
	if(ret < 0)
	{
		printk("Device " DEVICE_NAME " cannot get major number.\n");
		return ret;
	}

	major = MAJOR(devno);
	printk("Device " DEVICE_NAME " initialized (Major Number -- %d).\n", major);

	/*Register a char device*/
	cdev_init(&dev_lab4, &zili_demo_fops);
	dev_lab4.owner = THIS_MODULE;
	dev_lab4.ops   = &zili_demo_fops;
	ret = cdev_add(&dev_lab4, devno, N_D);
	if(ret)
	{
		printk("Device " DEVICE_NAME " register fail.\n");
		return ret;
	}
	return 0;
}

static void __exit zili_demo_char1_exit(void)
{
	cdev_del(&dev_lab4);
	unregister_chrdev_region(devno, N_D);
	printk("Device " DEVICE_NAME " unloaded.\n");
}

module_init( zili_demo_char1_init);
module_exit( zili_demo_char1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dr. Zili Shao <cszlshao@comp.polyu.edu.hk>");
MODULE_DESCRIPTION("Char Driver Development: Hello World. Course: HK POLYU COMP 3438");
