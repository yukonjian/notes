/////////////////////////////////////////////////////////////////////////////////////////////////////
�ļ�����
struct file_operations {
	struct module *owner;
	int (*open) (struct inode *, struct file *);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
}
���ӣ�
static struct file_operations backlight_fops = {
    .owner  =   THIS_MODULE,    /* ����һ���꣬�������ģ��ʱ�Զ�������__this_module���� */
    .open   =   backlight_open, 
    .unlocked_ioctl  =   backlight_ioctl, 
};
static int backlight_open(struct inode *inode, struct file *file)
static long  backlight_ioctl(struct file *filep, unsigned int cmd,
                unsigned long arg)
/////////////////////////////////////////////////////////////////////////////////////////////////////                
�豸��ע��
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count,  !!
			const char *name)
void cdev_init(struct cdev *cdev, const struct file_operations *fops)
int cdev_add(struct cdev *p, dev_t dev, unsigned count)									!!
void cdev_del(struct cdev *p)	
void unregister_chrdev_region(dev_t from, unsigned count)	
���ӣ�
dev_t devid;
static struct cdev backlight_cdev;
alloc_chrdev_region(&devid, 0, 1, "backlight");
cdev_init(&backlight_cdev, &backlight_fops);
cdev_add(&backlight_cdev, devid, 1);

cdev_del(&backlight_cdev);
unregister_chrdev_region(MKDEV(major, 0), 1);
///////////////////////////////////////////////////////////////////////////////////////////////////// 
�ڴ������ڴ�ӳ��
static inline void *kmalloc(size_t s, gfp_t gfp)   !!
static inline void kfree(void *p)
static inline void __iomem *ioremap(phys_addr_t offset, unsigned long size)
static inline void iounmap(void __iomem *addr)
���ӣ�
struct xxx *ret;
ret = kamlloc(sizeof(struct xxx),GFP_KERNEL);
kfree(ret);
///////////////////////////////////////////////////////////////////////////////////////////////////// 
cqa64�е�sys_config����
typedef enum {
	SCIRPT_ITEM_VALUE_TYPE_INVALID = 0,
	SCIRPT_ITEM_VALUE_TYPE_INT,
	SCIRPT_ITEM_VALUE_TYPE_STR,
	SCIRPT_ITEM_VALUE_TYPE_PIO,
} script_item_value_type_e;

typedef union {
    int                 val;
    char                *str;
    struct gpio_config  gpio;
} script_item_u;

struct gpio_config {
	u32	gpio;		/* gpio global index, must be unique */
	u32 	mul_sel;	/* multi sel val: 0 - input, 1 - output... */
	u32 	pull;		/* pull val: 0 - pull up/down disable, 1 - pull up... */
	u32 	drv_level;	/* driver level val: 0 - level 0, 1 - level 1... */
	u32	data;		/* data val: 0 - low, 1 - high, only vaild when mul_sel is input/output */
};

script_item_value_type_e
script_get_item(char *main_key, char *sub_key, script_item_u *item)
���ӣ�
static script_item_value_type_e		backlight_type;
script_item_u	backlight_val;
backlight_type = script_get_item("lcd0", "lcd_bl_en", &backlight_val); 
if (SCIRPT_ITEM_VALUE_TYPE_PIO != backlight_type) {
	printk("lcd_bl_en type fail !");
	return -1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
GPIO��ϵͳ����
int gpio_request(unsigned gpio, const char *label)
int gpio_direction_input(unsigned gpio)
static int gpiod_direction_output(struct gpio_desc *desc, int value)
void __gpio_set_value(unsigned gpio, int value)
int __gpio_get_value(unsigned gpio)
���ӣ�
if(0 != gpio_request(backlight_val.gpio.gpio, NULL)) {
	printk("lcd_bl_en gpio_request fail !");		
	backlight_val.gpio.gpio = -1;
}
if (0 != gpio_direction_output(backlight_val.gpio.gpio, 0)) {
	printk("lcd_bl_en gpio_direction_output fail !");
	backlight_val.gpio.gpio = -1;
}	
if(backlight_val.gpio.gpio != -1) {
	__gpio_set_value(backlight_val.gpio.gpio, 1);
/////////////////////////////////////////////////////////////////////////////////////////////////////}