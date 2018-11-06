#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#define DEVICE_NAME "cryptctl"
#define CLASS_NAME "crypt"
static int	majorNumber;
static struct	class* cryptClass = NULL;
static struct	device* cryptControllerDevice = NULL;

static int	dev_open(struct inode*, struct file*);
static int	dev_release(struct inode*, struct file*);
static ssize_t	dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t	dev_write(struct file*,	const char*, size_t, loff_t*);
static long	dev_ioctl(struct file*,unsigned int,unsigned long);

static struct file_operations fops =
{
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
	.unlocked_ioctl = dev_ioctl,
};

static int __init crypt_init(void){
	printk(KERN_WARNING "Crypt: Initializing...\n");
	return 0;
}	
static void __exit crypt_exit(void){
	printk(KERN_WARNING "Crypt: Cleaning up...\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Samyak K. Gupta");
MODULE_DESCRIPTION("Cryptographic Linux Kernel Module for CS416");
MODULE_VERSION("0.1");
module_init(crypt_init);
module_exit(crypt_exit);
