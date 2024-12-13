#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>//file_operations structure is present this header file
#include <linux/uaccess.h> // For copy_to_user and copy_from_user

#define DEVICE_NAME "simple_device"
#define BUFFER_SIZE 1024

static int major_number;
static char device_buffer[BUFFER_SIZE];
static int open_count = 0;
int num1=0,num2=0;

int i=0,j,k,res=0,rev=0;
char s[20],p[10],op[10];
int strcmp1(char *,char *);
int my_atoi(char *);
// Function prototypes for device operations
static int device_open(struct inode *inode, struct file *file);
static int device_close(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset);
static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset);

// File operations structure
static struct file_operations fops = {
    .open = device_open,//function is assigned to open function pointer
    .release = device_close,
    .read = device_read,
    .write = device_write,
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
    size_t len = strlen(device_buffer);

    if (*offset >= len) // Check if all data has been read
        return 0;

    if (size > len - *offset) // Adjust size to avoid reading beyond buffer
        size = len - *offset;

    if (copy_to_user(user_buffer, device_buffer + *offset, size)) {
        return -EFAULT;
    }

    *offset += size;
    printk(KERN_INFO "simple_device: Sent %zu bytes to the user\n", size);
    return size;
}

// Called when data is written to the device
static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset) {
    if (size > BUFFER_SIZE - 1) // Limit size to buffer capacity
        size = BUFFER_SIZE - 1;

    if (copy_from_user(device_buffer, user_buffer, size)) {
        return -EFAULT;
    }
    
    device_buffer[size] = '\0'; // Null-terminate the string
    printk(KERN_INFO "simple_device: Received %zu bytes from the user\n", size);
    
    /*for(i=0;device_buffer[i];i++)
    doub_num=doub_num*10+(device_buffer[i]-48);//converting string into num*/
   //doubling the number
    /*doub_num=(doub_num*2);*/
    //sqauring 
//    doub_num=doub_num*doub_num; 
/*    for(;doub_num;doub_num=doub_num/10)
	    rev=rev*10+doub_num%10;//reversing number

    for(i=0;rev;rev=rev/10)
	    device_buffer[i++]=rev%10+48;//converting into string*/
					 //calculator 
    for(i=0,j=0;device_buffer[i];i++,j++)
    {
	    if(device_buffer[i]==',')
		    break;
	    s[j]=device_buffer[i];
    }
    s[j]=0;
    printk(KERN_INFO "simple_device: s=%s\n", s);
    num1=my_atoi(s);

    printk(KERN_INFO "simple_device: num1=%d\n", num1);
    for(i=i+1,j=0;device_buffer[i];i++,j++)
    {
	    if(device_buffer[i]==',')
		    break;
	    p[j]=device_buffer[i];
    }
    p[j]=0;
    printk(KERN_INFO "simple_device: p=%s\n", p);
    
    num2=my_atoi(p);
    printk(KERN_INFO "simple_device:num2=%d\n", num2);
    for(i=i+1,j=0;device_buffer[i];i++)
    {
	    op[j++]=device_buffer[i];
    }
    op[j]=0;
    printk(KERN_INFO "simple_device: option%s\n", op);
    if(strcmp1(op,"add"))
    {
	    res=num1+num2;
    }
    else if(strcmp1(op,"sub"))
    {
	    res=num1-num2;
	    if(res<0)
		    res=-res;
    }
    else if(strcmp1(op,"mul"))
    {
	    res=num1*num2;
    }
    else if(strcmp1(op,"div"))
    {
	    res=num1/num2;
    }

    printk(KERN_INFO "simple_device: Result=%d\n", res);
  // for(rev=0;res;res=res/10)
  //rev=rev*10+res%10;//reversing number
    char t;
    int l;
    for(i=0;res;res=res/10)
	    device_buffer[i++]=res%10+48;//converting into string
    device_buffer[i]=0;
    for(j=0,l=i-1;j<l;j++,l--)
    {
	    t=device_buffer[j];
	    device_buffer[j]=device_buffer[l];
	    device_buffer[l]=t;
    }
    return size;
}
int strcmp1(char *s,char *s1)
{
	int i;
	for(i=0;s[i];i++)
	{
		if(s[i]!=s1[i])
			break;
	}
	if(s[i]==s1[i])
	return 1;
	else
	return 0;
}
int my_atoi(char *p)
{
  int n=0;
    for(k=0;p[k];k++)
	    n=n*10+(p[k]-48);//converting string into num
			     return n;
}

// Module initialization
static int __init simple_driver_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "simple_device: Failed to register device\n");
        return major_number;
    }
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
