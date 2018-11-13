#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-adc.h>
#include <mach/regs-gpio.h>
#include <plat/regs-timer.h> // check this one

#undef DEBUG
#ifdef DEBUG
#define DPRINTK(x...) {printk(__FUNCTION__"(%d): ",__LINE__);printk(##x);}
#else
#define DPRINTK(x...) (void)(0)
#endif

#define DEVICE_NAME "jahja-adc"

#define N_D 1  /*Number of Devices*/
#define S_N 1  /*The start minor number*/

static int          major;
static dev_t        devno;
static struct cdev  jahja_adc;

//////

static void __iomem *base_addr; ///

typedef struct {
        wait_queue_head_t wait;
        int channel;
        int prescale;
} ADC_DEV;

static int __ADC_locked = 0;
static ADC_DEV adcdev;
static volatile int ev_adc = 0;
static int adc_data;
static struct clk        *adc_clock;

#define __ADCREG(name)        (*(volatile unsigned long *)(base_addr + name))
#define ADCCON                  __ADCREG(S3C_ADCCON)        // ADC control
#define ADCTSC                  __ADCREG(S3C_ADCTSC)        // ADC touch screen control
#define ADCDLY                  __ADCREG(S3C_ADCDLY)        // ADC start or Interval Delay
#define ADCDAT0                 __ADCREG(S3C_ADCDAT0)       // ADC conversion data 0
#define ADCDAT1                 __ADCREG(S3C_ADCDAT1)       // ADC conversion data 1
#define ADCUPDN                 __ADCREG(S3C_ADCUPDN)       // Stylus Up/Down interrupt status

#define PRESCALE_DIS                (0 << 14)
#define PRESCALE_EN                 (1 << 14)
#define PRSCVL(x)                   ((x) << 6)
#define ADC_INPUT(x)                ((x) << 3)
#define ADC_START                   (1 << 0)
#define ADC_ENDCVT                  (1 << 15)

#define START_ADC_AIN(ch, prescale) \
    do { \
            ADCCON = PRESCALE_EN | PRSCVL(prescale) | ADC_INPUT((ch)) ; \
            ADCCON |= ADC_START; \
    } while (0)


static irqreturn_t adcdone_int_handler(int irq, void *dev_id) {
#if 1
    if (__ADC_locked) {
        adc_data = ADCDAT0 & 0x3ff;

        ev_adc = 1;
        wake_up_interruptible(&adcdev.wait);

        /* clear interrupt */
        __raw_writel(0x0, base_addr + S3C_ADCCLRINT);
    }
#endif

    return IRQ_HANDLED;
}

static ssize_t ok210_adc_read(struct file *filp, char *buffer, size_t count, loff_t *ppos) {
    char str[20];
    int value;
    size_t len;

    __ADC_locked = 1;
    START_ADC_AIN(adcdev.channel, adcdev.prescale);
    wait_event_interruptible(adcdev.wait, ev_adc);
    ev_adc = 0;
    DPRINTK("AIN[%d] = 0x%04x, %d\n", adcdev.channel, adc_data, ADCCON & 0x80 ? 1:0);
    value = adc_data;
    __ADC_locked = 0;

    len = sprintf(str, "%d\n", value);
    if (count >= len) {
            int r = copy_to_user(buffer, str, len);
            return r ? r : len;
    } else {
            return -EINVAL;
    }
}

static int ok210_adc_open(struct inode *inode, struct file *filp)
{
        init_waitqueue_head(&(adcdev.wait));

        adcdev.channel=0;
        adcdev.prescale=0xff;

        DPRINTK("adc_tt opened\n");
        return 0;
}

static int ok210_adc_release(struct inode *inode, struct file *filp)
{
        DPRINTK("adc_tt closed\n");
        return 0;
}




//////////////////








static struct file_operations dev_fops = {
        owner:        THIS_MODULE,
        open:        ok210_adc_open,
        read:        ok210_adc_read,
        release:        ok210_adc_release,
};

static struct miscdevice misc = {
        .minor        = MISC_DYNAMIC_MINOR,
        .name        = "adc_tt",        //DEVICE_NAME,
        .fops        = &dev_fops,
};

static int __init dev_init(void)
{
        int ret;

        base_addr = ioremap(SAMSUNG_PA_ADC, 0x20); // plat/adc
        if (base_addr == NULL) {
                printk(KERN_ERR "Failed to remap register block\n");
                return -ENOMEM;
        }

        adc_clock = clk_get(NULL, "adc_tt");
        if (!adc_clock) {
                printk(KERN_ERR "failed to get adc_tt clock source\n");
                return -ENOENT;
        }
        clk_enable(adc_clock);

        /* normal ADC */
        ADCTSC = 0;

#if        1
        ret = request_irq(IRQ_ADC, adcdone_int_handler, IRQF_SHARED, DEVICE_NAME, &adcdev);
        if (ret) {
                printk("request IRQ %d failed for adc_tt, %d\n", IRQ_ADC, ret);
                iounmap(base_addr);
                return ret;
        }
#endif

        ret = misc_register(&misc);

        printk (DEVICE_NAME"\tinitialized\n");
        return ret;
}

static void __exit dev_exit(void)
{
        free_irq(IRQ_ADC, &adcdev);
        iounmap(base_addr);

        if (adc_clock) {
                clk_disable(adc_clock);
                clk_put(adc_clock);
                adc_clock = NULL;
        }

        misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gjianw217@163.com");
MODULE_DESCRIPTION("ADC driver");