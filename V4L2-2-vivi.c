ǰ��һƪ�����У��򵥷����� V4L2 ���ܣ����Ľ����ں��е���������ͷ���� vivi ������һ������������ͷ��������vivi ����ں���Ҫ������ usb ����ͷ����������û��������Ӳ����ز�Ĳ�����Ҳ����˵�׿��˸��ӵ� usb ������֪ʶ��������� V4L2 ������ܣ�������������Ӧ�õĽ�����
ǰ�������ᵽ��V4L2 �ĺ����� v4l2-dev.c �������ṩͳһ���ļ������ӿ� v4l2_fops �������ṩ video_device ע��ӿ� register_video_device ����Ϊһ���������������Ҫ���Ĺ������Ƿ��䡢���á�ע��һ�� video_device.��ܼܺ򵥣����ӵ�����Ƶ�豸����ڶ�� ioctl��
һ��vivi ��ܷ���
static int __init vivi_init(void)
{
ret = vivi_create_instance(i);
...
return ret;
}
module_init(vivi_init);
vivi ������һ�� video_device ָ�룬û��ȥ���ö���ֱ������ָ����һ���ֳɵ� video_device �ṹ vivi_template ����ôȫ���Ĺ�������Χ�� vivi_template չ����
static int __init vivi_create_instance(int inst)
{
struct vivi_dev *dev;
struct video_device *vfd;
struct v4l2_ctrl_handler *hdl;
struct vb2_queue *q;

// ����һ�� vivi_dev �ṹ��
dev = kzalloc(sizeof(*dev), GFP_KERNEL);

// v4l2_dev ��ʼ��,��û��ʲô����
ret = v4l2_device_register(NULL, &dev->v4l2_dev);

// ���� dev ��һЩ����������ͼ���ʽ����С
dev->fmt = &formats[0];
dev->width = 640;
dev->height = 480;
dev->pixelsize = dev->fmt->depth / 8;
...

// vivi_dev->vb_vidq(vb2_queue) ��ʼ��
q = &dev->vb_vidq;
memset(q, 0, sizeof(dev->vb_vidq));
q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_READ;
q->drv_priv = dev;
q->buf_struct_size = sizeof(struct vivi_buffer);

// vivi_dev->vb_vidq(vb2_queue)->ops
q->ops = &vivi_video_qops;

// vivi_dev->vb_vidq(vb2_queue)->mem_ops
q->mem_ops = &vb2_vmalloc_memops;

// ��ʼ��һЩ��֮��Ķ���
vb2_queue_init(q);

/* init video dma queues */
INIT_LIST_HEAD(&dev->vidq.active);
init_waitqueue_head(&dev->vidq.wq);

// ����һ�� video_device ,������ص�
vfd = video_device_alloc();

*vfd = vivi_template;
vfd->debug = debug;
vfd->v4l2_dev = &dev->v4l2_dev;
set_bit(V4L2_FL_USE_FH_PRIO, &vfd->flags);

vfd->lock = &dev->mutex;

// ע�� video_device ������
ret = video_register_device(vfd, VFL_TYPE_GRABBER, video_nr);
// �� vivi_dev ���� video_device->dev->p->driver_data ,�����߾����õ�
video_set_drvdata(vfd, dev);

/* Now that everything is fine, let&#39;s add it to device list */
list_add_tail(&dev->vivi_devlist, &vivi_devlist);

if (video_nr != -1)
video_nr++;
// vivi_dev->vfd(video_device) = vfd
dev->vfd = vfd;
v4l2_info(&dev->v4l2_dev, "V4L2 device registered as %s\n",
video_device_node_name(vfd));
return 0;
}
�û��ռ���õ��� v4l2_fops ���������ջ���õ� vivi_fops ��vivi_fops �е� ioctl ����video_ioctl2
static struct video_device vivi_template = {
.name	= "vivi",
.fops = &vivi_fops,
.ioctl_ops = &vivi_ioctl_ops,
.minor	= -1,
.release	= video_device_release,

.tvnorms = V4L2_STD_525_60,
.current_norm = V4L2_STD_NTSC_M,
};
video_register_device ���̾Ͳ���ϸ�����ˣ�ǰ��������з���������ž�������Ĳ�ע�� video_device �ṹ�壬���Ĳ�ע���ַ��豸���ṩһ��ͳһ�� fops �����û��ռ� read write ioctl �ȣ����ջ��ǻ���ת�� video_device->fops ,����һ����Ǻ��Ĳ�������ע������� video_device �ṹ����һ��ȫ�ֵ� video_device���顣
static const struct v4l2_file_operations vivi_fops = {
.owner	= THIS_MODULE,
.open = v4l2_fh_open,
.release = vivi_close,
.read = vivi_read,
.poll	= vivi_poll,
.unlocked_ioctl = video_ioctl2, /* V4L2 ioctl handler */
.mmap = vivi_mmap,
};
����ȿ�һ�� v4l2_fh_open ����
int v4l2_fh_open(struct file *filp)
{
// ǰ��ע��ʱ�����ǽ� video_device �ṹ�������ȫ������ video_device ������ͨ�� video_devdata ����ȡ���������澭���õ���������
struct video_device *vdev = video_devdata(filp);
// ����һ�� v4l2_fh �ṹ������file->private_data ��
struct v4l2_fh *fh = kzalloc(sizeof(*fh), GFP_KERNEL);
filp->private_data = fh;
if (fh == NULL)
return -ENOMEM;
v4l2_fh_init(fh, vdev);
v4l2_fh_add(fh);
return 0;
}
1��������ʱ����ͨ�� video_devdata ȡ������ע��� video_device �ṹ���в���
2��������ʱ����ͨ�� file->private_data ȡ�� v4l2_fh �ṹ����Ȼ���ڻ���֪������ɶ��
���������� ioctl ...��������һ�µ��ù���
long video_ioctl2(struct file *file,
unsigned int cmd, unsigned long arg)
{
return video_usercopy(file, cmd, arg, __video_do_ioctl);
}
static long __video_do_ioctl(struct file *file,
unsigned int cmd, void *arg)
{
struct video_device *vfd = video_devdata(file);
const struct v4l2_ioctl_ops *ops = vfd->ioctl_ops;
void *fh = file->private_data;
struct v4l2_fh *vfh = NULL;
int use_fh_prio = 0;
long ret = -ENOTTY;

if (ops == NULL) {
printk(KERN_WARNING "videodev: \"%s\" has no ioctl_ops.\n",
vfd->name);
return ret;
}

if (test_bit(V4L2_FL_USES_V4L2_FH, &vfd->flags)) {
vfh = file->private_data;
use_fh_prio = test_bit(V4L2_FL_USE_FH_PRIO, &vfd->flags);
}

if (v4l2_is_known_ioctl(cmd)) {
struct v4l2_ioctl_info *info = &v4l2_ioctls[_IOC_NR(cmd)];

if (!test_bit(_IOC_NR(cmd), vfd->valid_ioctls) &&
!((info->flags & INFO_FL_CTRL) && vfh && vfh->ctrl_handler))
return -ENOTTY;

if (use_fh_prio && (info->flags & INFO_FL_PRIO)) {
ret = v4l2_prio_check(vfd->prio, vfh->prio);
if (ret)
return ret;
}
}

if ((vfd->debug & V4L2_DEBUG_IOCTL) &&
!(vfd->debug & V4L2_DEBUG_IOCTL_ARG)) {
v4l_print_ioctl(vfd->name, cmd);
printk(KERN_CONT "\n");
}

switch (cmd) {

/* --- capabilities ------------------------------------------ */
case VIDIOC_QUERYCAP:
{
struct v4l2_capability *cap = (struct v4l2_capability *)arg;

cap->version = LINUX_VERSION_CODE;
ret = ops->vidioc_querycap(file, fh, cap);
if (!ret)
dbgarg(cmd, "driver=%s, card=%s, bus=%s, "
"version=0x%08x, "
"capabilities=0x%08x, "
"device_caps=0x%08x\n",
cap->driver, cap->card, cap->bus_info,
cap->version,
cap->capabilities,
cap->device_caps);
break;
}
vivi �����͸�������Щ ioctl �ϣ����水��Ӧ�ò��������Ľ���˳��������ķ�����Щ ioctl ��
����ioctl �������
Ӧ�ÿռ��һ����Ƶ app �������Ľ������̴�������ͼ��ʾ��
\
����͸������̣�����ÿһ�� ioctl �� vivi �еľ���ʵ�֡������ϵĹ��̳�͸���Լ�дһ����������ͷ����Ӧ�þͲ��������ˡ�
\
2.1VIDIOC_QUERYCAP ��ѯ�豸����
Ӧ�ò㣺
struct v4l2_capability {
__u8	driver[16];	/* i.e. "bttv" */
__u8	card[32];	/* i.e. "Hauppauge WinTV" */
__u8	bus_info[32];	/* "PCI:" + pci_name(pci_dev) */
__u32 version;  	/* should use KERNEL_VERSION() */
__u32	capabilities;	/* Device capabilities */
__u32	reserved[4];
};

struct v4l2_capability cap;
ret = ioctl(fd,VIDIOC_QUERYCAP,&cap);
if (ret < 0) {
LOG("VIDIOC_QUERYCAP failed (%d)\n", ret);
return ret;
}
�����㣺
void *fh = file->private_data;
ops->vidioc_querycap(file, fh, cap);
static int vidioc_querycap(struct file *file, void *priv, struct v4l2_capability *cap)
{
struct vivi_fh *fh = priv;
struct vivi_dev *dev = fh->dev;


// ����ֻ�ǽ�һЩ��Ϣд���û��ռ���ѣ��ǳ���
strcpy(cap->driver, "vivi"); 
strcpy(cap->card, "vivi");
strlcpy(cap->bus_info, dev->v4l2_dev.name, sizeof(cap->bus_info));
cap->version = VIVI_VERSION; cap->capabilities =V4L2_CAP_VIDEO_CAPTURE |V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;return 0;}
}
һ������ֻ���� capabilities ��Ա������V4L2_CAP_VIDEO_CAPTURE ������Ƶ���������������������£�
/* Values for &#39;capabilities&#39; field */
#define V4L2_CAP_VIDEO_CAPTURE	0x00000001 /* Is a video capture device */
#define V4L2_CAP_VIDEO_OUTPUT	0x00000002 /* Is a video output device */
#define V4L2_CAP_VIDEO_OVERLAY	0x00000004 /* Can do video overlay */
#define V4L2_CAP_VBI_CAPTURE	0x00000010 /* Is a raw VBI capture device */
#define V4L2_CAP_VBI_OUTPUT	0x00000020 /* Is a raw VBI output device */
#define V4L2_CAP_SLICED_VBI_CAPTURE	0x00000040 /* Is a sliced VBI capture device */
#define V4L2_CAP_SLICED_VBI_OUTPUT	0x00000080 /* Is a sliced VBI output device */
#define V4L2_CAP_RDS_CAPTURE	0x00000100 /* RDS data capture */
#define V4L2_CAP_VIDEO_OUTPUT_OVERLAY	0x00000200 /* Can do video output overlay */
#define V4L2_CAP_HW_FREQ_SEEK	0x00000400 /* Can do hardware frequency seek */
#define V4L2_CAP_RDS_OUTPUT	0x00000800 /* Is an RDS encoder */
2.2VIDIOC_ENUM_FMT ö��(��ѯ)�豸֧�ֵ���Ƶ��ʽ
Ӧ�ò㣺
struct v4l2_fmtdesc {
__u32	index; /* Format number */
enum v4l2_buf_type type; /* buffer type */
__u32 flags;
__u8	description[32]; /* Description string */
__u32	pixelformat; /* Format fourcc */
__u32	reserved[4];
};

struct v4l2_fmtdesc fmtdesc;
fmtdesc.index=0;
fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
{
printf("SUPPORT\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
fmtdesc.index++;
}
�����㣺
static struct vivi_fmt formats[] = {
{
.name = "4:2:2, packed, YUYV",
.fourcc = V4L2_PIX_FMT_YUYV,
.depth = 16,
},
...
}
static int vidioc_enum_fmt_vid_cap(struct file *file, void *priv,
struct v4l2_fmtdesc *f)
{
struct vivi_fmt *fmt;

if (f->index >= ARRAY_SIZE(formats))
return -EINVAL;

fmt = &formats[f->index];

strlcpy(f->description, fmt->name, sizeof(f->description));
f->pixelformat = fmt->fourcc;
return 0;
}
һ��һ���豸֧�ֶ�����Ƶ��ʽ������ vivi ����֧�ֵĸ�ʽ����� formats �����У�����Ӧ�ò㲢��֪���豸֧�ֶ����ָ�ʽ��Ҳ��֪��ĳ�ָ�ʽ���������ĸ��������У����ͨ��index��0��ʼ���ԣ�������������˵���Ǳ������е����������ÿһ��index��Ӧ����Ƶ��ʽ������ V4L2_PIX_FMT_YUYV .
2.3VIDIOC_S_FMT ������Ƶ��ʽ
Ӧ�ò㣺
struct v4l2_format {
enum v4l2_buf_type type;
union {
struct v4l2_pix_format	pix; /* V4L2_BUF_TYPE_VIDEO_CAPTURE */
struct v4l2_window	win; /* V4L2_BUF_TYPE_VIDEO_OVERLAY */
struct v4l2_vbi_format	vbi; /* V4L2_BUF_TYPE_VBI_CAPTURE */
struct v4l2_sliced_vbi_format	sliced; /* V4L2_BUF_TYPE_SLICED_VBI_CAPTURE */
__u8	raw_data[200]; /* user-defined */
} fmt;
};
struct v4l2_pix_format {
__u32 width;
__u32	height;
__u32	pixelformat;
enum v4l2_field field;
__u32 bytesperline;	/* for padding, zero if unused */
__u32 sizeimage;
enum v4l2_colorspace	colorspace;
__u32	priv;	/* private data, depends on pixelformat */
};
struct v4l2_format fmt;
memset(&fmt, 0, sizeof(fmt));
fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//��ʽ����
fmt.fmt.pix.width //���
fmt.fmt.pix.height //�߶�
fmt.fmt.pix.pixelformat = VIDEO_FORMAT;//��һ�������ǰ���ѯ������ĳ�ָ�ʽ����Ӧ vivi formats����
fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;//�����Ǹ���ɨ�����˼
ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
if (ret < 0) {
LOG("VIDIOC_S_FMT failed (%d)\n", ret);
return ret;
}
�����㣺
static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
struct v4l2_format *f)
{
struct vivi_dev *dev = video_drvdata(file);
struct vb2_queue *q = &dev->vb_vidq;

int ret = vidioc_try_fmt_vid_cap(file, priv, f);
//if (fmt->fourcc == f->fmt.pix.pixelformat)����formats[k]
dev->fmt = get_format(f);
dev->pixelsize = dev->fmt->depth / 8;
dev->width = f->fmt.pix.width;
dev->height = f->fmt.pix.height;
dev->field = f->fmt.pix.field;

return 0;
}
static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
struct v4l2_format *f)
{
struct vivi_dev *dev = video_drvdata(file);
struct vivi_fmt *fmt;
enum v4l2_field field;

fmt = get_format(f);

field = f->fmt.pix.field;

if (field == V4L2_FIELD_ANY) {
field = V4L2_FIELD_INTERLACED;
} 

f->fmt.pix.field = field;
v4l_bound_align_image(&f->fmt.pix.width, 48, MAX_WIDTH, 2,
&f->fmt.pix.height, 32, MAX_HEIGHT, 0, 0);
f->fmt.pix.bytesperline =
(f->fmt.pix.width * fmt->depth) >> 3;
f->fmt.pix.sizeimage =
f->fmt.pix.height * f->fmt.pix.bytesperline;
if (fmt->fourcc == V4L2_PIX_FMT_YUYV ||
fmt->fourcc == V4L2_PIX_FMT_UYVY)
f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
else
f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
return 0;
}
���ｫӦ�ò㴫��������Ƶ��ʽ�򵥴�����Ž���һ�� vivi_dev �ṹ��vivi_dev ���������أ�����һ��ʼ��ʱ�� vivi_create_instance ,���Ǵ�����һ�� video_device �ṹ�������ǵ��豸����������һ�� vivi_dev ��Ϊvideo_device->dev->privatedata ��֮�� register_video_device ���ں˻��Զ������ǵ� video_device ����ȫ������ video_device[] �С�
2.4VIDIOC_G_FMT ������úõ���Ƶ��ʽ
Ӧ�ò㣺
ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
if (ret < 0) {
LOG("VIDIOC_G_FMT failed (%d)\n", ret);
return ret;
}
// Print Stream Format
LOG("Stream Format Informations:\n");
LOG(" type: %d\n", fmt.type);
LOG(" width: %d\n", fmt.fmt.pix.width);
LOG(" height: %d\n", fmt.fmt.pix.height);
char fmtstr[8];
memset(fmtstr, 0, 8);
memcpy(fmtstr, &fmt.fmt.pix.pixelformat, 4);
LOG(" pixelformat: %s\n", fmtstr);
LOG(" field: %d\n", fmt.fmt.pix.field);
LOG(" bytesperline: %d\n", fmt.fmt.pix.bytesperline);
LOG(" sizeimage: %d\n", fmt.fmt.pix.sizeimage);
LOG(" colorspace: %d\n", fmt.fmt.pix.colorspace);
LOG(" priv: %d\n", fmt.fmt.pix.priv);
LOG(" raw_date: %s\n", fmt.fmt.raw_data);
�����㣺
static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
struct v4l2_format *f)
{
struct vivi_dev *dev = video_drvdata(file);
	// �Ѽ�¼�� vivi_dev �еĲ���д���û��ռ�
f->fmt.pix.width = dev->width;
f->fmt.pix.height = dev->height;
f->fmt.pix.field = dev->field;
f->fmt.pix.pixelformat = dev->fmt->fourcc;
f->fmt.pix.bytesperline =
(f->fmt.pix.width * dev->fmt->depth) >> 3;
f->fmt.pix.sizeimage =
f->fmt.pix.height * f->fmt.pix.bytesperline;
if (dev->fmt->fourcc == V4L2_PIX_FMT_YUYV ||
dev->fmt->fourcc == V4L2_PIX_FMT_UYVY)
f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
else
f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
return 0;
}
������֮ǰ���õĸ�ʽ���ض��ѡ�
2.5 VIDIOC_REQBUFS �������ں˿ռ������Ƶ������
������ڴ�λ���ں˿ռ�,Ӧ�ó����޷�ֱ�ӷ���,��Ҫͨ������mmap�ڴ�ӳ�亯��,���ں˿ռ���ڴ�ӳ�䵽�û��ռ�,Ӧ�òſ������û��ռ��ַ�������ں˿ռ䡣
Ӧ�ò㣺
struct v4l2_requestbuffers {
__u32	count;
__u32	type;	/* enum v4l2_buf_type */
__u32	memory;	/* enum v4l2_memory */
__u32	reserved[2];
};
struct v4l2_requestbuffers reqbuf;
reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
reqbuf.memory = V4L2_MEMORY_MMAP;
reqbuf.count = BUFFER_COUNT;
ret = ioctl(fd , VIDIOC_REQBUFS, &reqbuf);
if(ret < 0) {
LOG("VIDIOC_REQBUFS failed (%d)\n", ret);
return ret;
}
�����㣺
static int vidioc_reqbufs(struct file *file, void *priv,
struct v4l2_requestbuffers *p)
{
struct vivi_dev *dev = video_drvdata(file);
return vb2_reqbufs(&dev->vb_vidq, p);	//���Ĳ��ṩ�ı�׼����
}
vb_vidq �� vivi_dev ��һ����Ա��ǰ�������ᵽ�������� ops ��һ���� ops ��һ���� mem_ops
static struct vb2_ops vivi_video_qops = {
.queue_setup	= queue_setup,
.buf_init	= buffer_init,
.buf_prepare	= buffer_prepare,
.buf_finish	= buffer_finish,
.buf_cleanup	= buffer_cleanup,
.buf_queue	= buffer_queue,
.start_streaming= start_streaming,
.stop_streaming	= stop_streaming,
.wait_prepare	= vivi_unlock,
.wait_finish	= vivi_lock,
};
static int vidioc_reqbufs(struct file *file, void *priv,
struct v4l2_requestbuffers *p)
{
struct vivi_dev *dev = video_drvdata(file);
return vb2_reqbufs(&dev->vb_vidq, p);	//���Ĳ��ṩ�ı�׼����
}
int vb2_reqbufs(struct vb2_queue *q, struct v4l2_requestbuffers *req)
{
unsigned int num_buffers, allocated_buffers, num_planes = 0;
int ret = 0;
// �ж� re->count �Ƿ�С�� VIDEO_MAX_FRAME
num_buffers = min_t(unsigned int, req->count, VIDEO_MAX_FRAME);
memset(q->plane_sizes, 0, sizeof(q->plane_sizes));
memset(q->alloc_ctx, 0, sizeof(q->alloc_ctx));
q->memory = req->memory;

//(q)->ops->queue_setup(q,NULL,...)
ret = call_qop(q, queue_setup, q, NULL, &num_buffers, &num_planes,
q->plane_sizes, q->alloc_ctx);
/* Finally, allocate buffers and video memory */
ret = __vb2_queue_alloc(q, req->memory, num_buffers, num_planes);

allocated_buffers = ret;

q->num_buffers = allocated_buffers;
req->count = allocated_buffers;
return 0;
}
static int queue_setup(struct vb2_queue *vq, const struct v4l2_format *fmt,
unsigned int *nbuffers, unsigned int *nplanes,
unsigned int sizes[], void *alloc_ctxs[])
{
struct vivi_dev *dev = vb2_get_drv_priv(vq);
unsigned long size;
// ÿһ��buffer �Ĵ�С
size = dev->width * dev->height * dev->pixelsize;
if (0 == *nbuffers)
*nbuffers = 32;
// ��������buffer���࣬���¿ռ䲻������buffer
while (size * *nbuffers > vid_limit * 1024 * 1024)
(*nbuffers)--;
*nplanes = 1;
// ���ܴ�С���� vivi_dev->vb_vidq->plane_size[0]
sizes[0] = size;
return 0;
}
static int __vb2_queue_alloc(struct vb2_queue *q, enum v4l2_memory memory,
unsigned int num_buffers, unsigned int num_planes)
{
unsigned int buffer;
struct vb2_buffer *vb;
int ret;
// ������ vb2_buffer ��䲢���� vivi_dev->vb_vidq->bufs[]
for (buffer = 0; buffer < num_buffers; ++buffer) {
/* Allocate videobuf buffer structures */
vb = kzalloc(q->buf_struct_size, GFP_KERNEL);

/* Length stores number of planes for multiplanar buffers */
if (V4L2_TYPE_IS_MULTIPLANAR(q->type))
vb->v4l2_buf.length = num_planes;

vb->state = VB2_BUF_STATE_DEQUEUED;
vb->vb2_queue = q;
vb->num_planes = num_planes;
vb->v4l2_buf.index = q->num_buffers + buffer;
vb->v4l2_buf.type = q->type;
vb->v4l2_buf.memory = memory;

/* Allocate video buffer memory for the MMAP type */
if (memory == V4L2_MEMORY_MMAP) {
ret = __vb2_buf_mem_alloc(vb);//�����ṩ�ı�׼����
ret = call_qop(q, buf_init, vb);//q->ops->buf_init
}

q->bufs[q->num_buffers + buffer] = vb;
}
__setup_offsets(q, buffer);
return buffer;
}
static int __vb2_buf_mem_alloc(struct vb2_buffer *vb)
{
struct vb2_queue *q = vb->vb2_queue;
void *mem_priv;
int plane;

/* num_planes == 1 */
for (plane = 0; plane < vb->num_planes; ++plane) {
mem_priv = call_memop(q, alloc, q->alloc_ctx[plane],
q->plane_sizes[plane]);

/* Associate allocator private data with this plane */
vb->planes[plane].mem_priv = mem_priv;
vb->v4l2_planes[plane].length = q->[plane];
}

return 0;
}
static void *vb2_vmalloc_alloc(void *alloc_ctx, unsigned long size)
{
struct vb2_vmalloc_buf *buf;

buf = kzalloc(sizeof(*buf), GFP_KERNEL);

buf->size = size;
// ����ռ�
buf->vaddr = vmalloc_user(buf->size);
buf->handler.refcount = &buf->refcount;
buf->handler.put = vb2_vmalloc_put;
buf->handler.arg = buf;

atomic_inc(&buf->refcount);
return buf;
}
2.6 VIDIOC_QUERYBUF ��ѯ����õ� buffer ��Ϣ
��ѯ�Ѿ�����õ�V4L2��Ƶ�������������Ϣ,������������ʹ��״̬�����ں˿ռ��ƫ�Ƶ�ַ�����������ȵ�,Ȼ��Ӧ�ó��������Щ��Ϣʹ��mmap���ں˿ռ��ַӳ�䵽�û��ռ䡣
Ӧ�ò㣺
struct v4l2_buffer {
__u32	index;
enum v4l2_buf_type type;
__u32	bytesused;
__u32	flags;
enum v4l2_field	field;
struct timeval	timestamp;
struct v4l2_timecode	timecode;
__u32	sequence;

/* memory location */
enum v4l2_memory memory;
union {
__u32 offset;
unsigned long userptr;
} m;
__u32	length;
__u32	input;
__u32	reserved;
};
v4l2_buffer buf;
buf.index = i;
buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
buf.memory = V4L2_MEMORY_MMAP;
ret = ioctl(fd , VIDIOC_QUERYBUF, &buf);
if(ret < 0) {
LOG("VIDIOC_QUERYBUF (%d) failed (%d)\n", i, ret);
return ret;
}
�����㣺
ops->vidioc_querybuf(file, fh, p);
static int vidioc_querybuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
struct vivi_dev *dev = video_drvdata(file);
return vb2_querybuf(&dev->vb_vidq, p);
}
int vb2_querybuf(struct vb2_queue *q, struct v4l2_buffer *b)
{
struct vb2_buffer *vb;
// ȡ�� buf
vb = q->bufs[b->index];
// �� buf ��Ϣд���û��ռ䴫�ݵ� b
return __fill_v4l2_buffer(vb, b);
}
static int __fill_v4l2_buffer(struct vb2_buffer *vb, struct v4l2_buffer *b)
{
struct vb2_queue *q = vb->vb2_queue;
int ret;

/* Copy back data such as timestamp, flags, input, etc. */
memcpy(b, &vb->v4l2_buf, offsetof(struct v4l2_buffer, m));
b->input = vb->v4l2_buf.input;
b->reserved = vb->v4l2_buf.reserved;

if (V4L2_TYPE_IS_MULTIPLANAR(q->type)) {
ret = __verify_planes_array(vb, b);
if (ret)
return ret;

/*
* Fill in plane-related data if userspace provided an array
* for it. The memory and size is verified above.
*/
memcpy(b->m.planes, vb->v4l2_planes,
b->length * sizeof(struct v4l2_plane));

if (q->memory == V4L2_MEMORY_DMABUF) {
unsigned int plane;
for (plane = 0; plane < vb->num_planes; ++plane)
b->m.planes[plane].m.fd = 0;
}
} else {
/*
* We use length and offset in v4l2_planes array even for
* single-planar buffers, but userspace does not.
*/
b->length = vb->v4l2_planes[0].length;
b->bytesused = vb->v4l2_planes[0].bytesused;
if (q->memory == V4L2_MEMORY_MMAP)
b->m.offset = vb->v4l2_planes[0].m.mem_offset;
else if (q->memory == V4L2_MEMORY_USERPTR)
b->m.userptr = vb->v4l2_planes[0].m.userptr;
else if (q->memory == V4L2_MEMORY_DMABUF)
b->m.fd = 0;
}

/*
* Clear any buffer state related flags.
*/
b->flags &= ~V4L2_BUFFER_STATE_FLAGS;

switch (vb->state) {
case VB2_BUF_STATE_QUEUED:
case VB2_BUF_STATE_ACTIVE:
b->flags |= V4L2_BUF_FLAG_QUEUED;
break;
case VB2_BUF_STATE_ERROR:
b->flags |= V4L2_BUF_FLAG_ERROR;
/* fall through */
case VB2_BUF_STATE_DONE:
b->flags |= V4L2_BUF_FLAG_DONE;
break;
case VB2_BUF_STATE_PREPARED:
b->flags |= V4L2_BUF_FLAG_PREPARED;
break;
case VB2_BUF_STATE_DEQUEUED:
/* nothing */
break;
}

if (__buffer_in_use(q, vb))
b->flags |= V4L2_BUF_FLAG_MAPPED;

return 0;
}
2.7 mmap
Ӧ�ò㣺
v4l2_buffer framebuf[]
framebuf[i].length = buf.length;
framebuf[i].start = (char *) mmap(
NULL, // ��ָ���ڴ����ʼ��ַ��һ��ΪNULL,��ʾϵͳ�Զ�����
buf.length,	//ӳ�䳤��
PROT_READ|PROT_WRITE, //�ɶ���д
MAP_SHARED, //��ӳ�����Ķ�д��д���ں˿ռ䣬������������ӳ����ں˿ռ��ַ�Ľ��̹���
fd, 
buf.m.offset
);
if (framebuf[i].start == MAP_FAILED) {
LOG("mmap (%d) failed: %s\n", i, strerror(errno));
return -1;
}
�����㣺
static int vivi_mmap(struct file *file, struct vm_area_struct *vma)
{
struct vivi_dev *dev = video_drvdata(file);
int ret;
ret = vb2_mmap(&dev->vb_vidq, vma);//���Ĳ��ṩ�ĺ���
return ret;
}
2.8 VIDIOC_QBUF
Ͷ��һ���յ���Ƶ����������Ƶ������������У�ִ�гɹ�����������Ƶ�豸����ͼ��ʱ����Ӧ����Ƶ���ݱ����浽��Ƶ���������Ӧ����Ƶ�������С�
Ӧ�ò㣺
ret = ioctl(fd , VIDIOC_QBUF, &buf);
if (ret < 0) {
LOG("VIDIOC_QBUF (%d) failed (%d)\n", i, ret);
return -1;
�����㣺
static int vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
struct vivi_dev *dev = video_drvdata(file);
return vb2_qbuf(&dev->vb_vidq, p);
}
int vb2_qbuf(struct vb2_queue *q, struct v4l2_buffer *b)
{
struct rw_semaphore *mmap_sem = NULL;
struct vb2_buffer *vb;
int ret = 0;

vb = q->bufs[b->index];

switch (vb->state) {
case VB2_BUF_STATE_DEQUEUED:
ret = __buf_prepare(vb, b);
}
// ����� buffer ���� q->queued_list
list_add_tail(&vb->queued_entry, &q->queued_list);
vb->state = VB2_BUF_STATE_QUEUED;

if (q->streaming)
__enqueue_in_driver(vb);

/* Fill buffer information for the userspace */
__fill_v4l2_buffer(vb, b);

unlock:
if (mmap_sem)
up_read(mmap_sem);
return ret;
}
ʵ���Ͼ���ȡ��һ�� vb2_buffer ���� vivi_dev->vb_vidq->queued_list
2.9 VIDIOC_STREAMON
Ӧ�ò㣺
enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
ret = ioctl(fd, VIDIOC_STREAMON, &type);
if (ret < 0) {
LOG("VIDIOC_STREAMON failed (%d)\n", ret);
return ret;
}
�����㣺
static int vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
struct vivi_dev *dev = video_drvdata(file);
return vb2_streamon(&dev->vb_vidq, i);
}
int vb2_streamon(struct vb2_queue *q, enum v4l2_buf_type type)
{
struct vb2_buffer *vb;
int ret;
vb->state = VB2_BUF_STATE_ACTIVE;
// �� queued_list ������ȡ�� buffer prepare һ�� Ȼ���ڷ��� vidq->active ����
list_for_each_entry(vb, &q->queued_list, queued_entry)
__enqueue_in_driver(vb);
//	for (plane = 0; plane < vb->num_planes; ++plane)
//	call_memop(q, prepare, vb->planes[plane].mem_priv);
//	q->ops->buf_queue(vb);
//q->ops->start_streaming
ret = call_qop(q, start_streaming, q, atomic_read(&q->queued_count));

q->streaming = 1;
return 0;
}
static void buffer_queue(struct vb2_buffer *vb)
{
struct vivi_dev *dev = vb2_get_drv_priv(vb->vb2_queue);
struct vivi_buffer *buf = container_of(vb, struct vivi_buffer, vb);
struct vivi_dmaqueue *vidq = &dev->vidq;
unsigned long flags = 0;

spin_lock_irqsave(&dev->slock, flags);
// �� buffer ���� vivi_dev->vidq->active ����
list_add_tail(&buf->list, &vidq->active);
spin_unlock_irqrestore(&dev->slock, flags);
}
static int start_streaming(struct vb2_queue *vq, unsigned int count)
{
struct vivi_dev *dev = vb2_get_drv_priv(vq);
dprintk(dev, 1, "%s\n", __func__);
return vivi_start_generating(dev);
}
static int vivi_start_generating(struct vivi_dev *dev)
{
struct vivi_dmaqueue *dma_q = &dev->vidq;

/* Resets frame counters */
dev->ms = 0;
dev->mv_count = 0;
dev->jiffies = jiffies;

dma_q->frame = 0;
dma_q->ini_jiffies = jiffies;
// ����һ���ں��̣߳���ں��� vivi_thread
dma_q->kthread = kthread_run(vivi_thread, dev, dev->v4l2_dev.name);

/* Wakes thread */
wake_up_interruptible(&dma_q->wq);

return 0;
}
static int vivi_thread(void *data)
{
struct vivi_dev *dev = data;

dprintk(dev, 1, "thread started\n");

set_freezable();

for (;;) {
vivi_sleep(dev);

if (kthread_should_stop())
break;
}
dprintk(dev, 1, "thread: exit\n");
return 0;
}
static void vivi_sleep(struct vivi_dev *dev)
{
struct vivi_dmaqueue *dma_q = &dev->vidq;
int timeout;
DECLARE_WAITQUEUE(wait, current);

add_wait_queue(&dma_q->wq, &wait);
if (kthread_should_stop())
goto stop_task;

/* Calculate time to wake up */
timeout = msecs_to_jiffies(frames_to_ms(1));

vivi_thread_tick(dev);

schedule_timeout_interruptible(timeout);

stop_task:
remove_wait_queue(&dma_q->wq, &wait);
try_to_freeze();
}
ÿ�ε��� vivi_sleep ����̶߳�������ȴ����У����� vivi_thread_tick ������ݣ�Ȼ������ָ����ʱ���Զ����ѣ�һֱѭ����ȥ��������������һ֡һ֡����Ƶ���ݡ�
static void vivi_thread_tick(struct vivi_dev *dev)
{
struct vivi_dmaqueue *dma_q = &dev->vidq;
struct vivi_buffer *buf;
unsigned long flags = 0;

spin_lock_irqsave(&dev->slock, flags);

buf = list_entry(dma_q->active.next, struct vivi_buffer, list);
list_del(&buf->list);
spin_unlock_irqrestore(&dev->slock, flags);

do_gettimeofday(&buf->vb.v4l2_buf.timestamp);

/* ���Buffer */
vivi_fillbuff(dev, buf);

vb2_buffer_done(&buf->vb, VB2_BUF_STATE_DONE);
}
void vb2_buffer_done(struct vb2_buffer *vb, enum vb2_buffer_state state)
{
struct vb2_queue *q = vb->vb2_queue;
unsigned long flags;
unsigned int plane;

/* sync buffers */
for (plane = 0; plane < vb->num_planes; ++plane)
call_memop(q, finish, vb->planes[plane].mem_priv);

/* Add the buffer to the done buffers list */
spin_lock_irqsave(&q->done_lock, flags);
vb->state = state;
list_add_tail(&vb->done_entry, &q->done_list);
atomic_dec(&q->queued_count);
#ifdef CONFIG_SYNC
sw_sync_timeline_inc(q->timeline, 1);
#endif
spin_unlock_irqrestore(&q->done_lock, flags);

/* Ӧ�ó���select ʱ poll_wait �����ߣ������������˻��� */
wake_up(&q->done_wq);
}
��ʼ��ʱ�����ǽ���һ�� vb_buffer ���� vb_vidq->queued_list ����������Ƶ����֮������ȡ������ vb_vidq->vidq->active ���У�Ȼ�����ں��߳���ÿһ�� tick ���ֽ���ȡ�������Ƶ����֮���ٹ��� vb_vidq->done_list �������������ߵȴ���Ƶ���ݵ�Ӧ�ó���
2.10 select
�����㣺
vivi_poll(struct file *file, struct poll_table_struct *wait)
{
struct vivi_dev *dev = video_drvdata(file);
struct vb2_queue *q = &dev->vb_vidq;

return vb2_poll(q, file, wait);
}
unsigned int vb2_poll(struct vb2_queue *q, struct file *file, poll_table *wait)
{
// �������߶��У��Ƿ����߻�Ҫ������ֵ�����û�����ݾ����ߣ������ݾͲ�����
poll_wait(file, &q->done_wq, wait);

if (!list_empty(&q->done_list))
vb = list_first_entry(&q->done_list, struct vb2_buffer,
done_entry);
spin_unlock_irqrestore(&q->done_lock, flags);

if (vb && (vb->state == VB2_BUF_STATE_DONE
|| vb->state == VB2_BUF_STATE_ERROR)) {
return (V4L2_TYPE_IS_OUTPUT(q->type)) ?
res | POLLOUT | POLLWRNORM :
res | POLLIN | POLLRDNORM;
}
return res;
}
����֮�����ǾͿ���ȥ����Ƶ���������ȡ��buffer��Ȼ�����ӳ���ϵ����Ӧ�ÿռ�ȡ����Ƶ������
2.11 VIDIOC_DQBUF
Ӧ�ò㣺
ret = ioctl(fd, VIDIOC_DQBUF, &buf);
if (ret < 0) {
LOG("VIDIOC_DQBUF failed (%d)\n", ret);
return ret;
}
static int vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
struct vivi_dev *dev = video_drvdata(file);
return vb2_dqbuf(&dev->vb_vidq, p, file->f_flags & O_NONBLOCK);
}
int vb2_dqbuf(struct vb2_queue *q, struct v4l2_buffer *b, bool nonblocking)
{
struct vb2_buffer *vb = NULL;
int ret;
// �ȴ��� q->done_list ȡ����һ�����õ� buffer
ret = __vb2_get_done_vb(q, &vb, nonblocking);

ret = call_qop(q, buf_finish, vb);

/* д��buffer����Ϣ���û��ռ䣬Ӧ�ó����Ҹ����buffer��mmap֮��ĵ�ַ������ */
__fill_v4l2_buffer(vb, b);
/* Remove from videobuf queue */
list_del(&vb->queued_entry);

vb->state = VB2_BUF_STATE_DEQUEUED;
return 0;
}
static int __vb2_get_done_vb(struct vb2_queue *q, struct vb2_buffer **vb,int nonblocking)
{
unsigned long flags;
int ret;
/*
* Wait for at least one buffer to become available on the done_list.
*/
ret = __vb2_wait_for_done_vb(q, nonblocking);


spin_lock_irqsave(&q->done_lock, flags);
*vb = list_first_entry(&q->done_list, struct vb2_buffer, done_entry);
list_del(&(*vb)->done_entry);
spin_unlock_irqrestore(&q->done_lock, flags);
return 0;
}
static int buffer_finish(struct vb2_buffer *vb)
{
struct vivi_dev *dev = vb2_get_drv_priv(vb->vb2_queue);
dprintk(dev, 1, "%s\n", __func__);
return 0;
}