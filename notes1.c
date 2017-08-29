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
�й�ioctal��cmd��������

�ڲ���ioctlʱ���֣���cmdΪ2��ʱ��ioctl������ȷ���ã�
ioctl���ǽ���ʹ�û���
�ο��ں˵��ĵ�documention\ioctl\ioctl-number.txt
///////////////////////////////////////////////////////////////////////////////////////////////////// 
linuxӦ���ļ�����
#include<sys/types.h>  /*�ṩ����pid_t,size_t�Ķ���*/
#include<sys/stat.h>
#include<fcntl.h>
int open(const char *path, int oflags,mode_t mode);
O_RDONLY��O_WRONLY��O_RDWR
ssize_t read(int fd,void * buf ,size_t count); 		#include<unistd.h> �ɹ�д����ֽ��飬ʧ�ܷ���-1
ssize_t write (int fd,const void * buf,size_t count);  #include<unistd.h> ����ʵ�ʶ�ȡ���ֽ���
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
ret = alloc_chrdev_region(&devid, 0, 1, "backlight");  //�ɹ�����0��ʧ�ܷ��ش�����
if(IS_ERR_VALUE��ret��)
	goto errno;
cdev_init(&backlight_cdev, &backlight_fops);					//�ɹ�����0��ʧ�ܷ��ش�����
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
/////////////////////////////////////////////////////////////////////////////////////////////////////
androidϵͳ�´�ӡLinux�����е�printk����

��Androidϵͳ�У�printk�������־��Ϣ������/proc/kmsg��

cat  /proc/kmsg | grep "alarm" //grep "alarm"��ʾֻץȡalarm����Ϣ
/////////////////////////////////////////////////////////////////////////////////////////////////////
�����ж�̬�����豸�ڵ�
udev��ȡ/sys/class����Ϣ������µ��豸���ú�̨����ͻ��Զ������豸�ڵ㡣

