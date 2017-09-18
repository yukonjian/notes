/* 驱动中的头文件 */
#include <linux/module.h>
#include <linux/init.h>					//驱动必须的头文件

#include <linux/fs.h>						//file_operations
#include <linux/cdev.h>					//字符设备注册需要的头文件

#include <asm/uaccess.h>					//copy_to_user, copy_from_user
#include <linux/errno.h>					//包含了错误号

#include <linux/interrupt.h>			//中断注册函数和中断的触发模式
#include <linux/workqueue.h>			//工作队列头文件

#include <linux/wait.h>					//等待队列头文件
#include <linux/sched.h>					//调度头文件，schedule，jiffies,add_timer,wake_up
#include <linux/timer.h>					//定时器相关头文件

#include <linux/platform_device.h>			//平台设备
#include <linux/ioport.h>				//平台设备资源

#include <linux/slab.h>					//内存分配kmalloc
#include <linux/device.h>				//文件节点类的创建

#include <asm/current.h>					//current






/* 应用中的头文件 */
#include <stdio.h>								//标准输入输出，printf,read,write
#include <fcntl.h>								//open
#include <sys/types.h>						//提供类型pid_t,size_t的定义
#include <sys/stat.h>