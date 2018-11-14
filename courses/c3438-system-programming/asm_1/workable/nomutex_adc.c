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

#define ADC_SET_CHANNEL		0xc000fa01
#define ADC_SET_ADCTSC		0xc000fa02

#define DEVICE_NAME "jahjaADC"
#define N_D         1           /*Number of Devices*/
#define S_N         1           /*The start minor number*/

typedef struct {
  // struct mutex lock;
  struct s3c_adc_client *client;
  int channel;
} ADC_DEV;

static int          major;
static dev_t        devno;
static struct cdev 	adc_cd;

static ADC_DEV adcdev;
static struct platform_device pd = {
  .name = "jahjaADC",
  .id = 7
};

/* 
 * ---Functions---
 */

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
  if (channel < 0 || channel > 9) return;
  adcdev.channel = channel;
}

static int op_adc_open(struct inode *inode, struct file *filp) {
  exynos_adc_set_channel(0); // init to 0
  printk("Device " DEVICE_NAME "open.\n");
  return 0;
}

static int op_adc_release(struct inode *inode, struct file *filp) {
  printk("Device " DEVICE_NAME " release.\n");
  return 0;
}

static ssize_t op_adc_read(
  struct file *filp, char *buffer,
  size_t count, loff_t *ppos
) {
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


static long op_adc_ioctl(
  struct file *file, unsigned int cmd,
  unsigned long arg
) {
  switch (cmd) {
    case ADC_SET_CHANNEL:
      exynos_adc_set_channel(arg);
      break;
    case ADC_SET_ADCTSC:
      break; /* No Touchscreen */
    default:
      return -EINVAL;
  }
  return 0;
}

static struct file_operations jahja_adc = {
  owner:    THIS_MODULE,
  open:	    op_adc_open,
  release:  op_adc_release,
  read:     op_adc_read,
  unlocked_ioctl:	op_adc_ioctl
};

static int __init jahja_adc_init(void) {
  int ret;

  adcdev.client = s3c_adc_register(&pd, NULL, NULL, 0);
  /*Register a major number*/
  ret = alloc_chrdev_region(&devno, S_N, N_D, DEVICE_NAME);
  if(ret < 0) {
    printk("Device " DEVICE_NAME " cannot get major number.\n");
    return ret;
  }

  major = MAJOR(devno);
  printk("Device " DEVICE_NAME " initialized (Major Number -- %d).\n", major);

  /*Register a char device*/
  cdev_init(&adc_cd, &jahja_adc);
  adc_cd.owner = THIS_MODULE;
  adc_cd.ops   = &jahja_adc;
  ret = cdev_add(&adc_cd, devno, N_D);

  if(ret) {
    printk("Device " DEVICE_NAME " register fail.\n");
    return ret;
  }
  return 0;
}

static void __exit jahja_adc_exit(void) {
  cdev_del(&adc_cd);
  unregister_chrdev_region(devno, N_D);
  s3c_adc_release(adcdev.client);

  printk("Device " DEVICE_NAME " unloaded.\n");
}

module_init(jahja_adc_init);
module_exit(jahja_adc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Darwin");
MODULE_DESCRIPTION("Char Driver Development: A/D Converter");
