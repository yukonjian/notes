/************************************************************
** 驱动部分 
*************************************************************/

/* input 设备 */

/* 分配input设备结构体 */
static struct input_dev *button_dev;  
struct input_dev *input_allocate_device(void)

/* 初始化input_dev结构体 */
//需要设置事件类型(type)和事件码(code)
void set_bit(unsigned int bit, volatile unsigned long *p)
void clear_bit(unsigned int bit, volatile unsigned long *p)
set_bit(EV_ABS, input_dev->evbit);
set_bit(ABS_X, input_dev->absbit);

/* 注册input设备 */
int input_register_device(struct input_dev *dev)

/*************************************************************/
//报告事件
void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
static inline void input_mt_sync(struct input_dev *dev)		//多点触摸屏，每报告一个点需加该函数，报告完后加input_sync
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

#define EV_SYN			0x00	//同步事件
#define EV_KEY			0x01	//按键事件
#define EV_REL			0x02	//相对坐标(如：鼠标移动，报告相对最后一次位置的偏移)
#define EV_ABS			0x03	//绝对坐标(如：触摸屏或操作杆，报告绝对的坐标位置)
#define EV_MSC			0x04	//其它
#define EV_SW				0x05	//开关
#define EV_LED			0x11	//按键/设备灯
#define EV_SND			0x12	//声音/警报
#define EV_REP			0x14	//重复
#define EV_FF				0x15	//力反馈
#define EV_PWR			0x16	//电源
#define EV_FF_STATUS		0x17	// 力反馈状态
#define EV_MAX			0x1f			//事件类型最大个数和提供位掩码支持
#define EV_CNT			(EV_MAX+1)

/* input结构体中的要被初始化的位 */
unsigned long evbit[BITS_TO_LONGS(EV_CNT)];
unsigned long keybit[BITS_TO_LONGS(KEY_CNT)];
unsigned long relbit[BITS_TO_LONGS(REL_CNT)];
unsigned long absbit[BITS_TO_LONGS(ABS_CNT)];

unsigned long key[BITS_TO_LONGS(KEY_CNT)];
unsigned long led[BITS_TO_LONGS(LED_CNT)];

/*************************************************************/
//多点触摸屏input结构体初始化
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
			     
//触摸屏input结构体初始化
set_bit(ABS_X, input_dev->absbit);
set_bit(ABS_Y, input_dev->absbit);
set_bit(ABS_PRESSURE, input_dev->absbit);
set_bit(BTN_TOUCH, input_dev->keybit);
input_set_abs_params(input_dev, ABS_X, 0, SCREEN_MAX_X, 0, 0);
input_set_abs_params(input_dev, ABS_Y, 0, SCREEN_MAX_Y, 0, 0);
input_set_abs_params(input_dev,
		     ABS_PRESSURE, 0, PRESS_MAX, 0 , 0);
		     
/*************************************************************/