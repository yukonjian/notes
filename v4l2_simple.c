/******************************************************************************************************************************************/
Linux系统中视频输入设备主要包括以下四个部分：
1.字符设备驱动程序核心：V4L2本身就是一个字符设备，具有字符设备所有的特性，暴露接口给用户空间；
2.V4L2驱动核心：主要是构建一个内核中标准视频设备驱动的框架，为视频操作提供统一的接口函数；
3.平台V4L2设备驱动：在V4L2框架下，根据平台自身的特性实现与平台相关的V4L2驱动部分，包括注册video_device和v4l2_dev。
4.具体的sensor驱动：主要上电、提供工作时钟、视频图像裁剪、流IO开启等，实现各种设备控制方法供上层调用并注册v4l2_subdev。
/******************************************************************************************************************************************/
V4L2的核心源码位于drivers/media/v4l2-core，源码以实现的功能可以划分为四类：
1.核心模块实现：由v4l2-dev.c实现，主要作用申请字符主设备号、注册class和提供video device注册注销等相关函数；
2.V4L2框架：由v4l2-device.c、v4l2-subdev.c、v4l2-fh.c、v4l2-ctrls.c等文件实现，构建V4L2框架；
3.Videobuf管理：由videobuf2-core.c、videobuf2-dma-contig.c、videobuf2-dma-sg.c、videobuf2-memops.c、videobuf2-vmalloc.c、v4l2-mem2mem.c等文件实现，
	完成videobuffer的分配、管理和注销。
4.Ioctl框架：由v4l2-ioctl.c文件实现，构建V4L2ioctl的框架。
/******************************************************************************************************************************************/
v4l2_device
v4l2_device的主要作用是管理注册在其下的子设备，方便系统查找引用到。
struct v4l2_device {
				struct device *dev;
				structlist_head subdevs;    //用链表管理注册的subdev
				charname[V4L2_DEVICE_NAME_SIZE];    //device 名字
				structkref ref;      //引用计数
				/* The control handler. May be NULL. */
				struct v4l2_ctrl_handler *ctrl_handler;
};
int v4l2_device_register(struct device*dev, struct v4l2_device *v4l2_dev)
static void v4l2_device_release(struct kref *ref)
/******************************************************************************************************************************************/
V4l2_subdev
struct v4l2_subdev {
         structv4l2_device *v4l2_dev;  //指向父设备
         //提供一些控制v4l2设备的接口
         conststruct v4l2_subdev_ops *ops;
         //向V4L2框架提供的接口函数
         conststruct v4l2_subdev_internal_ops *internal_ops;
         //subdev控制接口
         structv4l2_ctrl_handler *ctrl_handler;
         /* namemust be unique */
         charname[V4L2_SUBDEV_NAME_SIZE];
         /*subdev device node */
         struct video_device *devnode;  
};
struct v4l2_subdev_ops {
//视频设备通用的操作：初始化、加载FW、上电和RESET等
         conststruct v4l2_subdev_core_ops        *core;
//视频设备的特有操作：设置帧率、裁剪图像、开关视频流等
         conststruct v4l2_subdev_video_ops      *video;
};
int v4l2_device_register_subdev(struct v4l2_device*v4l2_dev, struct v4l2_subdev *sd)
void v4l2_device_unregister_subdev(struct v4l2_subdev*sd)
/******************************************************************************************************************************************/
video_device
video_device结构体用于在/dev目录下生成设备节点文件，把操作设备的接口暴露给用户空间。
struct video_device
{
 	/* device ops */
	const struct v4l2_file_operations *fops;

	/* sysfs */
	struct device dev;		/* v4l device */
	struct cdev *cdev;		/* character device */

	/* Set either parent or v4l2_dev if your driver uses v4l2_device */
	struct device *parent;		/* device parent */
	struct v4l2_device *v4l2_dev;	/* v4l2_device parent */

	/* Control handler associated with this device node. May be NULL. */
	struct v4l2_ctrl_handler *ctrl_handler;

	/* vb2_queue associated with this device node. May be NULL. */
	struct vb2_queue *queue;	

	/* V4L2 file handles */
	spinlock_t		fh_lock; /* Lock for all v4l2_fhs */
	struct list_head	fh_list; /* List of struct v4l2_fh */
	
	/* callbacks */
	void (*release)(struct video_device *vdev);

