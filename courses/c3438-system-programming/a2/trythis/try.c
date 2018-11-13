#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>

//使用宋宝华推荐的普通字符设备框架
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/ctype.h>
#include <linux/pagemap.h>
#include <linux/device.h>
#include <asm/io.h> 
#include <mach/regs-gpio.h>
#include <mach/regs-irq.h>
#include <linux/wait.h>  
#include <asm/uaccess.h>  
#include <mach/regs-clock.h>  
#include <plat/regs-timer.h>  
#include <linux/clk.h>

#include <plat/regs-adc.h>  
#include <linux/sched.h>
#include <linux/irq.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include<linux/slab.h>

// #undef DEBUG
// //#define  DEBUG
// #ifdef DEBUG
// #define DPRINTK printk
// #else
// #define DPRINTK(x...)
// #endif

#undef DEBUG
#ifdef DEBUG
#define DPRINTK(x...) {printk(__FUNCTION__"(%d): ",__LINE__);printk(##x);}
#else
#define DPRINTK(x...) (void)(0)
#endif

static void __iomem *base_addr;

/*定义ADC 相关的寄存器*/ 
#define ADCCON     (*(volatile unsigned long *)(base_addr + 0x00))  //ADC control  
#define ADCTSC     (*(volatile unsigned long *)(base_addr + 0x04))  //ADC touch screen control  
#define ADCDLY     (*(volatile unsigned long *)(base_addr + 0x08))  //ADC start or Interval Delay  
#define ADCDAT0    (*(volatile unsigned long *)(base_addr + 0x0c))  //ADC conversion data 0  
#define ADCDAT1    (*(volatile unsigned long *)(base_addr + 0x10))  //ADC conversion data 1  
#define ADCUPDN    (*(volatile unsigned long *)(base_addr + 0x14))  //Stylus Up/Down interrupt status  
#define PRESCALE_DIS    (0 << 14)  
#define PRESCALE_EN     (1 << 14)  
#define PRSCVL(x)       ((x) << 6)
#define ADC_INPUT(x)    ((x) << 3)
#define ADC_START       (1 << 0)  
#define ADC_ENDCVT      (1 << 15)  

// 定义“开启AD 输入”宏，因为比较简单，故没有做成函数  
#define  START_ADC_AIN(ch, prescale) \
    do{ \
        ADCCON = PRESCALE_EN | PRSCVL(prescale) | ADC_INPUT((ch)) ; \
        ADCCON |= ADC_START; \
    }while(0)

#define ADC_MAJOR 249  //静态分配
// #define ADC_MAJOR 0 //动态分配
#define ADC_MINOR 0


int devmajor =   ADC_MAJOR;
int devminor =   ADC_MINOR;
dev_t dev = 0;

//设备结构
struct ADC_dev {
    struct cdev cdev;   /* Char device structure*/ 
    wait_queue_head_t wait; 
    int channel; 
    int prescale; 
};

struct ADC_dev *ADC_devices;/*设备结构体指针*/
struct class  *adc_lass;
static int  OwnADC = 0;    //;ADC 驱动是否拥有A/D 转换器资源的状态变量  
static volatile int ev_adc = 0; 
static int  adc_data; 
static struct clk *adc_clock;   
DEFINE_SEMAPHORE(ADC_LOCK);  //;声明全局信号量，以便和触摸屏驱动程序共享A/D 转换器

/*******************************************************
与内核交互函数实现
*******************************************************/
//;ADC 中断处理函数  
static irqreturn_t adcdone_int_handler(int irq, void *dev_id) { 
    //;如果ADC 驱动拥有“A/D 转换器”资源，则从ADC 寄存器读取转换结果 
    if (OwnADC) { 
    adc_data = ADCDAT0 & 0x3ff; 
    ev_adc = 1; 
    wake_up_interruptible(&(ADC_devices->wait)); 
    } 
    return IRQ_HANDLED; 
}

int ADC_open(struct inode *inode, struct file *filp) {
    //打开文件file的私有数据指针指向设备结构体指针
    filp->private_data=ADC_devices;
    init_waitqueue_head(&(ADC_devices->wait)); //;初始化中断队列  
    ADC_devices->channel=0;  //;缺省通道为“0”  
    ADC_devices->prescale=0xff; 
    //DPRINTK("open successfully!!\n");
    return 0;
}
int ADC_release(struct inode *inode, struct file *filp) { 
    return 0;
}

//使用ADC_write来替代ADC_ioctl
ssize_t ADC_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    return 0;
}

