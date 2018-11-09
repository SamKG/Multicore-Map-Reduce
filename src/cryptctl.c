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

//command defines
#define CRYPTCTL_CREATE 1

typedef struct cryptworker{
	dev_t encrypter_dev_t;
	dev_t decrypter_dev_t;
	struct cdev encrypter;
	struct cdev decrypter;
} cryptworker;

typedef struct file_private_data{
	int type;
	char* cipher;
	int cipher_size;
	char* data;
	int data_size;
	int data_count;
}file_private_data;


static int	majorNumber;
static int	minorCount;
static struct	class* cryptClass = NULL;
//static struct	device* cryptctlCdev = NULL;
static struct	cdev*   cryptctlCdev = NULL;

static int	ctl_open(struct inode*, struct file*);
static int	ctl_release(struct inode*, struct file*);
static ssize_t	ctl_read(struct file*, char*, size_t, loff_t*);
static ssize_t	ctl_write(struct file*,	const char*, size_t, loff_t*);
static long	ctl_ioctl(struct file*,unsigned int,unsigned long);

static int	worker_open(struct inode*, struct file*);
static int	worker_release(struct inode*, struct file*);
static ssize_t	encrypt_worker_read(struct file*, char*, size_t, loff_t*);
static ssize_t	encrypt_worker_write(struct file*,	const char*, size_t, loff_t*);
static ssize_t	decrypt_worker_read(struct file*, char*, size_t, loff_t*);
static ssize_t	decrypt_worker_write(struct file*,	const char*, size_t, loff_t*);
static long	worker_ioctl(struct file*,unsigned int,unsigned long);

static cryptworker* workers = NULL;
static int 	numWorkers = 0;
static struct file_operations fops =
{
	.open = ctl_open,
	.release = ctl_release,
	.read = ctl_read,
	.write = ctl_write,
	.unlocked_ioctl = ctl_ioctl,
};

