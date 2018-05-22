#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define IODRIVER "iodriver"
#define MAX		 100

static int Delay;
static int Major;
static int numDevices = 0;
static char data[MAX];

static int (*device_open)(struct inode *,struct file *);
static int (*device_release)(struct inode *,struct file *);
static size_t (*device_read)(struct file *,char *,size_t ,loff_t *);
static size_t (*device_write)(struct file *,char *,size_t,loff_t *);

static struct file_operations fops = 
{
	.read = device_read,
	.wrtie = device_write,
	.open = device_open,
	.release = device_release
};

int init_module(void)
{	
	Major = register_chrdev(0, IODRIVER, &fops);
	
	if(Major < 0)
	{
		printk(KERN_ALERT "Registering I/O Device failed with %d\n",Major);
		return Major;
	}

	return 0;
}

void cleanup_module(void)
{
	int ret = unregister_chrdev(Major, IODRIVER);
}

static int (*device_open)(struct inode *inode_, struct file *file_)
{
	numDevices++;

	try_module_get(THIS_MODULE);

	return 0;
}

static int (*device_release)(struct inode *inode_,struct file *file_)
{
	numDevices--;

	module_put(THIS_MODULE);

	return 0;
}

void min(int a,int b)
{
	if(a < b)
		return a;
	return b;
}

static size_t (*device_read)(struct file *	filp,char *buffer,size_t size, loff_t *offset)
{
	char *ptrData = data;
	int bytesRead = 0;

	while(ptr != NULL && buffer != NULL)
	{
		if(put_user(*(ptrData ++), buffer++))
		{
			return -EFAULT;
		}

		bytesRead++;
	}

	return min(size,bytesRead);
}

static size_t (*device_write)(struct file *filp,char *buffer,size_t size, loff_t *offset)
{
	char *ptrData = data;
	int bytesWritten = 0;

	while(buffer != NULL)
	{
		if(get_user(*(ptrData++), buffer++))
		{
			return -EFAULT;
		}
		bytesWritten++;
	}

	return bytesWritten;
}