ssize_t ADC_read(struct file *filp,char __user *buff,size_t size,loff_t *offp) {
    int value; 
    //;判断“A/D 转换器”资源是否可用 
    if (down_trylock(&ADC_LOCK) == 0) { 
        OwnADC = 1;    //标记“A/D 转换器”资源状态为可用 
        
        START_ADC_AIN(ADC_devices->channel, ADC_devices->prescale); //开始转换 
        wait_event_interruptible(ADC_devices->wait, ev_adc); //通过终端的方式等待转换结果  
        ev_adc = 0; 
        DPRINTK("AIN[%d] = 0x%04x, %d\n", ADC_devices->channel, adc_data, ADCCON & 0x80 ? 1:0); 
        
        value = adc_data; //;把转换结果赋予value，以便传递到用户层/应用层  
        
        OwnADC = 0;   //;释放“A/D 转换器”资源 
        up(&ADC_LOCK); 

    } else { 
        value = -1;   //;没有“A/D 转换器”资源，赋值为“-1”  
    }

    if( copy_to_user(buff, &value, size)) {
        printk("copy error!!");
        return -1;
    } //;把转换结果传递到用户层/应用层 
    return 0; 
}
/*     文件操作结构体   */
struct file_operations ADC_fops = {
    .owner = THIS_MODULE,
    .open = ADC_open,
    .write = ADC_write,
    .read =  ADC_read,
    .release=ADC_release,
};

/*******************************************************
MODULE ROUTINE
*******************************************************/

void adc_exit_module(void) {
    //2 <1>释放中断

    free_irq(IRQ_ADC,ADC_devices);
    //2 <2>删除KEY基址映射
    iounmap(base_addr);

    if (adc_clock) { 
        clk_disable(adc_clock); 
        clk_put(adc_clock); 
        adc_clock = NULL; 
    } 
    //2 <3>从内核中删除设备
    if (ADC_devices) {
        cdev_del(&ADC_devices->cdev);//从系统中移除一个字符设备
        kfree(ADC_devices);//释放空间
    }
    //2 <4>删除节点
    device_destroy(adc_lass, MKDEV(devmajor, 0));         //delete device node under /dev
    //2 <5>释放节点空间
    class_destroy(adc_lass);                               //delete class created by us
    //2 <6>释放设备号
    unregister_chrdev_region(dev,1);// 6 释放设备编号

}

int adc_init_module(void) {
    int result;
    int ret;
    //2 <1>申请设备号(静态 动态)
    if(devmajor) { //3 a.静态申请设备号
        dev = MKDEV(devmajor, devminor); // 1 获得设备号
        result = register_chrdev_region(dev, 1, "adc_driver"); // 2 分配设备编号
    } else { //3  b.动态分配设备编号
        result = alloc_chrdev_region(&dev, devminor, 1, "adc_driver");
        devmajor = MAJOR(dev);
    }

    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", devmajor);
        return result;
    }
    //2 <2>给设备分配空间
    printk(KERN_WARNING "led get major: %d\n", devmajor);
    ADC_devices = kmalloc(sizeof(struct ADC_dev), GFP_KERNEL);//分配内存给本设备结构体

    if (!ADC_devices){ //空间分配失败
        result = -ENOMEM;
        goto fail;
    }

    memset(ADC_devices, 0, sizeof(struct ADC_dev));
    //2 <3>在内核中注册设备
    cdev_init(&ADC_devices->cdev, &ADC_fops);
    //2 <4>初始化设备
    ADC_devices->cdev.owner = THIS_MODULE;
    ADC_devices->cdev.ops = &ADC_fops;
    //2 <5>添加设备到内核中
    result = cdev_add (&ADC_devices->cdev, dev, 1);// 4 把本设备放内核中

    if(result) { //添加失败
        printk(KERN_NOTICE "Error %d adding KEY\n", result);
        goto fail;
    }
    //2 <6>申请中断 if fail, try IRQ_ADC+1
    ret = request_irq(IRQ_ADC, adcdone_int_handler, IRQF_SHARED, "adc", ADC_devices);
    if (ret) {
        printk("request button irq failed!\n");
        return ret;
    }
    //3 <7>GP与timer 的基地址映射
    base_addr=ioremap(0x58000000,0x18); // suspicious
    if(!base_addr) {
        printk("Err: failed in creating timer_base.\n");
        result = -ENOMEM;
        goto fail;
        return -1;
    }
    //2 <8>获取时钟
    adc_clock = clk_get(NULL, "adc"); 
    if (!adc_clock) { 
        printk(KERN_ERR "failed to get adc clock source\n"); 
        return -ENOENT;
    } 
    clk_enable(adc_clock); 
    //2 <9>跟据需要创建节点
    adc_lass = class_create(THIS_MODULE, "adc_lass");
    if(IS_ERR(adc_lass)) {
        printk("Err: failed in creating class.\n");
        return -1;
    }
    device_create(adc_lass, NULL, MKDEV(devmajor, 0),  NULL, "adc_driver");

    return 0;
    
    fail:
    adc_exit_module();
    return result;
}

module_init(adc_init_module);
module_exit(adc_exit_module);
MODULE_AUTHOR("hui 2012.11.2 23:04");
MODULE_LICENSE("GPL");