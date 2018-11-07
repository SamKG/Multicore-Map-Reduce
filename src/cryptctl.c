#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#define DEVICE_NAME "cryptctl"
#define CLASS_NAME "crypt"
#define SUCCESS 0
#define MAX_DEVICE_COUNT 1024

static int	majorNumber;
static int	minorCount;
static struct	class* cryptClass = NULL;
//static struct	device* cryptctlCdev = NULL;
static struct	cdev*   cryptctlCdev = NULL;
static struct	cdev*	cryptCdev = NULL;

static int	ctl_open(struct inode*, struct file*);
static int	ctl_release(struct inode*, struct file*);
static ssize_t	ctl_read(struct file*, char*, size_t, loff_t*);
static ssize_t	ctl_write(struct file*,	const char*, size_t, loff_t*);
static long	ctl_ioctl(struct file*,unsigned int,unsigned long);

static struct file_operations fops =
{
	.open = ctl_open,
	.release = ctl_release,
	.read = ctl_read,
	.write = ctl_write,
	.unlocked_ioctl = ctl_ioctl,
};

static struct file_operations workerfops =
{
	.open = ctl_open,
	.release = ctl_release,
	.read = ctl_read,
	.write = ctl_write,
	.unlocked_ioctl = ctl_ioctl,
};
static int __init crypt_init(void){
	printk(KERN_WARNING "Crypt: Initializing...\n");
	dev_t tmp;
	int allocReturn = alloc_chrdev_region(&tmp, 0, MAX_DEVICE_COUNT, DEVICE_NAME);//register_chrdev(0, DEVICE_NAME, &fops);
	if (allocReturn < 0){
		printk(KERN_ALERT "Crypt: Wasn't able to allocate a chrdev region!\n");
		return allocReturn;
	}
	majorNumber = MAJOR(tmp);
	cryptClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(cryptClass)){
		unregister_chrdev(majorNumber, DEVICE_NAME);	
		printk(KERN_ALERT "Crypt: Wasn't able to register device class!\n");
		return PTR_ERR(cryptClass);
	}

	cryptctlCdev = cdev_alloc();

	cdev_init(cryptctlCdev, &fops);
	if (IS_ERR(cryptctlCdev)){
		kobject_put(&cryptctlCdev->kobj);
		cryptctlCdev = NULL;
		class_destroy(cryptClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);	
		printk(KERN_ALERT "Crypt: Wasn't able to create controller device!\n");
		return PTR_ERR(cryptctlCdev);
	}
	cryptctlCdev->owner = THIS_MODULE;
	device_create(cryptClass, NULL, MKDEV(majorNumber,0), NULL, DEVICE_NAME);
	cdev_add(cryptctlCdev, MKDEV(majorNumber,0), 1);	
	
	cryptCdev = cdev_alloc();
	cdev_init(cryptCdev, &workerfops);
	if (IS_ERR(cryptCdev)){
		kobject_put(&cryptCdev->kobj);
		cryptCdev = NULL;
		kobject_put(&cryptctlCdev->kobj);
		cryptctlCdev = NULL;
		class_destroy(cryptClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);	
		printk(KERN_ALERT "Crypt: Wasn't able to create worker cdev!\n");
		return PTR_ERR(cryptCdev);
	}
	cryptCdev->owner = THIS_MODULE;

	printk(KERN_WARNING "Crypt: Finished initializing!\n");
	return 0;
}	
static void __exit crypt_exit(void){
	printk(KERN_WARNING "Crypt: 
Cleaning up...\n");
	cdev_del(cryptCdev);
	device_destroy(cryptClass, MKDEV(majorNumber,0));
	cdev_del(cryptctlCdev);
	class_destroy(cryptClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_WARNING "Crypt: Done cleaning up! Goodbye!\n");
}
// Define controller methods below
//Defines what happens when ctrl is opened (should not be allowed - only should be allowed to ioctl
static int ctl_open(struct inode* inode, struct file* filp){
	try_module_get(THIS_MODULE);
	printk(KERN_WARNING "Crypt: Opened!\n");
	return SUCCESS;
}
static int ctl_release(struct inode* inode, struct file* filp){
	module_put(THIS_MODULE);
	printk(KERN_WARNING "Crypt: Released!\n");
	return SUCCESS;
}
static ssize_t ctl_read(struct file* filp, char* buff, size_t readsize, loff_t* filepos){
	return 0; 
}
static ssize_t ctl_write(struct file* filp, const char* msg, size_t strsize, loff_t* filepos){
	return 0;
}
static long ctl_ioctl(struct file* filp,unsigned int cmd,unsigned long arg){
	return 0;
}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Samyak K. Gupta");
MODULE_DESCRIPTION("Cryptographic Linux Kernel Module for CS416");
MODULE_VERSION("0.1");
module_init(crypt_init);
module_exit(crypt_exit);
