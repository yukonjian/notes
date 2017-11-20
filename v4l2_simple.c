/******************************************************************************************************************************************/
Linuxϵͳ����Ƶ�����豸��Ҫ���������ĸ����֣�
1.�ַ��豸����������ģ�V4L2�������һ���ַ��豸�������ַ��豸���е����ԣ���¶�ӿڸ��û��ռ䣻
2.V4L2�������ģ���Ҫ�ǹ���һ���ں��б�׼��Ƶ�豸�����Ŀ�ܣ�Ϊ��Ƶ�����ṩͳһ�Ľӿں�����
3.ƽ̨V4L2�豸��������V4L2����£�����ƽ̨���������ʵ����ƽ̨��ص�V4L2�������֣�����ע��video_device��v4l2_dev��
4.�����sensor��������Ҫ�ϵ硢�ṩ����ʱ�ӡ���Ƶͼ��ü�����IO�����ȣ�ʵ�ָ����豸���Ʒ������ϲ���ò�ע��v4l2_subdev��
/******************************************************************************************************************************************/
V4L2�ĺ���Դ��λ��drivers/media/v4l2-core��Դ����ʵ�ֵĹ��ܿ��Ի���Ϊ���ࣺ
1.����ģ��ʵ�֣���v4l2-dev.cʵ�֣���Ҫ���������ַ����豸�š�ע��class���ṩvideo deviceע��ע������غ�����
2.V4L2��ܣ���v4l2-device.c��v4l2-subdev.c��v4l2-fh.c��v4l2-ctrls.c���ļ�ʵ�֣�����V4L2��ܣ�
3.Videobuf������videobuf2-core.c��videobuf2-dma-contig.c��videobuf2-dma-sg.c��videobuf2-memops.c��videobuf2-vmalloc.c��v4l2-mem2mem.c���ļ�ʵ�֣�
	���videobuffer�ķ��䡢�����ע����
4.Ioctl��ܣ���v4l2-ioctl.c�ļ�ʵ�֣�����V4L2ioctl�Ŀ�ܡ�
/******************************************************************************************************************************************/
v4l2_device
v4l2_device����Ҫ�����ǹ���ע�������µ����豸������ϵͳ�������õ���
struct v4l2_device {
				struct device *dev;
				structlist_head subdevs;    //���������ע���subdev
				charname[V4L2_DEVICE_NAME_SIZE];    //device ����
				structkref ref;      //���ü���
				/* The control handler. May be NULL. */
				struct v4l2_ctrl_handler *ctrl_handler;
};
int v4l2_device_register(struct device*dev, struct v4l2_device *v4l2_dev)
static void v4l2_device_release(struct kref *ref)
/******************************************************************************************************************************************/
V4l2_subdev
struct v4l2_subdev {
         structv4l2_device *v4l2_dev;  //ָ���豸
         //�ṩһЩ����v4l2�豸�Ľӿ�
         conststruct v4l2_subdev_ops *ops;
         //��V4L2����ṩ�Ľӿں���
         conststruct v4l2_subdev_internal_ops *internal_ops;
         //subdev���ƽӿ�
         structv4l2_ctrl_handler *ctrl_handler;
         /* namemust be unique */
         charname[V4L2_SUBDEV_NAME_SIZE];
         /*subdev device node */
         struct video_device *devnode;  
};
struct v4l2_subdev_ops {
//��Ƶ�豸ͨ�õĲ�������ʼ��������FW���ϵ��RESET��
         conststruct v4l2_subdev_core_ops        *core;
//��Ƶ�豸�����в���������֡�ʡ��ü�ͼ�񡢿�����Ƶ����
         conststruct v4l2_subdev_video_ops      *video;
};
int v4l2_device_register_subdev(struct v4l2_device*v4l2_dev, struct v4l2_subdev *sd)
void v4l2_device_unregister_subdev(struct v4l2_subdev*sd)
/******************************************************************************************************************************************/
video_device
video_device�ṹ��������/devĿ¼�������豸�ڵ��ļ����Ѳ����豸�Ľӿڱ�¶���û��ռ䡣
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
type���豸���ͣ�����VFL_TYPE_GRABBER��VFL_TYPE_VBI��VFL_TYPE_RADIO��VFL_TYPE_SUBDEV��
nr���豸�ڵ�����ţ���/dev/video[nr]��
/******************************************************************************************************************************************/
v4l2_fh
v4l2_fh�������������豸�����в���������Ҳ��������Ҫ��������v4l2_ctrl_handler���ں��ṩһ��v4l2_fh�Ĳ���������ͨ���ڴ��豸�ڵ�ʱ����v4l2_fhע�ᡣ
void v4l2_fh_init(struct v4l2_fh *fh, structvideo_device *vdev) //��ʼ��v4l2_fh�����v4l2_ctrl_handler��v4l2_fh��
void v4l2_fh_add(struct v4l2_fh *fh) //���v4l2_fh��video_device��������Ĳ����
/******************************************************************************************************************************************/
v4l2_ctrl_handler
v4l2_ctrl_handler�����ڱ������豸���Ʒ������Ľṹ�壬������Ƶ�豸��Щctrls�����������ȡ����Ͷȡ��ԱȶȺ������ȵȣ�������ķ�ʽ������ctrls��
����ͨ��v4l2_ctrl_new_std�������������ctrls��
struct v4l2_ctrl *v4l2_ctrl_new_std(structv4l2_ctrl_handler *hdl,
                            conststruct v4l2_ctrl_ops *ops,
                            u32 id, s32 min, s32 max, u32 step, s32 def)