static struct file_operations encrypt_workerfops =
{
	.open = worker_open,
	.release = worker_release,
	.read = encrypt_worker_read,
	.write = encrypt_worker_write,
	.unlocked_ioctl = worker_ioctl,
};
static struct file_operations decrypt_workerfops =
{
	.open = worker_open,
	.release = worker_release,
	.read = decrypt_worker_read,
	.write = decrypt_worker_write,
	.unlocked_ioctl = worker_ioctl,
};
static int __init crypt_init(void){
	printk(KERN_WARNING "Crypt: Initializing...\n");
	int i;
	dev_t tmp;
	int allocReturn = alloc_chrdev_region(&tmp, 0, MAX_DEVICE_COUNT, DEVICE_NAME);//register_chrdev(0, DEVICE_NAME, &fops);
	if (allocReturn < 0){
		printk(KERN_ALERT "Crypt: Wasn't able to allocate a chrdev region!\n");
		return allocReturn;
	}
	majorNumber = MAJOR(tmp);
	minorCount = 0;
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
	device_create(cryptClass, NULL, MKDEV(majorNumber,minorCount++), NULL, DEVICE_NAME);
	cdev_add(cryptctlCdev, MKDEV(majorNumber,0), 1);	
	
	numWorkers = 0;
	workers = (cryptworker*) kmalloc(sizeof(cryptworker)*MAX_DEVICE_COUNT,0);
	printk(KERN_WARNING "Crypt: Finished initializing!\n");
	return 0;
}	
static void __exit crypt_exit(void){
	printk(KERN_WARNING "Crypt: Cleaning up...\n");
	int i = 0;
	for (i = 0 ; i < numWorkers ; i++){
		device_destroy(cryptClass, workers[numWorkers].encrypter_dev_t);
		device_destroy(cryptClass, workers[numWorkers].decrypter_dev_t);
		cdev_del(&workers[numWorkers].encrypter);
		cdev_del(&workers[numWorkers].decrypter);
	}
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
	switch(cmd){
	case CRYPTCTL_CREATE:;
		cryptworker* newWorker = &(workers[numWorkers++]);
		newWorker->encrypter_dev_t = MKDEV(majorNumber,minorCount++);
		newWorker->decrypter_dev_t = MKDEV(majorNumber,minorCount++);
		cdev_init(&newWorker->encrypter,&encrypt_workerfops);
		cdev_init(&newWorker->decrypter,&decrypt_workerfops);
		newWorker->encrypter.owner = THIS_MODULE;
		newWorker->decrypter.owner = THIS_MODULE;
		char* encrypterName = (char*) kmalloc(sizeof(char)*100,0);
		snprintf(encrypterName, 100,"encrypt%d",numWorkers-1);
		
		char* decrypterName = (char*) kmalloc(sizeof(char)*100,0);
		snprintf(decrypterName, 100,"encrypt%d",numWorkers-1);

		device_create(cryptClass, NULL, newWorker->encrypter_dev_t, NULL, encrypterName);
		device_create(cryptClass, NULL, newWorker->decrypter_dev_t, NULL, decrypterName);
		cdev_add(&newWorker->encrypter, newWorker->encrypter_dev_t, 1);	
		cdev_add(&newWorker->decrypter, newWorker->decrypter_dev_t, 1);	
		printk(KERN_WARNING "Crypt: Creating new encrypt/decrypt pair!\n");
		kfree(encrypterName);
		kfree(decrypterName);
		break;
	default:
		break;
	}
	return 0;
}
//define worker methods below
static int worker_open(struct inode* inode, struct file* filp){
	try_module_get(THIS_MODULE);
	file_private_data* dataptr = (file_private_data*) kmalloc(sizeof(file_private_data),0);
	filp->private_data = dataptr;
	dataptr->type = 0;
	dataptr->data = (char*) kmalloc(sizeof(char),0); 
	dataptr->data_size = 1;
	dataptr->data_count = 0;
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
char vigenere_encrypt(char key, char value){
	if(value >= ' ' && value <= '~'){
		char tmp = value - ' ';
		tmp = (tmp + key) % 95;
		return (char) (tmp+' ');
	}
	return value;
}
char vigenere_decrypt(char key, char value){
	if(value >= ' ' && value <= '~'){
		char tmp = value - ' ';
		tmp = ((tmp - key + 256)%256) % 95;
		return (char) (tmp+' ');
	}
	return value;
}
static ssize_t encrypt_worker_read(struct file* filp, char* buff, size_t readsize, loff_t* fileoff){
	int filepos = filp->f_pos;
	file_private_data* dat = (file_private_data*) (filp->private_data);
	if(dat == NULL || dat->cipher == NULL || dat->data == NULL || readsize == 0){
		return 0;
	}
	char* tmp = (char*) kmalloc(sizeof(char)*readsize,0);
	int i = 0;
	for (i = 0 ; i < readsize ; i++){
		int currpos = i + filepos;
		if (dat->data_count < currpos){
			break;
		}
		tmp[i] = vigenere_encrypt(dat->cipher[i%dat->cipher_size],dat->data[currpos]);
	}
	copy_to_user(buff,tmp,i);	
	kfree(tmp);
	return i; 
}
static ssize_t encrypt_worker_write(struct file* filp, const char* msg, size_t strsize, loff_t* fileoff){	
	int filepos = filp->f_pos;
	file_private_data* dat = (file_private_data*) (filp->private_data);
	if(dat == NULL || dat->cipher == NULL || dat->data == NULL || strsize == 0){
		return 0;
	}
	char* tmp = (char*) kmalloc(sizeof(char)*strsize,0);	
	copy_from_user(tmp,msg,strsize);
	if (filepos + strsize > dat->data_size){
		krealloc(dat->data,(filepos + strsize)*2*sizeof(char),0);
		dat->data_size = (filepos + strsize)*2;
	}	
	int i;
	for (i = 0 ; i < strsize ; i++){
		dat->data[filepos+i] = tmp[i];
		if (filepos + i > dat->data_count){
			dat->data_count = filepos + i;
		}
	}
	kfree(tmp);
	return i;
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
