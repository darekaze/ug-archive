#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "comp3438_4_leds"
#define LED_ON      0
#define LED_OFF     1
#define LED1 	    S5PV210_GPJ2(0)
#define LED2	    S5PV210_GPJ2(1)
#define LED3	    S5PV210_GPJ2(2)
#define LED4	    S5PV210_GPJ2(3)
#define N_D         4                   /*Number of Devices*/
#define S_N         1                   /*The start minor number*/

static unsigned int LED[]= {LED1, LED2, LED3, LED4};
static int          major;
static int          minor;
static dev_t        devno;
static struct cdev  dev_lab5b;

static int zili_demo_char_led_open(struct inode *inode, struct file *fp)
{
  minor = MINOR(inode->i_rdev);

  printk("Device " DEVICE_NAME " open (Minor=%d).\n", minor);
  return 0;
}

static ssize_t zili_demo_char_led_write(struct file *fp, const char *buf, size_t count, loff_t *position)
{
  char led_status;
  int  ret;

  ret = copy_from_user(&led_status, buf, sizeof(led_status) );
  if(ret)
  {
    printk("Fail to copy data from the user space to the kernel space!\n");
  }

  printk("Write: led = 0x%x, count = %d.\n", led_status, count);

  if( led_status > '0' )
    gpio_set_value(LED[minor-S_N], LED_ON);
  else
    gpio_set_value(LED[minor-S_N], LED_OFF);

  return sizeof(led_status);
}

static int zili_demo_char_led_release(struct inode *inode, struct file *fp)
{
  printk("Device " DEVICE_NAME " release.\n");
  return 0;
}

static struct file_operations zili_demo_fops = {
  owner:   THIS_MODULE,
  open:    zili_demo_char_led_open,
  write:   zili_demo_char_led_write,
  release: zili_demo_char_led_release,
};

static int __init zili_demo_char_led_init(void)
{
  int ret;

  s3c_gpio_cfgpin(LED1, S3C_GPIO_OUTPUT);
  gpio_set_value (LED1, LED_OFF);
  s3c_gpio_cfgpin(LED2, S3C_GPIO_OUTPUT);
  gpio_set_value (LED2, LED_OFF);
  s3c_gpio_cfgpin(LED3, S3C_GPIO_OUTPUT);
  gpio_set_value (LED3, LED_OFF);
  s3c_gpio_cfgpin(LED4, S3C_GPIO_OUTPUT);
  gpio_set_value (LED4, LED_OFF);

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
  cdev_init(&dev_lab5b, &zili_demo_fops);
  dev_lab5b.owner = THIS_MODULE;
  dev_lab5b.ops   = &zili_demo_fops;
  ret = cdev_add(&dev_lab5b, devno, N_D);
  if(ret)
  {
    printk("Device " DEVICE_NAME " register fail.\n");
    return ret;
  }

  return 0;
}

static void __exit zili_demo_char_led_exit(void)
{
  cdev_del(&dev_lab5b);
  unregister_chrdev_region(devno, N_D);
  printk("Device " DEVICE_NAME " unloaded.\n");

}

module_init( zili_demo_char_led_init);
module_exit( zili_demo_char_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dr. Zili Shao <cszlshao@comp.polyu.edu.hk>");
MODULE_DESCRIPTION("Char Driver Development: 4 LEDs On/Off \
                    with Major/Minor Numbers. Course: HK POLYU COMP 3438");
