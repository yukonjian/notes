/* �����е�ͷ�ļ� */
#include <linux/module.h>
#include <linux/init.h>					//���������ͷ�ļ�

#include <linux/fs.h>						//file_operations
#include <linux/cdev.h>					//�ַ��豸ע����Ҫ��ͷ�ļ�

#include <asm/uaccess.h>					//copy_to_user, copy_from_user
#include <linux/errno.h>					//�����˴����

#include <linux/slab.h>					//�ڴ����kmalloc
#include <linux/device.h>				//�ļ��ڵ���Ĵ���





/* Ӧ���е�ͷ�ļ� */
#include <stdio.h>								//��׼���������printf,read,write
#include <fcntl.h>								//open
#include <sys/types.h>						//�ṩ����pid_t,size_t�Ķ���
#include <sys/stat.h>