	/* ioctl callbacks */
	const struct v4l2_ioctl_ops *ioctl_ops;
};
struct video_device *video_device_alloc(void)
void video_device_release(struct video_device *vdev)
static inline int __must_check video_register_device(struct video_device *vdev, int type, int nr)
void video_unregister_device(struct video_device *vdev);
type：设备类型，包括VFL_TYPE_GRABBER、VFL_TYPE_VBI、VFL_TYPE_RADIO和VFL_TYPE_SUBDEV。
nr：设备节点名编号，如/dev/video[nr]。
/******************************************************************************************************************************************/
v4l2_fh
v4l2_fh是用来保存子设备的特有操作方法，也就是下面要分析到的v4l2_ctrl_handler，内核提供一组v4l2_fh的操作方法，通常在打开设备节点时进行v4l2_fh注册。
void v4l2_fh_init(struct v4l2_fh *fh, structvideo_device *vdev) //初始化v4l2_fh，添加v4l2_ctrl_handler到v4l2_fh：
void v4l2_fh_add(struct v4l2_fh *fh) //添加v4l2_fh到video_device，方便核心层调用
/******************************************************************************************************************************************/
v4l2_ctrl_handler
v4l2_ctrl_handler是用于保存子设备控制方法集的结构体，对于视频设备这些ctrls包括设置亮度、饱和度、对比度和清晰度等，用链表的方式来保存ctrls，
可以通过v4l2_ctrl_new_std函数向链表添加ctrls。
struct v4l2_ctrl *v4l2_ctrl_new_std(structv4l2_ctrl_handler *hdl,
                            conststruct v4l2_ctrl_ops *ops,
                            u32 id, s32 min, s32 max, u32 step, s32 def)
id是通过IOCTL的arg参数传过来的指令，定义在v4l2-controls.h文件；
/******************************************************************************************************************************************/
ioctl框架
用户空间通过打开/dev/目录下的设备节点，获取到文件的file结构体，通过系统调用ioctl把cmd和arg传入到内核。通过一系列的调用后最终会调用到__video_do_ioctl函数，
然后通过cmd检索v4l2_ioctls[]，判断是INFO_FL_STD还是INFO_FL_FUNC。如果是INFO_FL_STD会直接调用到视频设备驱动中video_device->v4l2_ioctl_ops函数集。
如果是INFO_FL_FUNC会先调用到v4l2自己实现的标准回调函数，然后根据arg再调用到video_device->v4l2_ioctl_ops或v4l2_fh->v4l2_ctrl_handler函数集。
/******************************************************************************************************************************************/
IO访问
1.read和write，是基本帧IO访问方式，通过read读取每一帧数据，数据需要在内核和用户之间拷贝，这种方式访问速度可能会非常慢；
2.内存映射缓冲区(V4L2_MEMORY_MMAP)，是在内核空间开辟缓冲区，应用通过mmap()系统调用映射到用户地址空间。
这些缓冲区可以是大而连续DMA缓冲区、通过vmalloc()创建的虚拟缓冲区，或者直接在设备的IO内存中开辟的缓冲区(如果硬件支持)；
3.用户空间缓冲区(V4L2_MEMORY_USERPTR)，是用户空间的应用中开辟缓冲区，用户与内核空间之间交换缓冲区指针。
Read和write方式属于帧IO访问方式，每一帧都要通过IO操作，需要用户和内核之间数据拷贝，而后两种是流IO访问方式，不需要内存拷贝，访问速度比较快。
内存映射缓冲区访问方式是比较常用的方式。
/******************************************************************************************************************************************/
vb2_queue
struct vb2_queue {
         enum v4l2_buf_type                  type;  //buffer类型
         unsigned int                        io_modes;  //访问IO的方式:mmap、userptr etc
 
         const struct vb2_ops                 *ops;   //buffer队列操作函数集合
         const struct vb2_mem_ops     *mem_ops;  //buffer memory操作集合
 
         struct vb2_buffer              *bufs[VIDEO_MAX_FRAME];  //代表每个buffer
         unsigned int                        num_buffers;    //分配的buffer个数
};
Vb2_queue代表一个videobuffer队列，vb2_buffer是这个队列中的成员，vb2_mem_ops是缓冲内存的操作函数集，vb2_ops用来管理队列。
｛
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct vfe_buffer);
	q->ops = &vfe_video_qops;
	q->mem_ops = &vb2_dma_contig_memops;
	q->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	ret = vb2_queue_init(q);
｝

