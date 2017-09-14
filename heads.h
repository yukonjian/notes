/* 驱动中的头文件 */
#include <linux/module.h>
#include <linux/init.h>					//驱动必须的头文件

#include <linux/fs.h>						//file_operations
#include <linux/cdev.h>					//字符设备注册需要的头文件

#include <asm/uaccess.h>					//copy_to_user, copy_from_user
#include <linux/errno.h>					//包含了错误号

#include <linux/slab.h>					//内存分配kmalloc
#include <linux/device.h>				//文件节点类的创建





/* 应用中的头文件 */
#include <stdio.h>								//标准输入输出，printf,read,write
#include <fcntl.h>								//open
#include <sys/types.h>						//提供类型pid_t,size_t的定义
#include <sys/stat.h>