#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/gpio.h>
#include<linux/interrupt.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/workqueue.h>

#define DEVICE_NAME "work_drivr"
#define GPIO_SW 529

static int IRQ_NO;
static dev_t dev;
static struct cdev cdev_t;
static struct class *clas;

void work_fun(struct work_struct *work);

DECLARE_WORK(work_q,work_fun);

void work_fun(struct work_struct *work){
	pr_info("workqueue executed\n");
}

static irqreturn_t irq_handler(int irq,void *dev_id)
{
	pr_info("interrupt triggered\n");
	schedule_work(&work_q);

	return IRQ_HANDLED;
}


static int __init work_init(void){
	if(alloc_chrdev_region(&dev,0,1,DEVICE_NAME)){
		pr_info("cannot allocate major number and minor number\n");
		return -1;
	}

	cdev_init(&cdev_t,NULL);

	if(cdev_add(&cdev_t,dev,1)<0){
		pr_info("cannot register the driver with kernel\n");
		goto r_class;
	}

	if(IS_ERR(clas=class_create(DEVICE_NAME))){
		pr_info("unable to create class");
		goto r_class;
	}

	if(IS_ERR(device_create(clas,NULL,dev,NULL,DEVICE_NAME))){
		pr_info("Unable to create device\n");
		goto r_device;
	}

        IRQ_NO=gpio_to_irq(GPIO_SW);
	if(IRQ_NO<0){
		pr_info("gpio interrupt not allocated\n");
		gpio_free(GPIO_SW);
		goto r_device;
	}	

	if(request_irq(IRQ_NO,irq_handler,IRQF_SHARED,DEVICE_NAME,NULL)){
		pr_info("cannot register irq\n");
		goto r_irq;
	}
	return 0;

r_irq:
	free_irq(IRQ_NO,irq_handler);
r_device:
	class_destroy(clas);
r_class:
	unregister_chrdev_region(dev,1);
	cdev_del(&cdev_t);
return -1;
}

static void __exit work_exit(void){
	gpio_free(GPIO_SW);
	free_irq(IRQ_NO,irq_handler);
	class_destroy(clas);
	unregister_chrdev_region(dev,1);
	cdev_del(&cdev_t);
	pr_info("Workqueue driver unloaded successfully\n");	
}

module_init(work_init);
module_exit(work_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHARATH");
MODULE_DESCRIPTION("Simple workqueue driver");

