/* �����е�ͷ�ļ� */
#include <linux/module.h>
#include <linux/init.h>					//���������ͷ�ļ�

#include <linux/fs.h>						//file_operations
#include <linux/cdev.h>					//�ַ��豸ע����Ҫ��ͷ�ļ�

#include <asm/uaccess.h>					//copy_to_user, copy_from_user
#include <linux/errno.h>					//�����˴����

#include <linux/interrupt.h>			//�ж�ע�ắ�����жϵĴ���ģʽ
#include <linux/workqueue.h>			//��������ͷ�ļ�

#include <linux/wait.h>					//�ȴ�����ͷ�ļ�
#include <linux/sched.h>					//����ͷ�ļ���schedule��jiffies,add_timer,wake_up
#include <linux/timer.h>					//��ʱ�����ͷ�ļ�

#include <linux/platform_device.h>			//ƽ̨�豸
#include <linux/ioport.h>				//ƽ̨�豸��Դ

#include <linux/slab.h>					//�ڴ����kmalloc
#include <linux/device.h>				//�ļ��ڵ���Ĵ���

#include <asm/current.h>					//current






/* Ӧ���е�ͷ�ļ� */
#include <stdio.h>								//��׼���������printf,read,write
#include <fcntl.h>								//open
#include <sys/types.h>						//�ṩ����pid_t,size_t�Ķ���
#include <sys/stat.h>