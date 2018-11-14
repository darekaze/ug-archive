#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <asm/uaccess.h>
#include <plat/adc.h>

#define DEVICE_NAME "aadc"
#define N_D         1                   /*Number of Devices*/
#define S_N         1                   /*The start minor number*/

static int          major;
static dev_t        devno;
static struct cdev  dev_aadc;

typedef struct {
	// struct mutex lock;
	struct s3c_adc_client *client;
	int channel;
} ADC_DEV;

static ADC_DEV adcdev;

struct platform_device plt = {
  .name = "aadc",
  .id = "1"
};

static inline int exynos_adc_read_ch(void) {
	int ret;

	// ret = mutex_lock_interruptible(&adcdev.lock);
	// if (ret < 0)
	// 	return ret;

	ret = s3c_adc_read(adcdev.client, adcdev.channel);
	// mutex_unlock(&adcdev.lock);

	return ret;
}

static inline void exynos_adc_set_channel(int channel) {
	if (channel < 0 || channel > 9)
		return;

	adcdev.channel = channel;
}

static long exynos_adc_ioctl(struct file *file,
		unsigned int cmd, unsigned long arg)
{
#define ADC_SET_CHANNEL		0xc000fa01
#define ADC_SET_ADCTSC		0xc000fa02

	switch (cmd) {
		case ADC_SET_CHANNEL:
			exynos_adc_set_channel(arg);
			break;
		case ADC_SET_ADCTSC:
			/* do nothing */
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

static int exynos_adc_open(struct inode *inode, struct file *filp)
{
	exynos_adc_set_channel(0);

	// DPRINTK("adc opened\n");
	return 0;
}

static ssize_t exynos_adc_read(struct file *filp, char *buffer,
		size_t count, loff_t *ppos)
{
	char str[20];
	int value;
	size_t len;

	value = exynos_adc_read_ch();

	len = sprintf(str, "%d\n", value);
	if (count >= len) {
		int r = copy_to_user(buffer, str, len);
		return r ? r : len;
	} else {
		return -EINVAL;
	}
}

static int exynos_adc_release(struct inode *inode, struct file *filp)
{
	// DPRINTK("adc closed\n");
	return 0;
}


static struct file_operations zili_demo_fops = {
	owner:   THIS_MODULE,
	open:	exynos_adc_open,
	read:    exynos_adc_read,
  unlocked_ioctl:	exynos_adc_ioctl,
	release: exynos_adc_release
};

static int __init zili_demo_char1_init(void) {
	int ret;

  adcdev.client = s3c_adc_register(&plt, NULL, NULL, 0);

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
	cdev_init(&dev_aadc, &zili_demo_fops);
	dev_aadc.owner = THIS_MODULE;
	dev_aadc.ops   = &zili_demo_fops;
	ret = cdev_add(&dev_aadc, devno, N_D);
	if(ret)
	{
		printk("Device " DEVICE_NAME " register fail.\n");
		return ret;
	}
	return 0;
}

static void __exit zili_demo_char1_exit(void) {
	cdev_del(&dev_aadc);
	unregister_chrdev_region(devno, N_D);
  s3c_adc_release(adcdev.client);

	printk("Device " DEVICE_NAME " unloaded.\n");
}

module_init( zili_demo_char1_init);
module_exit( zili_demo_char1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dr. Zili Shao <cszlshao@comp.polyu.edu.hk>");
MODULE_DESCRIPTION("Char Driver Development: Hello World. Course: HK POLYU COMP 3438");