id��ͨ��IOCTL��arg������������ָ�������v4l2-controls.h�ļ���
/******************************************************************************************************************************************/
ioctl���
�û��ռ�ͨ����/dev/Ŀ¼�µ��豸�ڵ㣬��ȡ���ļ���file�ṹ�壬ͨ��ϵͳ����ioctl��cmd��arg���뵽�ںˡ�ͨ��һϵ�еĵ��ú����ջ���õ�__video_do_ioctl������
Ȼ��ͨ��cmd����v4l2_ioctls[]���ж���INFO_FL_STD����INFO_FL_FUNC�������INFO_FL_STD��ֱ�ӵ��õ���Ƶ�豸������video_device->v4l2_ioctl_ops��������
�����INFO_FL_FUNC���ȵ��õ�v4l2�Լ�ʵ�ֵı�׼�ص�������Ȼ�����arg�ٵ��õ�video_device->v4l2_ioctl_ops��v4l2_fh->v4l2_ctrl_handler��������
/******************************************************************************************************************************************/
IO����
1.read��write���ǻ���֡IO���ʷ�ʽ��ͨ��read��ȡÿһ֡���ݣ�������Ҫ���ں˺��û�֮�俽�������ַ�ʽ�����ٶȿ��ܻ�ǳ�����
2.�ڴ�ӳ�仺����(V4L2_MEMORY_MMAP)�������ں˿ռ俪�ٻ�������Ӧ��ͨ��mmap()ϵͳ����ӳ�䵽�û���ַ�ռ䡣
��Щ�����������Ǵ������DMA��������ͨ��vmalloc()���������⻺����������ֱ�����豸��IO�ڴ��п��ٵĻ�����(���Ӳ��֧��)��
3.�û��ռ仺����(V4L2_MEMORY_USERPTR)�����û��ռ��Ӧ���п��ٻ��������û����ں˿ռ�֮�佻��������ָ�롣
Read��write��ʽ����֡IO���ʷ�ʽ��ÿһ֡��Ҫͨ��IO��������Ҫ�û����ں�֮�����ݿ�����������������IO���ʷ�ʽ������Ҫ�ڴ濽���������ٶȱȽϿ졣
�ڴ�ӳ�仺�������ʷ�ʽ�ǱȽϳ��õķ�ʽ��
/******************************************************************************************************************************************/
vb2_queue
struct vb2_queue {
         enum v4l2_buf_type                  type;  //buffer����
         unsigned int                        io_modes;  //����IO�ķ�ʽ:mmap��userptr etc
 
         const struct vb2_ops                 *ops;   //buffer���в�����������
         const struct vb2_mem_ops     *mem_ops;  //buffer memory��������
 
         struct vb2_buffer              *bufs[VIDEO_MAX_FRAME];  //����ÿ��buffer
         unsigned int                        num_buffers;    //�����buffer����
};
Vb2_queue����һ��videobuffer���У�vb2_buffer����������еĳ�Ա��vb2_mem_ops�ǻ����ڴ�Ĳ�����������vb2_ops����������С�
��
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct vfe_buffer);
	q->ops = &vfe_video_qops;
	q->mem_ops = &vb2_dma_contig_memops;
	q->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	ret = vb2_queue_init(q);
��

vb2_mem_ops
vb2_mem_ops�������ڴ�ӳ�仺�������û��ռ仺�������ڴ����������
struct vb2_mem_ops {
         void           *(*alloc)(void *alloc_ctx, unsignedlong size);  //������Ƶ����
         void           (*put)(void *buf_priv);            //�ͷ���Ƶ����
//��ȡ�û��ռ���Ƶ������ָ��
         void           *(*get_userptr)(void *alloc_ctx,unsigned long vaddr, 
                                               unsignedlong size, int write);
         void           (*put_userptr)(void *buf_priv);       //�ͷ��û��ռ���Ƶ������ָ��
 
//���ڻ���ͬ��
         void           (*prepare)(void *buf_priv);
         void           (*finish)(void *buf_priv);
 