struct class *class_create(struct module *owner, const char *name)   !
void class_destroy(struct class *cls)
struct device *device_create(struct class *class, struct device *parent, dev_t devt, void *drvdata, const char *fmt, ...)
void device_destroy(struct class *class, dev_t devt)
���ӣ�
struct class *my_class;
my_class = class_create(THIS_MODULE, "test_class"); //����class
if(IS_ERR(my_class)){
	printk("create cleaa wrong!\n");
	return -1;
}
device_create(my_class, NULL, MKDEV(253, 0), NULL, "%s", "test_led"); //����class���豸�Ŵ����豸
device_destroy(my_class, MKDEV(253, 0)); //ע�����豸
class_destroy(my_class); //ע����
/////////////////////////////////////////////////////////////////////////////////////////////////////
�����жϺ���
��linux/err.h�а�������һ���ƵĴ�����Ҫͨ��IS_ERR, PTR_ERR, ERR_PTR�����ꡣ
#define MAX_ERRNO       4095
#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)
/* �������ת��Ϊָ�룬���ڴ������-1000~0�䣬���ص�ָ����������һҳ  */
static inline void *ERR_PTR(long error)
{
         return (void *) error;
}
/* ��ָ��ת��Ϊ�����  */
static inline long PTR_ERR(const void *ptr)
{
         return (long) ptr;
}
/* �жϷ��ص�ָ���Ǵ�����Ϣ����ʵ�ʵ�ַ����ָ���Ƿ��������һҳ */
static inline long IS_ERR(const void *ptr)
{
         return IS_ERR_VALUE((unsigned long)ptr);
}
�����ں��з��ص�ָ�룬������ķ�ʽ����if(!retptr)������if( IS_ERR(retptr) �� If( IS_ERR_VALUE(retptr) )��
/////////////////////////////////////////////////////////////////////////////////////////////////////
�й�androidһЩ�ļ���Դ���е�·��
SystemServer.java·����frameworks/base/services/java/com/android/server/SystemServer.java
Java: frameworks/base/services/core/java/com/android/server/lights/LightsService.java
JNI: frameworks/base/services/core/jni/com_android_server_lights_LightsService.cpp
Hal:  lights.c
/////////////////////////////////////////////////////////////////////////////////////////////////////
�豸����
���ߣ�
/*linux/sysfs.h*/
struct attribute {
 const char *name; //�趨���ļ�������
 struct module *owner; //�趨���ļ�������
 mode_t mode; //�趨���ļ����ļ�����Ȩ��
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
BUS_ATTR(_name, _mode, _show, _store);   �������߽ṹ��
int bus_create_file(struct bus_type *bus, struct bus_attribute *attr)  �ɹ�����0
void bus_remove_file(struct bus_type *bus, struct bus_attribute *attr)
int bus_register(struct bus_type *bus);          �ɹ�����0
void bus_unregister(struct bus_type *bus);
���ӣ�
BUS_ATTR(version, S_IRUGO|S_IWUGO, show_bus_version, store_bus_version);
retval = bus_create_file(&usb_bus, &bus_attr_version);
if(retval)
	goto   ;
struct bus_type usb_bus = {
.name = "usb", //�������ߵ�����Ϊusb��ע��ɹ�����/sys/busĿ¼�¿���
}; //Ŀ¼usb��������ϵͳ�Ѿ���usb���ߣ������Ҫ�������֡�
ret = bus_register(&usb_bus);
bus_unregister(&usb_bus);

�豸��
struct device{
	 struct device *parent; //ָ�����豸�ĸ��豸�������ָ��(NULL)��ע�����豸Ŀ¼��sys/device��
	 char bus_id[BUS_ID_SIZE]; /* position on parent bus */ //��������ʶ���豸���ַ�����ͬʱҲ���豸ע����Ŀ¼���֡�
	 struct bus_type *bus; /* type of bus device is on */ //ָ�����豸���ӵ�����
	 struct device_driver *driver; /* which driver has allocated this device */ //������豸����������
	 void *driver_data; /* data private to the driver */ //���������˽������
	 void (*release)(struct device *dev); //�����豸�����һ�����ñ�ɾ��ʱ�����øú���,����Ҫ��������ģ��ʱ�����
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

������
struct device_driver {
	const char *name; //�������������֣��ڶ�Ӧ���ߵ�driverĿ¼����ʾ
	struct bus_type *bus; //ָ���������������������������ͣ��������ã���Ȼ��ע��ʧ��
	int (*probe) (struct device *dev); //̽�⺯��
	int (*remove) (struct device *dev); //ж�غ��������豸��ϵͳ��ɾ��ʱ���ã�
	void (*shutdown) (struct device *dev); //��ϵͳ�ػ��ǵ���
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
�豸���Ĳ���API  http://www.linuxidc.com/Linux/2017-02/140818.htm
//include/of.h
struct device_node {
	const char *name;						�ڵ���
	const char *type;						�豸����
	const char *full_name;				ȫ·���ڵ���
	struct  device_node *parent;	���ڵ�ָ��
	struct  device_node *child;		�ӽڵ�ָ��
}
/**
 * of_find_compatible_node - ͨ��compatible���Բ���ָ���ڵ�
 * @from - ָ��ʼ·���Ľڵ㣬���ΪNULL����Ӹ��ڵ㿪ʼ
 * @type - device_type�豸���ͣ�����ΪNULL
 * @compat - ָ��ڵ��compatible���Ե�ֵ���ַ��������׵�ַ
 * �ɹ����õ��ڵ���׵�ַ��ʧ�ܣ�NULL
 */
struct device_node * of_find_compatible_node(struct device_node *from,const char *type, const char *compat);
/**
 * of_find_matching_node - ͨ��compatible���Բ���ָ���ڵ�
 * @from - ָ��ʼ·���Ľڵ㣬���ΪNULL����Ӹ��ڵ㿪ʼ
 * @matches - ָ���豸ID��ע��ID�������NULL����
 * ������  const struct of_device_id mydemo_of_match[] = {
                { .compatible = "fs4412,mydemo", },
                {}
            };      
 * �ɹ����õ��ڵ���׵�ַ��ʧ�ܣ�NULL
 */
struct device_node * of_find_matching_node(struct device_node *from,const struct of_device_id *matches);
/*���豸���np�ж�ȡ������Ϊpropname������Ϊ8��16��32λ������ֵ��������out_values*/
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