/////////////////////////////////////////////////////////////////////////////////////////////////////
文件操作
struct file_operations {
	struct module *owner;
	int (*open) (struct inode *, struct file *);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
}
例子：
static struct file_operations backlight_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   backlight_open, 
    .unlocked_ioctl  =   backlight_ioctl, 
};
static int backlight_open(struct inode *inode, struct file *file)
static long  backlight_ioctl(struct file *filep, unsigned int cmd,
                unsigned long arg)
/////////////////////////////////////////////////////////////////////////////////////////////////////     
有关ioctal的cmd参数问题

在测试ioctl时发现，当cmd为2的时候，ioctl不能正确调用，
ioctl还是建议使用幻数
参考内核的文档documention\ioctl\ioctl-number.txt
///////////////////////////////////////////////////////////////////////////////////////////////////// 
linux应用文件操作
#include<sys/types.h>  /*提供类型pid_t,size_t的定义*/
#include<sys/stat.h>
#include<fcntl.h>
int open(const char *path, int oflags,mode_t mode);
O_RDONLY，O_WRONLY，O_RDWR
ssize_t read(int fd,void * buf ,size_t count); 		#include<unistd.h> 成功写入的字节书，失败返回-1
ssize_t write (int fd,const void * buf,size_t count);  #include<unistd.h> 返回实际读取的字节数
/////////////////////////////////////////////////////////////////////////////////////////////////////             
设备的注册
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count,  !!
			const char *name)
void cdev_init(struct cdev *cdev, const struct file_operations *fops)
int cdev_add(struct cdev *p, dev_t dev, unsigned count)									!!
void cdev_del(struct cdev *p)	
void unregister_chrdev_region(dev_t from, unsigned count)	
例子：
dev_t devid;
static struct cdev backlight_cdev;
ret = alloc_chrdev_region(&devid, 0, 1, "backlight");  //成功返回0，失败返回错误码
if(IS_ERR_VALUE（ret）)
	goto errno;
cdev_init(&backlight_cdev, &backlight_fops);					//成功返回0，失败返回错误码
cdev_add(&backlight_cdev, devid, 1);

cdev_del(&backlight_cdev);
unregister_chrdev_region(MKDEV(major, 0), 1);
///////////////////////////////////////////////////////////////////////////////////////////////////// 
内存分配和内存映射
static inline void *kmalloc(size_t s, gfp_t gfp)   !!
static inline void kfree(void *p)
static inline void __iomem *ioremap(phys_addr_t offset, unsigned long size)
static inline void iounmap(void __iomem *addr)
例子：
struct xxx *ret;
ret = kamlloc(sizeof(struct xxx),GFP_KERNEL);
kfree(ret);
///////////////////////////////////////////////////////////////////////////////////////////////////// 
cqa64中的sys_config操作
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
例子：
static script_item_value_type_e		backlight_type;
script_item_u	backlight_val;
backlight_type = script_get_item("lcd0", "lcd_bl_en", &backlight_val); 
if (SCIRPT_ITEM_VALUE_TYPE_PIO != backlight_type) {
	printk("lcd_bl_en type fail !");
	return -1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
GPIO子系统操作
int gpio_request(unsigned gpio, const char *label)
int gpio_direction_input(unsigned gpio)
static int gpiod_direction_output(struct gpio_desc *desc, int value)
void __gpio_set_value(unsigned gpio, int value)
int __gpio_get_value(unsigned gpio)
例子：
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
/////////////////////////////////////////////////////////////////////////////////////////////////////
android系统下打印Linux驱动中的printk内容

在Android系统中，printk输出的日志信息保存在/proc/kmsg中

cat  /proc/kmsg | grep "alarm" //grep "alarm"表示只抓取alarm的信息
/////////////////////////////////////////////////////////////////////////////////////////////////////
驱动中动态创建设备节点
udev读取/sys/class的信息，添加新的设备，该后台程序就会自动创建设备节点。

struct class *class_create(struct module *owner, const char *name)   !
void class_destroy(struct class *cls)
struct device *device_create(struct class *class, struct device *parent, dev_t devt, void *drvdata, const char *fmt, ...)
void device_destroy(struct class *class, dev_t devt)
例子：
struct class *my_class;
my_class = class_create(THIS_MODULE, "test_class"); //创建class
if(IS_ERR(my_class)){
	printk("create cleaa wrong!\n");
	return -1;
}
device_create(my_class, NULL, MKDEV(253, 0), NULL, "%s", "test_led"); //根据class和设备号创建设备
device_destroy(my_class, MKDEV(253, 0)); //注销类设备
class_destroy(my_class); //注销类
/////////////////////////////////////////////////////////////////////////////////////////////////////
错误判断函数
在linux/err.h中包含了这一机制的处理，主要通过IS_ERR, PTR_ERR, ERR_PTR几个宏。
#define MAX_ERRNO       4095
#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)
/* 将错误号转化为指针，由于错误号在-1000~0间，返回的指针会落在最后一页  */
static inline void *ERR_PTR(long error)
{
         return (void *) error;
}
/* 将指针转化为错误号  */
static inline long PTR_ERR(const void *ptr)
{
         return (long) ptr;
}
/* 判断返回的指针是错误信息还是实际地址，即指针是否落在最后一页 */
static inline long IS_ERR(const void *ptr)
{
         return IS_ERR_VALUE((unsigned long)ptr);
}
对于内核中返回的指针，检查错误的方式不是if(!retptr)，而是if( IS_ERR(retptr) 或 If( IS_ERR_VALUE(retptr) )。
/////////////////////////////////////////////////////////////////////////////////////////////////////
有关android一些文件在源码中的路径
SystemServer.java路径：frameworks/base/services/java/com/android/server/SystemServer.java
Java: frameworks/base/services/core/java/com/android/server/lights/LightsService.java
JNI: frameworks/base/services/core/jni/com_android_server_lights_LightsService.cpp
Hal:  lights.c
/////////////////////////////////////////////////////////////////////////////////////////////////////
设备驱动
总线：
/*linux/sysfs.h*/
struct attribute {
 const char *name; //设定该文件的名字
 struct module *owner; //设定该文件的属主
 mode_t mode; //设定该文件的文件操作权限
};
/*linux/device.h*/
struct bus_attribute {
 struct attribute attr;
 ssize_t (*show)(struct bus_type *bus, char *buf);
 ssize_t (*store)(struct bus_type *bus, const char *buf, size_t count);
};
struct bus_type{
	const char *name;
	struct bus_attribute *bus_attrs;
	int (*match)(struct device *dev, struct device_driver *drv);
	int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
	int (*probe)(struct device *dev);
};
BUS_ATTR(_name, _mode, _show, _store);   创建总线结构体
int bus_create_file(struct bus_type *bus, struct bus_attribute *attr)  成功返回0
void bus_remove_file(struct bus_type *bus, struct bus_attribute *attr)
int bus_register(struct bus_type *bus);          成功返回0
void bus_unregister(struct bus_type *bus);
例子：
BUS_ATTR(version, S_IRUGO|S_IWUGO, show_bus_version, store_bus_version);
retval = bus_create_file(&usb_bus, &bus_attr_version);
if(retval)
	goto   ;
struct bus_type usb_bus = {
.name = "usb", //定义总线的名字为usb，注册成功后将在/sys/bus目录下看到
}; //目录usb，如果你的系统已经有usb总线，那你就要换个名字。
ret = bus_register(&usb_bus);
bus_unregister(&usb_bus);

设备：
struct device{
	 struct device *parent; //指定该设备的父设备，如果不指定(NULL)，注册后的设备目录在sys/device下
	 char bus_id[BUS_ID_SIZE]; /* position on parent bus */ //在总线生识别设备的字符串，同时也是设备注册后的目录名字。
	 struct bus_type *bus; /* type of bus device is on */ //指定该设备连接的总线
	 struct device_driver *driver; /* which driver has allocated this device */ //管理该设备的驱动函数
	 void *driver_data; /* data private to the driver */ //驱动程序的私有数据
	 void (*release)(struct device *dev); //当给设备的最后一个引用被删除时，调用该函数,必须要，否则载模块时会出错
}
int device_register(struct device *dev);
void device_unregister(struct device *dev);
struct device_attribute {
 struct attribute attr;
 ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
 char *buf);
 ssize_t (*store)(struct device *dev, struct device_attribute *attr,const char *buf, size_t count);
 const char *buf, size_t count);
};
DEVICE_ATTR(_name, _mode, _show, _store)
int device_create_file(struct device *dev, struct device_attribute *attr)
void device_remove_file(struct device *dev, struct device_attribute *attr)
int snprintf(char *str, size_t size, const char *format, ...)

