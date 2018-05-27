#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define IODRIVER "iodriver"
#define MAX		 1000000

static int Delay;
static int Major;
static int numDevices = 0;
static char data[MAX] = "What";

static int device_open(struct inode *,struct file *);
static int device_release(struct inode *,struct file *);
static ssize_t device_read(struct file *,char *,size_t ,loff_t *);
static ssize_t device_write(struct file *,const char *,size_t,loff_t *);

static struct file_operations fops = 
{
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static int __init driver_init(void)
{	
	printk(KERN_ALERT "Loading the device ....\n");
	Major = register_chrdev(0, IODRIVER, &fops);
	
	if(Major < 0)
	{
		printk(KERN_INFO "Registering I/O Device failed with %d\n",Major);
		return Major;
	}

	printk(KERN_ALERT "Driver Registered with %d device id\n",Major);
	return 0;
}

static void __exit driver_cleanup(void)
{
	unregister_chrdev(Major, IODRIVER);
	printk(KERN_ALERT "CLeaning up the module...\n");
}

static int device_open(struct inode *inode_, struct file *file_)
{
	numDevices++;

	try_module_get(THIS_MODULE);

	return 0;
}

static int device_release(struct inode *inode_,struct file *file_)
{
	numDevices--;

	module_put(THIS_MODULE);

	return 0;
}


static ssize_t device_read(struct file *filp,char *buffer,size_t size, loff_t *offset)
{
	static int finished = 0;
	if(finished)
	{
		finished = 0;
		return 0;
	}
	char *ptrData = data;
	int bytesRead = 0;
	int dataLen = strlen(ptrData);
	while(dataLen && buffer != NULL)
	{
		if(put_user(*(ptrData++),buffer++))
		{
			return -EFAULT;
		}
		bytesRead++;
		dataLen--;
	}
	finished = 1;
	return bytesRead;
}


static ssize_t device_write(struct file *filp,const char *buffer,size_t size, loff_t *offset)
{
	char *ptrData = data;
	int bytesWritten = 0;
	int bufLen = strlen(buffer);
	Delay = 10;

	while(bufLen)
	{
		if(get_user(*(ptrData++), buffer++))
		{
			return -EFAULT;
		}
		bytesWritten++;
		bufLen--;
	}
	printk(KERN_ALERT "String received : %s\n",data);
	return bytesWritten;
}


module_init(driver_init);
module_exit(driver_cleanup);
