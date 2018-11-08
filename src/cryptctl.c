#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#define DEVICE_NAME "cryptctl"
#define CLASS_NAME "crypt"
#define SUCCESS 0
#define MAX_DEVICE_COUNT 1024
#define PRIVATE_DATA_SIZE 4096

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

static int	worker_open(struct inode*, struct file*);
static int	worker_release(struct inode*, struct file*);
static ssize_t	worker_read(struct file*, char*, size_t, loff_t*);
static ssize_t	worker_write(struct file*,	const char*, size_t, loff_t*);
static long	worker_ioctl(struct file*,unsigned int,unsigned long);

static dev_t	workers[MAX_DEVICE_COUNT];
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
	.open = worker_open,
	.release = worker_release,
	.read = worker_read,
	.write = worker_write,
	.unlocked_ioctl = worker_ioctl,
};
static int __init crypt_init(void){
	printk(KERN_WARNING "Crypt: Initializing...\n");
	int i;;
	for (i = 0 ; i < MAX_DEVICE_COUNT ; i++){
		workers[i] = 0;
	}
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
	printk(KERN_WARNING "Crypt: Cleaning up...\n");
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
//define worker methods below
static int worker_open(struct inode* inode, struct file* filp){
	try_module_get(THIS_MODULE);
	void* dataptr = kmalloc(sizeof(char) * PRIVATE_DATA_SIZE,0);
	filp->private_data = dataptr;
	printk(KERN_WARNING "Crypt worker: Opened!\n");
	return SUCCESS;
}
static int worker_release(struct inode* inode, struct file* filp){
	module_put(THIS_MODULE);
	kfree(filp->private_data);
	filp->private_data = NULL;
	printk(KERN_WARNING "Crypt worker: Released!\n");
	return SUCCESS;
}
static ssize_t worker_read(struct file* filp, char* buff, size_t readsize, loff_t* filepos){
	return 0; 
}
static ssize_t worker_write(struct file* filp, const char* msg, size_t strsize, loff_t* filepos){
	
	return 0;
}
static long worker_ioctl(struct file* filp,unsigned int cmd,unsigned long arg){
	return 0;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Samyak K. Gupta");
MODULE_DESCRIPTION("Cryptographic Linux Kernel Module for CS416");
MODULE_VERSION("0.1");
module_init(crypt_init);
module_exit(crypt_exit);