驱动：
struct device_driver {
	const char *name; //驱动函数的名字，在对应总线的driver目录下显示
	struct bus_type *bus; //指定该驱动程序所操作的总线类型，必须设置，不然会注册失败
	int (*probe) (struct device *dev); //探测函数
	int (*remove) (struct device *dev); //卸载函数，当设备从系统中删除时调用，
	void (*shutdown) (struct device *dev); //当系统关机是调用
}
int driver_register(struct device_driver *drv)
void driver_unregister(struct device_driver *drv)
struct driver_attribute {
 struct attribute attr;
 ssize_t (*show)(struct device_driver *driver, char *buf);
 ssize_t (*store)(struct device_driver *driver, const char *buf,
 size_t count);
};
DRIVER_ATTR(_name, _mode, _show, _store)
int driver_create_file(struct device_driver *drv, struct driver_attribute *attr)
void driver_remove_file(struct device_driver *drv, struct driver_attribute *attr)
/////////////////////////////////////////////////////////////////////////////////////////////////////
设备树的操作API  http://www.linuxidc.com/Linux/2017-02/140818.htm
//include/of.h
struct device_node {
	const char *name;						节点名
	const char *type;						设备类型
	const char *full_name;				全路径节点名
	struct  device_node *parent;	父节点指针
	struct  device_node *child;		子节点指针
}
/**
 * of_find_compatible_node - 通过compatible属性查找指定节点
 * @from - 指向开始路径的节点，如果为NULL，则从根节点开始
 * @type - device_type设备类型，可以为NULL
 * @compat - 指向节点的compatible属性的值（字符串）的首地址
 * 成功：得到节点的首地址；失败：NULL
 */
struct device_node * of_find_compatible_node(struct device_node *from,const char *type, const char *compat);
/**
 * of_find_matching_node - 通过compatible属性查找指定节点
 * @from - 指向开始路径的节点，如果为NULL，则从根节点开始
 * @matches - 指向设备ID表，注意ID表必须以NULL结束
 * 范例：  const struct of_device_id mydemo_of_match[] = {
                { .compatible = "fs4412,mydemo", },
                {}
            };      
 * 成功：得到节点的首地址；失败：NULL
 */
struct device_node * of_find_matching_node(struct device_node *from,const struct of_device_id *matches);
/*从设备结点np中读取属性名为propname，类型为8、16、32位的属性值，并放入out_values*/
static inline int of_property_read_u32(const struct device_node *np,const char *propname,u8 *out_value)

/*
 * Struct used for matching a device
 */
struct of_device_id
{
	char	name[32];
	char	type[32];
	char	compatible[128];
	const void *data;
};