/************************************************************
** �������� 
*************************************************************/

/* input �豸 */

/* ����input�豸�ṹ�� */
static struct input_dev *button_dev;  
struct input_dev *input_allocate_device(void)

/* ��ʼ��input_dev�ṹ�� */
//��Ҫ�����¼�����(type)���¼���(code)
void set_bit(unsigned int bit, volatile unsigned long *p)
void clear_bit(unsigned int bit, volatile unsigned long *p)
set_bit(EV_ABS, input_dev->evbit);
set_bit(ABS_X, input_dev->absbit);

/* ע��input�豸 */
int input_register_device(struct input_dev *dev)

/*************************************************************/
//�����¼�
void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
static inline void input_mt_sync(struct input_dev *dev)		//��㴥������ÿ����һ������Ӹú�������������input_sync
static inline void input_sync(struct input_dev *dev)
{
	input_event(dev, EV_SYN, SYN_REPORT, 0);
}
example:
static inline void input_report_abs(struct input_dev *dev, unsigned int code, int value)
{
	input_event(dev, EV_ABS, code, value);
}
/*************************************************************/
/* Event types */

#define EV_SYN			0x00	//ͬ���¼�
#define EV_KEY			0x01	//�����¼�
#define EV_REL			0x02	//�������(�磺����ƶ�������������һ��λ�õ�ƫ��)
#define EV_ABS			0x03	//��������(�磺������������ˣ�������Ե�����λ��)
#define EV_MSC			0x04	//����
#define EV_SW				0x05	//����
#define EV_LED			0x11	//����/�豸��
#define EV_SND			0x12	//����/����
#define EV_REP			0x14	//�ظ�
#define EV_FF				0x15	//������
#define EV_PWR			0x16	//��Դ
#define EV_FF_STATUS		0x17	// ������״̬
#define EV_MAX			0x1f			//�¼��������������ṩλ����֧��
#define EV_CNT			(EV_MAX+1)

/* input�ṹ���е�Ҫ����ʼ����λ */
unsigned long evbit[BITS_TO_LONGS(EV_CNT)];
unsigned long keybit[BITS_TO_LONGS(KEY_CNT)];
unsigned long relbit[BITS_TO_LONGS(REL_CNT)];
unsigned long absbit[BITS_TO_LONGS(ABS_CNT)];

unsigned long key[BITS_TO_LONGS(KEY_CNT)];
unsigned long led[BITS_TO_LONGS(LED_CNT)];

/*************************************************************/
//��㴥����input�ṹ���ʼ��
set_bit(ABS_MT_TOUCH_MAJOR, input_dev->absbit);
set_bit(ABS_MT_POSITION_X, input_dev->absbit);
set_bit(ABS_MT_POSITION_Y, input_dev->absbit);
set_bit(ABS_MT_WIDTH_MAJOR, input_dev->absbit);	

input_set_abs_params(input_dev,
		     ABS_MT_POSITION_X, 0, SCREEN_MAX_X, 0, 0);
input_set_abs_params(input_dev,
		     ABS_MT_POSITION_Y, 0, SCREEN_MAX_Y, 0, 0);
input_set_abs_params(input_dev,
		     ABS_MT_TOUCH_MAJOR, 0, PRESS_MAX, 0, 0);
input_set_abs_params(input_dev,
		     ABS_MT_WIDTH_MAJOR, 0, 200, 0, 0);
input_set_abs_params(input_dev,
		     ABS_MT_TRACKING_ID, 0, 4, 0, 0);
			     
//������input�ṹ���ʼ��
set_bit(ABS_X, input_dev->absbit);
set_bit(ABS_Y, input_dev->absbit);
set_bit(ABS_PRESSURE, input_dev->absbit);
set_bit(BTN_TOUCH, input_dev->keybit);
input_set_abs_params(input_dev, ABS_X, 0, SCREEN_MAX_X, 0, 0);
input_set_abs_params(input_dev, ABS_Y, 0, SCREEN_MAX_Y, 0, 0);
input_set_abs_params(input_dev,
		     ABS_PRESSURE, 0, PRESS_MAX, 0 , 0);
		     
/*************************************************************/