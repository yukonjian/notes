#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include <asm/uaccess.h>
#include <linux/errno.h>

//#include <linux/mm.h>
#include <linux/device.h>

static int pat_debug_enable = 1;

#define dprintk(fmt, arg...) \
	do { if (pat_debug_enable) printk(KERN_DEBUG fmt, ##arg); } while (0)
		

struct test_t{
	dev_t devno;
	struct cdev test_cdev;
};

static struct test_t *test;
static struct class *test_class;

static int test_open(struct inode *inode, struct file *file)
{
		return 0;
}

static int test_close(struct inode *node, struct file *filp)
{
		return 0;
}
ssize_t test_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
	int retval;
	dprintk("the read offset data is: %ld /n", *offset);
	if(copy_to_user(buf,"This is a test read",count)){
		retval = -EFAULT;
	}else{
		retval = count;
		dprintk("the read success,%s \n",__FUNCTION__);
	}
	dprintk("the read offset data is: %ld /n", *offset);
	return retval;
}
ssize_t test_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)  
{
	int retval;
	char data[50];
	dprintk("the write offset data is: %ld /n", *offset);
	if(copy_from_user(data,buf,count)){
		retval = -EFAULT;
	}else{
		retval = count;
		dprintk("the write data: %s /n",data);
	}
	dprintk("the write offset data is: %ld /n", *offset);
	return retval;
}

loff_t test_llseek (struct file *filp, loff_t offset, int whence)
{
	return 0;
}

static struct file_operations test_fops = {
    .owner  =   THIS_MODULE,    /* ����һ���꣬�������ģ��ʱ�Զ�������__this_module���� */
    .open   =   test_open, 
    .release=		test_close,
	.read	=	test_read,
	.write	=	test_write,
    
};



static int __init test_init(void)
{
	int retval;
	dprintk("the start init \n");
	test = kmalloc(sizeof(struct test_t),GFP_KERNEL);
	if(!test)
		return -ENOMEM;
		
	retval = alloc_chrdev_region(&test->devno, 0, 1, "test"); 
	if(retval < 0)
		goto alloc_devno_failed;
		
	cdev_init(&test->test_cdev, &test_fops);
	retval = cdev_add(&test->test_cdev, test->devno, 1);	
	if(retval < 0)
		goto cdev_add_failed;
		
	test_class = class_create(THIS_MODULE, "test_class");
		if(IS_ERR(test_class)){
		retval = PTR_ERR(test_class);
		goto class_create_failed;
	}
//	device_create(test_class, NULL, test->devno, NULL, "test_led");
	class_device_create(test_class, NULL, test->devno, NULL, "test_led");

	
	return 0;
			
	class_create_failed:
		cdev_del(&test->test_cdev);
		
	cdev_add_failed:
		unregister_chrdev_region(test->devno, 1);	
	
	alloc_devno_failed:
		kfree(test);
		
		return retval;
}

static void __exit test_exit(void)
{
	device_destroy(test_class, test->devno); //ע�����豸
	class_destroy(test_class); //ע����
	
	cdev_del(&test->test_cdev);
	unregister_chrdev_region(test->devno, 1);	
	kfree(test);
}
module_init(test_init);
module_exit(test_exit);
