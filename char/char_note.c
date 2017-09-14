/************************************************************
** 驱动部分 
*************************************************************/

/*文件操作*/
struct file_operations {
	struct module *owner;
	int (*open) (struct inode *, struct file *);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);	
//long (*ioctl) (struct file *, unsigned int, unsigned long);			//老的内核使用
  loff_t (*llseek) (struct file *, loff_t, int);
}
/*************************************************************/
/* 调试打印 */
static int pat_debug_enable = 1;
#define dprintk(fmt, arg...) \
	do { if (pat_debug_enable) printk(KERN_DEBUG fmt, ##arg); } while (0)
		
/*************************************************************/
/* open */
static int test_open(struct inode *inode, struct file *file)
/**
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member)

/*************************************************************/
/* close */
static int test_close(struct inode *node, struct file *filp)

/*************************************************************/
/* read */
ssize_t test_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
/* BUF_SIZE 10; data[BUF_SIZE]; *offset最小值是0，最大值是BUF_SIZE，读取成功后*offset += count; */
copy_to_user(void __user *to, const void *from, unsigned long n)
copy_from_user(void *to, const void __user *from, unsigned long n)
返回：成功读取的字节数

/*************************************************************/
/* write */
ssize_t test_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)  
/* 读写对*offset处理一样 */
返回：成功写入的字节数

/*************************************************************/
/* llseek */
loff_t test_llseek (struct file *filp, loff_t offset, int whence)
#define SEEK_SET	0	/* seek relative to beginning of file */
#define SEEK_CUR	1	/* seek relative to current file position */
#define SEEK_END	2	/* seek relative to end of file */

/*************************************************************/
/* init */
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count,
			const char *name)
void cdev_init(struct cdev *cdev, const struct file_operations *fops)
int cdev_add(struct cdev *p, dev_t dev, unsigned count)									
void cdev_del(struct cdev *p)	
void unregister_chrdev_region(dev_t from, unsigned count)	

/*************************************************************/
/* 内存分配和内存映射 */
static inline void *kmalloc(size_t s, gfp_t gfp)   !!
static inline void kfree(void *p)
static inline void __iomem *ioremap(phys_addr_t offset, unsigned long size)
static inline void iounmap(void __iomem *addr)

/*************************************************************/