vb2_mem_ops
vb2_mem_ops包含了内存映射缓冲区、用户空间缓冲区的内存操作方法：
struct vb2_mem_ops {
         void           *(*alloc)(void *alloc_ctx, unsignedlong size);  //分配视频缓存
         void           (*put)(void *buf_priv);            //释放视频缓存
//获取用户空间视频缓冲区指针
         void           *(*get_userptr)(void *alloc_ctx,unsigned long vaddr, 
                                               unsignedlong size, int write);
         void           (*put_userptr)(void *buf_priv);       //释放用户空间视频缓冲区指针
 
//用于缓存同步
         void           (*prepare)(void *buf_priv);
         void           (*finish)(void *buf_priv);
 
         void           *(*vaddr)(void *buf_priv);
         void           *(*cookie)(void *buf_priv);
         unsignedint     (*num_users)(void *buf_priv);         //返回当期在用户空间的buffer数
 
         int              (*mmap)(void *buf_priv, structvm_area_struct *vma);  //把缓冲区映射到用户空间
};
提供了三种类型的视频缓存区操作方法：连续的DMA缓冲区、集散的DMA缓冲区以及vmalloc创建的缓冲区，
分别由videobuf2-dma-contig.c、videobuf2-dma-sg.c和videobuf-vmalloc.c文件实现
vb2_ops
vb2_ops是用来管理buffer队列的函数集合，包括队列和缓冲区初始化
struct vb2_ops {
         //队列初始化
         int(*queue_setup)(struct vb2_queue *q, const struct v4l2_format *fmt,
                               unsigned int *num_buffers, unsigned int*num_planes,
                               unsigned int sizes[], void *alloc_ctxs[]);
         //释放和获取设备操作锁
         void(*wait_prepare)(struct vb2_queue *q);
         void(*wait_finish)(struct vb2_queue *q);
         //对buffer的操作
         int(*buf_init)(struct vb2_buffer *vb);
         int(*buf_prepare)(struct vb2_buffer *vb);
         int(*buf_finish)(struct vb2_buffer *vb);
         void(*buf_cleanup)(struct vb2_buffer *vb);
//开始视频流
         int(*start_streaming)(struct vb2_queue *q, unsigned int count);
//停止视频流
         int(*stop_streaming)(struct vb2_queue *q);
//把VB传递给驱动
         void(*buf_queue)(struct vb2_buffer *vb);
};
vb2_buffer
帧以v4l2_buffer的格式在应用和驱动之间传输。一个缓冲区可以有三种状态：
1.在驱动的传入队列中，驱动程序将会对此队列中的缓冲区进行处理，用户空间通过IOCTL:VIDIOC_QBUF把缓冲区放入到队列。
对于一个视频捕获设备，传入队列中的缓冲区是空的，驱动会往其中填充数据；
2.在驱动的传出队列中，这些缓冲区已由驱动处理过，对于一个视频捕获设备，缓存区已经填充了视频数据，正等用户空间来认领；
3.用户空间状态的队列，已经通过IOCTL:VIDIOC_DQBUF传出到用户空间的缓冲区，此时缓冲区由用户空间拥有，驱动无法访问。
struct v4l2_buffer {
         __u32                          index;  //buffer 序号
         __u32                          type;   //buffer类型
         __u32                          bytesused;  缓冲区已使用byte数
         __u32                          flags;
         __u32                          field;
         struct timeval           timestamp;  //时间戳，代表帧捕获的时间
         struct v4l2_timecode       timecode;
         __u32                          sequence;
 
         /*memory location */
         __u32                          memory;  //表示缓冲区是内存映射缓冲区还是用户空间缓冲区
         union {
                   __u32           offset;  //内核缓冲区的位置
                   unsigned long   userptr;   //缓冲区的用户空间地址
                   struct v4l2_plane *planes;
                   __s32                 fd;
         } m;
         __u32                          length;   //缓冲区大小，单位byte
};
当用户空间拿到v4l2_buffer，可以获取到缓冲区的相关信息。

Byteused是图像数据所占的字节数，如果是V4L2_MEMORY_MMAP方式，m.offset是内核空间图像数据存放的开始地址，
会传递给mmap函数作为一个偏移，通过mmap映射返回一个缓冲区指针p，p+byteused是图像数据在进程的虚拟地址空间所占区域；

如果是用户指针缓冲区的方式，可以获取的图像数据开始地址的指针m.userptr，userptr是一个用户空间的指针，
userptr+byteused便是所占的虚拟地址空间，应用可以直接访问。
/******************************************************************************************************************************************/









