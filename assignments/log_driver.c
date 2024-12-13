#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // For copy_to_user and copy_from_user
#include <linux/time.h>
#include <linux/semaphore.h>
#include<linux/delay.h>


#define DEVICE_NAME "log_device"
#define BUFFER_SIZE 1024

typedef struct log_info{
char *msg;
char *time;
struct log_info *next;
}log;


static int major_number;
static char device_buffer[BUFFER_SIZE];
static int open_count = 0;
static int count=0;
static log *head=0;
static struct semaphore sem;

// Function prototypes for device operations
static int device_open(struct inode *inode, struct file *file);
static int device_close(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset);
static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset);



// File operations structure
static struct file_operations fops = {
    .open = device_open,                        //function pointer to open the device 
    .release = device_close,			//function pointer to close device
    .read = device_read,			//function pointer to read from user space
    .write = device_write,			//function pointer to write to user space
};

// Called when the device is opened
static int device_open(struct inode *inode, struct file *file) {
    open_count++;
    printk(KERN_INFO "simple_device: Device opened %d time(s)\n", open_count);
    return 0;
}

// Called when the device is closed
static int device_close(struct inode *inode, struct file *file) {
    printk(KERN_INFO "simple_device: Device closed\n");
    return 0;
}

// Called when data is read from the device
static ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset) {
    int i=0,j=0;
    log *temp=head;
    size_t len = strlen(device_buffer);

    if (*offset >= len) // Check if all data has been read
        return 0;

    if (size > len - *offset) // Adjust size to avoid reading beyond buffer
        size = len - *offset;
    
    printk("size: %lu\n",size);
    if(down_interruptible(&sem))
    {
	printk("semaphore not available\n");
	return -ERESTARTSYS;
    }

    while(temp!=head || i==0)
    {
	    i++;
	printk("debug: %s\n",temp->msg);
	temp=temp->next;
    }

    temp=head;
    i=0;

    if(head!=0)
    {
	while(temp!=head || i==0){
		strcpy(device_buffer+i+j, temp->msg);
		i=i+strlen(temp->msg);
		j++;
		device_buffer[i+j-1]='\n';
		temp=temp->next;
//		mdelay(5000);
	} 
    }
    device_buffer[i+j]=0;
    
    if (copy_to_user(user_buffer, device_buffer, size)) {
        return -EFAULT;
    }
    up(&sem);

    *offset += size;
    printk(KERN_INFO "simple_device: Sent %zu bytes to the user\n", size);
    return size;
}

// Called when data is written to the device
static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset) {
//    ktime_t e_time;
//    struct tm h_time;
    log *new=0,*temp=0,*del=0;

    if (size > BUFFER_SIZE - 1) // Limit size to buffer capacity
        size = BUFFER_SIZE - 1;

    if(down_interruptible(&sem))
    {
	printk("semaphore not available\n");
	return -ERESTARTSYS;
    }

    if (copy_from_user(device_buffer, user_buffer, size)) {
        return -EFAULT;
    }

    device_buffer[size] = '\0'; // Null-terminate the string
    
//    e_time=ktime_get_real();
//    time_t t=e_time;
//    time_to_tm(t,0,&h_time);

    new=kmalloc(sizeof(log),GFP_KERNEL);
    if(head==0)
    {
	    head=new;
	    new->next=head;
	    count++;
    }
    else
    {
	    temp=head;
	    while(temp->next!=head)
		    temp=temp->next;
	    if(count>=5)
	    {
	    	del=head;
	    	head=head->next;
		kfree(del);
	    }
	    temp->next=new;
	    new->next=head;
	    count++;
    }
  //  mdelay(9000);
    up(&sem);

    new->msg=kmalloc(strlen(device_buffer)+1,GFP_KERNEL);
    strcpy(new->msg,device_buffer);

    printk(KERN_INFO "simple_device: Received %zu bytes from the user\n", size);
    return size;
}

// Module initialization
static int __init simple_driver_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "simple_device: Failed to register device\n");
        return major_number;
    }
    sema_init(&sem,1);
    printk(KERN_INFO "simple_device: Registered with major number %d\n", major_number);
    return 0;
}

// Module cleanup
static void __exit simple_driver_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "simple_device: Unregistered device\n");
}

module_init(simple_driver_init);
module_exit(simple_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("A Simple Linux Device Driver");
