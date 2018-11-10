#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#include <plat/adc.h>

#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#define DPRINTK(x...) {printk(__FUNCTION__"(%d): ",__LINE__);printk(##x);}
#else
#define DPRINTK(x...) (void)(0)
#endif

#define DEVICE_NAME	"little-adc"
#define N_D 1  /*Number of Devices*/
#define S_N 1  /*The start minor number*/

static int          major;
static dev_t        devno;
static struct cdev  jahja_adc;

typedef struct {
	struct mutex lock;
	int channel;
} ADC_DEV;

static ADC_DEV adcdev;

static inline int exynos_adc_read_ch(void) {
	int ret;

	ret = mutex_lock_interruptible(&adcdev.lock);
	if (ret < 0)
		return ret;

	ret = s3c_adc_read(adcdev.client, adcdev.channel);
	mutex_unlock(&adcdev.lock);

	return ret;
}

static inline void exynos_adc_set_channel(int channel) {
	if (channel < 0 || channel > 9) return;
	adcdev.channel = channel;
}

static ssize_t exynos_adc_read(struct file *filp, char *buffer,
		size_t count, loff_t *ppos) {

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

static long exynos_adc_ioctl(struct file *file,
		unsigned int cmd, unsigned long arg) {

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

static int exynos_adc_open(struct inode *inode, struct file *filp) {
	exynos_adc_set_channel(0);

	DPRINTK("adc opened\n");
	return 0;
}

static int exynos_adc_release(struct inode *inode, struct file *filp) {
	DPRINTK("adc closed\n");
	return 0;
}

static struct file_operations adc_dev_fops = {
	owner:	THIS_MODULE,
	open:	exynos_adc_open,
	read:	exynos_adc_read,	
	unlocked_ioctl:	exynos_adc_ioctl,
	release:	exynos_adc_release,
};

static int __init exynos_adc_init(void)
{
  int ret;

  mutex_init(&adcdev.lock);


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
  cdev_init(&jahja_adc, &adc_dev_fops);
  jahja_adc.owner = THIS_MODULE;
  jahja_adc.ops   = &adc_dev_fops;
  ret = cdev_add(&jahja_adc, devno, N_D);
  if(ret)
  {
    printk("Device " DEVICE_NAME " register fail.\n");
    return ret;
  }

  return 0;
}

static void __exit exynos_adc_exit(void)
{
  cdev_del(&jahja_adc);
  unregister_chrdev_region(devno, N_D);
  printk("Device " DEVICE_NAME " unloaded.\n");
}

module_init(exynos_adc_init);
module_exit(exynos_adc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FFFFFFFF");