         void           *(*vaddr)(void *buf_priv);
         void           *(*cookie)(void *buf_priv);
         unsignedint     (*num_users)(void *buf_priv);         //���ص������û��ռ��buffer��
 
         int              (*mmap)(void *buf_priv, structvm_area_struct *vma);  //�ѻ�����ӳ�䵽�û��ռ�
};
�ṩ���������͵���Ƶ����������������������DMA����������ɢ��DMA�������Լ�vmalloc�����Ļ�������
�ֱ���videobuf2-dma-contig.c��videobuf2-dma-sg.c��videobuf-vmalloc.c�ļ�ʵ��
vb2_ops
vb2_ops����������buffer���еĺ������ϣ��������кͻ�������ʼ��
struct vb2_ops {
         //���г�ʼ��
         int(*queue_setup)(struct vb2_queue *q, const struct v4l2_format *fmt,
                               unsigned int *num_buffers, unsigned int*num_planes,
                               unsigned int sizes[], void *alloc_ctxs[]);
         //�ͷźͻ�ȡ�豸������
         void(*wait_prepare)(struct vb2_queue *q);
         void(*wait_finish)(struct vb2_queue *q);
         //��buffer�Ĳ���
         int(*buf_init)(struct vb2_buffer *vb);
         int(*buf_prepare)(struct vb2_buffer *vb);
         int(*buf_finish)(struct vb2_buffer *vb);
         void(*buf_cleanup)(struct vb2_buffer *vb);
//��ʼ��Ƶ��
         int(*start_streaming)(struct vb2_queue *q, unsigned int count);
//ֹͣ��Ƶ��
         int(*stop_streaming)(struct vb2_queue *q);
//��VB���ݸ�����
         void(*buf_queue)(struct vb2_buffer *vb);
};
vb2_buffer
֡��v4l2_buffer�ĸ�ʽ��Ӧ�ú�����֮�䴫�䡣һ������������������״̬��
1.�������Ĵ�������У��������򽫻�Դ˶����еĻ��������д����û��ռ�ͨ��IOCTL:VIDIOC_QBUF�ѻ��������뵽���С�
����һ����Ƶ�����豸����������еĻ������ǿյģ�������������������ݣ�
2.�������Ĵ��������У���Щ�������������������������һ����Ƶ�����豸���������Ѿ��������Ƶ���ݣ������û��ռ������죻
3.�û��ռ�״̬�Ķ��У��Ѿ�ͨ��IOCTL:VIDIOC_DQBUF�������û��ռ�Ļ���������ʱ���������û��ռ�ӵ�У������޷����ʡ�
struct v4l2_buffer {
         __u32                          index;  //buffer ���
         __u32                          type;   //buffer����
         __u32                          bytesused;  ��������ʹ��byte��
         __u32                          flags;
         __u32                          field;
         struct timeval           timestamp;  //ʱ���������֡�����ʱ��
         struct v4l2_timecode       timecode;
         __u32                          sequence;
 
         /*memory location */
         __u32                          memory;  //��ʾ���������ڴ�ӳ�仺���������û��ռ仺����
         union {
                   __u32           offset;  //�ں˻�������λ��
                   unsigned long   userptr;   //���������û��ռ��ַ
                   struct v4l2_plane *planes;
                   __s32                 fd;
         } m;
         __u32                          length;   //��������С����λbyte
};
���û��ռ��õ�v4l2_buffer�����Ի�ȡ���������������Ϣ��

Byteused��ͼ��������ռ���ֽ����������V4L2_MEMORY_MMAP��ʽ��m.offset���ں˿ռ�ͼ�����ݴ�ŵĿ�ʼ��ַ��
�ᴫ�ݸ�mmap������Ϊһ��ƫ�ƣ�ͨ��mmapӳ�䷵��һ��������ָ��p��p+byteused��ͼ�������ڽ��̵������ַ�ռ���ռ����

������û�ָ�뻺�����ķ�ʽ�����Ի�ȡ��ͼ�����ݿ�ʼ��ַ��ָ��m.userptr��userptr��һ���û��ռ��ָ�룬
userptr+byteused������ռ�������ַ�ռ䣬Ӧ�ÿ���ֱ�ӷ��ʡ�
/******************************************************************************************************************************************/









