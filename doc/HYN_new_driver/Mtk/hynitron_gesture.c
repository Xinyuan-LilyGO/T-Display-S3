/**
 *Name        : cst0xx_getsure.c
 *Author      : gary
 *Version     : V1.0
 *Create      : 2018-1-23
 *Copyright   : zxzz
 */

#include "hynitron_core.h"
#include "hynitron_config.h"


#if HYN_GESTURE_EN

#define  HYN_GESTURE_CAP_CST8XX   0
/******************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/
#define  KEY_GESTURE_U                          KEY_U
#define  KEY_GESTURE_UP                         KEY_UP
#define  KEY_GESTURE_DOWN                       KEY_DOWN
#define  KEY_GESTURE_LEFT                       KEY_LEFT
#define  KEY_GESTURE_RIGHT                      KEY_RIGHT
#define  KEY_GESTURE_O                          KEY_O
#define  KEY_GESTURE_E                          KEY_E
#define  KEY_GESTURE_M                          KEY_M
#define  KEY_GESTURE_L                          KEY_L
#define  KEY_GESTURE_W                          KEY_W
#define  KEY_GESTURE_S                          KEY_S
#define  KEY_GESTURE_V                          KEY_V
#define  KEY_GESTURE_C                          KEY_C
#define  KEY_GESTURE_Z                          KEY_Z
#define  KEY_GESTURE_DOUBLECLICK				KEY_POWER

#if HYN_GESTURE_CAP_CST8XX
//cap 
#define GESTURE_LEFT                            0x20
#define GESTURE_RIGHT                           0x21
#define GESTURE_UP                              0x22
#define GESTURE_DOWN                            0x23
#define GESTURE_DOUBLECLICK                     0x24
#define GESTURE_O                               0x30
#define GESTURE_W                               0x31
#define GESTURE_M                               0x32
#define GESTURE_E                               0x33
#define GESTURE_S                               0x46
#define GESTURE_V                               0x54
#define GESTURE_Z                               0x65
#define GESTURE_C                               0x34
#else
//mut
#define GESTURE_LEFT                            3
#define GESTURE_RIGHT                           1
#define GESTURE_UP                              2
#define GESTURE_DOWN                            4
#define GESTURE_DOUBLECLICK                     32
#define GESTURE_O                               5
#define GESTURE_O_1                             20
#define GESTURE_W                               10
#define GESTURE_M                               8
#define GESTURE_M_1                             9
#define GESTURE_M_2                             15
#define GESTURE_M_3                             16
#define GESTURE_E                               7
#define GESTURE_E_1                             17
#define GESTURE_E_2                             19
#define GESTURE_S                               12
#define GESTURE_S_1                             18
#define GESTURE_V                               11
#define GESTURE_Z                               13
#define GESTURE_C                               6
#define GESTURE_C_1                             14
#endif

#define HYN_GESTRUE_POINTS                      255
#define HYN_GESTRUE_POINTS_HEADER               8
#define HYN_GESTURE_OUTPUT_ADRESS               0xD3
/*****************************************************************************
* Private enumerations, structures and unions using typedef
*****************************************************************************/
struct hyn_gesture_st
{
    u8 header[HYN_GESTRUE_POINTS_HEADER];
    u16 coordinate_x[HYN_GESTRUE_POINTS];
    u16 coordinate_y[HYN_GESTRUE_POINTS];
	u16 report_key;
	u8  gestrue_id;
    u8  mode;
    u8  active;              /* 1-enter into gesture(suspend) 0-gesture disable or LCD on */
};

/*****************************************************************************
* Static variables
*****************************************************************************/
static struct hyn_gesture_st hyn_gesture_data;

/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/

/*****************************************************************************
* Static function prototypes
*****************************************************************************/
static ssize_t hyn_gesture_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t hyn_gesture_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static ssize_t hyn_gesture_buf_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t hyn_gesture_buf_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/* sysfs gesture node
 *   read example: cat  hyn_gesture_mode        ---read gesture mode
 *   write example:echo 01 > hyn_gesture_mode   ---write gesture mode to 01
 *
 */
static DEVICE_ATTR (hyn_gesture_mode, S_IRUGO|S_IWUSR, hyn_gesture_show, hyn_gesture_store);
/*
 *   read example: cat  hyn_gesture_buf        ---read gesture buf
 */
static DEVICE_ATTR (hyn_gesture_buf, S_IRUGO|S_IWUSR, hyn_gesture_buf_show, hyn_gesture_buf_store);
static struct attribute *hyn_gesture_mode_attrs[] =
{

    &dev_attr_hyn_gesture_mode.attr,
    &dev_attr_hyn_gesture_buf.attr,
    NULL,
};

static struct attribute_group hyn_gesture_group =
{
    .attrs = hyn_gesture_mode_attrs,
};


/************************************************************************
* Name: hyn_gesture_show
* Brief:  no
* Input: device, device attribute, char buf
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_gesture_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    //struct i2c_client *client = container_of(dev, struct i2c_client, dev);
	HYN_FUNC_ENTER();
    mutex_lock(&hyn_ts_data->input_dev->mutex);
    count = sprintf(buf, "Gesture Mode: %s\n", hyn_gesture_data.mode ? "On" : "Off");
    count += sprintf(buf + count, "Reg = %d\n", hyn_gesture_data.gestrue_id);
    mutex_unlock(&hyn_ts_data->input_dev->mutex);
	HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_gesture_store
* Brief:  no
* Input: device, device attribute, char buf, char count
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_gesture_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	HYN_FUNC_ENTER();
	mutex_lock(&hyn_ts_data->input_dev->mutex);

    if (buf[0]==1){
        HYN_INFO("[GESTURE]enable gesture");
        hyn_gesture_data.mode = ENABLE;
    }else if (buf[0]==0){
        HYN_INFO("[GESTURE]disable gesture");
        hyn_gesture_data.mode = DISABLE;
    }
    mutex_unlock(&hyn_ts_data->input_dev->mutex);

	HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_gesture_buf_show
* Brief:  no
* Input: device, device attribute, char buf
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_gesture_buf_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    int i = 0;
	HYN_FUNC_ENTER();
	
    mutex_lock(&hyn_ts_data->input_dev->mutex);
    count = snprintf(buf, PAGE_SIZE, "Gesture ID: 0x%x\n", hyn_gesture_data.header[0]);
    count += snprintf(buf + count, PAGE_SIZE, "Gesture PointNum: %d\n", hyn_gesture_data.header[1]);
    count += snprintf(buf + count, PAGE_SIZE, "Gesture Point Buf:\n");
    for (i = 0; i < hyn_gesture_data.header[1]; i++)
    {
        count += snprintf(buf + count, PAGE_SIZE, "%3d(%4d,%4d) ", i, hyn_gesture_data.coordinate_x[i], hyn_gesture_data.coordinate_y[i]);
        if ((i + 1)%4 == 0)
            count += snprintf(buf + count, PAGE_SIZE, "\n");
    }
    count += snprintf(buf + count, PAGE_SIZE, "\n");
    mutex_unlock(&hyn_ts_data->input_dev->mutex);

	HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_gesture_buf_store
* Brief:  no
* Input: device, device attribute, char buf, char count
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_gesture_buf_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    /* place holder for future use */
    return -EPERM;
}

/*****************************************************************************
*   Name: hyn_create_gesture_sysfs
*  Brief:
*  Input:
* Output: None
* Return: 0-success or error
*****************************************************************************/
int hyn_create_gesture_sysfs(struct i2c_client *client)
{
    int ret = 0;
	HYN_FUNC_ENTER();

	if(hyn_ts_data->k_obj==NULL)
	return 0;
		
    ret = sysfs_create_group(hyn_ts_data->k_obj, &hyn_gesture_group);
    if ( ret != 0){
        HYN_ERROR( "[GESTURE]hyn_gesture_mode_group(sysfs) create failed!");
        sysfs_remove_group(hyn_ts_data->k_obj, &hyn_gesture_group);
        return ret;
    }
	HYN_FUNC_EXIT();
    return 0;
}

/*****************************************************************************
*   Name: hyn_gesture_recovery
*  Brief: recovery gesture state when reset
*  Input:
* Output: None
* Return:
*****************************************************************************/
void hyn_gesture_recovery(struct i2c_client *client)
{
	HYN_FUNC_ENTER();
    if (hyn_gesture_data.mode && hyn_gesture_data.active)
    {
		if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){
			u8 buf[3];
			buf[0]=HYN_REG_MUT_GESTURE_EN>>8;
			buf[1]=HYN_REG_MUT_GESTURE_EN&0xff;
			buf[2]=0x80;
			cst3xx_i2c_write(client, buf, 3);
		}else{
	        hyn_i2c_write_byte(client, HYN_REG_CAP_GESTURE_EN, ENABLE);
		}
    }
	HYN_FUNC_EXIT();
}

/*****************************************************************************
*   Name: hyn_check_gesture
*  Brief:
*  Input:
* Output: None
* Return: None
*****************************************************************************/
static void hyn_check_gesture(struct input_dev *input_dev,int gesture_id)
{
	unsigned int report_key = 0;
	
    HYN_INFO("check_gesture ======================gesture_id==0x%x   ================\n ",gesture_id);
    HYN_FUNC_ENTER();
    switch (gesture_id)
    {
		
		case GESTURE_O:
#if (!HYN_GESTURE_CAP_CST8XX)
		case GESTURE_O_1:
#endif
			report_key = KEY_GESTURE_O;
			break;
		case GESTURE_W:
			report_key = KEY_GESTURE_W;    
			break;
		case GESTURE_M:
#if (!HYN_GESTURE_CAP_CST8XX)
		case GESTURE_M_1:
		case GESTURE_M_2:
		case GESTURE_M_3:
#endif
			report_key = KEY_GESTURE_M;
			break;
		case GESTURE_E:
#if (!HYN_GESTURE_CAP_CST8XX)
		case GESTURE_E_1:
		case GESTURE_E_2:
#endif
			report_key = KEY_GESTURE_E;
			break;
		case GESTURE_C:
#if (!HYN_GESTURE_CAP_CST8XX)
		case GESTURE_C_1:
#endif
			report_key = KEY_GESTURE_C;
			break;
		case GESTURE_S:
#if (!HYN_GESTURE_CAP_CST8XX)
		case GESTURE_S_1:
#endif
			report_key = KEY_GESTURE_S;
			break ;
		case GESTURE_V:
			report_key = KEY_GESTURE_V;
			break;
		case GESTURE_Z:
			report_key = KEY_GESTURE_Z;
			break;
        case GESTURE_RIGHT:
            report_key = KEY_GESTURE_RIGHT;
            break ;
        case GESTURE_DOWN:
             report_key = KEY_GESTURE_DOWN;
            break ;
        case GESTURE_UP: 
             report_key = KEY_GESTURE_UP;
             break ;
        case GESTURE_LEFT:   
            report_key = KEY_GESTURE_LEFT;
            break ;
        case GESTURE_DOUBLECLICK:   
            report_key = KEY_GESTURE_DOUBLECLICK;
            break ;
	default:
		break ;
    }
     HYN_INFO("check_gesture ======================report_key==0x%x   ================\n ",report_key);

	hyn_gesture_data.report_key=report_key;
    /* report event key */  
    if (report_key != -1){
        input_report_key(input_dev, report_key, 1);
        input_sync(input_dev);
        input_report_key(input_dev, report_key, 0);
        input_sync(input_dev);
    }
	 
	HYN_FUNC_EXIT();

   
}

/************************************************************************
*   Name: hyn_gesture_readdata
* Brief: read data from TP register
* Input: no
* Output: no
* Return: fail <0
***********************************************************************/
int hyn_gesture_readdata(void)
{
    u8 buf[HYN_GESTRUE_POINTS * 4] = { 0 };
    int ret = -1;
    int i = 0;
    int gestrue_id = 0;
    //int read_bytes = 0;
    u8 pointnum = 0;

    HYN_FUNC_ENTER();
	if (hyn_gesture_data.mode)
	{
		u8 state;	
		if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){

			u8 buf[3];
			buf[0]=HYN_REG_MUT_GESTURE_EN>>8;
			buf[1]=HYN_REG_MUT_GESTURE_EN&0xff;	
			ret=cst3xx_i2c_read_register(hyn_ts_data->client, buf, 1);
			state=buf[0]>>7;
			gestrue_id = buf[0]&0x7f;
		}else{
			ret = hyn_i2c_read_byte(hyn_ts_data->client, HYN_REG_CAP_GESTURE_EN, &state);
		}
		
	    if (ret<0)  	 HYN_ERROR("[HYN][Touch] read value fail");		
	    if (state !=1)   return -1;
   
    }
    if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){
    /* init variable before read gesture point */
	    memset(hyn_gesture_data.header, 0, HYN_GESTRUE_POINTS_HEADER);
	    memset(hyn_gesture_data.coordinate_x, 0, HYN_GESTRUE_POINTS * sizeof(u16));
	    memset(hyn_gesture_data.coordinate_y, 0, HYN_GESTRUE_POINTS * sizeof(u16));

	    buf[0] = HYN_REG_CAP_GESTURE_OUTPUT_ADDRESS;
	    ret = hyn_i2c_read(hyn_ts_data->client, buf, 1, buf, HYN_GESTRUE_POINTS_HEADER);
	    if (ret < 0){
	        HYN_ERROR("[GESTURE]Read gesture header data failed!!");
	        return ret;
	    }
		
	    gestrue_id = buf[0];  
	    for (i = 0; i < pointnum; i++){
	        hyn_gesture_data.coordinate_x[i] =  (((s16) buf[0 + (4 * i+8)]) & 0x0F) << 8
	                                            | (((s16) buf[1 + (4 * i+8)])& 0xFF);
	        hyn_gesture_data.coordinate_y[i] = (((s16) buf[2 + (4 * i+8)]) & 0x0F) << 8
	                                           | (((s16) buf[3 + (4 * i+8)]) & 0xFF);
    	}
    }
	hyn_gesture_data.gestrue_id=gestrue_id;
	hyn_check_gesture(hyn_ts_data->input_dev, gestrue_id);
    HYN_DEBUG("[GESTURE]gestrue_id:%d,report_key:%d", hyn_gesture_data.gestrue_id,hyn_gesture_data.report_key);
    HYN_FUNC_EXIT();
    return -1;
}
/*****************************************************************************
*  Name: hyn_irq_set_low_trig
*  Brief: 
*  Input:
*  Output:
*  Return:
*****************************************************************************/
void hyn_irq_gesture_mode(void)
{
	int ret=0;
	hyn_irq_disable();
	ret = enable_irq_wake(hyn_ts_data->client->irq);
    if(ret<0){
    	HYN_ERROR("[GESTURE]enable_irq_wake fail:%d",ret);
	}
	ret=irq_set_irq_type(hyn_ts_data->client->irq,IRQF_TRIGGER_LOW|IRQF_NO_SUSPEND);           
    if(ret<0){
    	HYN_ERROR("[GESTURE]irq_set_irq_type fail:%d",ret);
	}
	hyn_irq_enable();
 
}
void hyn_irq_normal_mode(void)
{
	int ret=0;
	hyn_irq_disable();
	ret =disable_irq_wake(hyn_ts_data->client->irq);
    if(ret<0){
    	HYN_ERROR("[GESTURE]disable_irq_wake fail:%d",ret);
	}
	ret=irq_set_irq_type(hyn_ts_data->client->irq,hyn_ts_data->pdata->irq_gpio_flags);         
    if(ret<0){
    	HYN_ERROR("[GESTURE]irq_set_irq_type fail:%d",ret);
	}
	hyn_irq_enable();
 
}
/*****************************************************************************
*   Name: hyn_gesture_suspend
*  Brief:
*  Input:
* Output: None
* Return: None
*****************************************************************************/
int hyn_gesture_suspend(void)
{
    u8 state = 0, i;

    HYN_FUNC_ENTER();
    //enter gesture scan 
    if (hyn_gesture_data.mode==0)
	return -1;

	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){

		u8 buf[3];
		buf[0]=HYN_REG_MUT_GESTURE_EN>>8;
		buf[1]=HYN_REG_MUT_GESTURE_EN&0xff;
		buf[2]=0x80;
		cst3xx_i2c_write(hyn_ts_data->client, buf, 3);
		msleep(10);
		for(i = 0; i < 3; i++)
		{
			buf[0]=HYN_REG_MUT_GESTURE_EN>>8;
			buf[1]=HYN_REG_MUT_GESTURE_EN&0xff;
		  	cst3xx_i2c_read_register(hyn_ts_data->client, buf, 1);
			state=buf[0]>>7;
			if(state == 1){
				HYN_DEBUG("gesture state= 0x01.\n");
	    	    break;
			}else{
				buf[0]=HYN_REG_MUT_GESTURE_EN>>8;
				buf[1]=HYN_REG_MUT_GESTURE_EN&0xff;
				buf[2]=0x80;
				cst3xx_i2c_write(hyn_ts_data->client, buf, 3);
				msleep(10);
			}
		}   


	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){

		hyn_i2c_write_byte(hyn_ts_data->client, HYN_REG_CAP_GESTURE_EN, ENABLE);
		msleep(10);
		for(i = 0; i < 3; i++)
		{
		  	hyn_i2c_read_byte(hyn_ts_data->client, HYN_REG_CAP_GESTURE_EN, &state);
			if(state == 1){
				HYN_DEBUG("gesture state= 0x01.\n");
	    	    break;
			}else{
				hyn_i2c_write_byte(hyn_ts_data->client, HYN_REG_CAP_GESTURE_EN, ENABLE);
				msleep(10);
			}
		}   
	}
    hyn_gesture_data.active = 1;
	hyn_irq_gesture_mode();
	
    HYN_DEBUG("[GESTURE]Enter into gesture(suspend) successfully!");
    HYN_FUNC_EXIT();
    return 0;
}

/*****************************************************************************
*   Name: hyn_gesture_resume
*  Brief:
*  Input:
* Output: None
* Return: None
*****************************************************************************/
int hyn_gesture_resume(void)
{

    HYN_FUNC_ENTER();
	
	if (hyn_gesture_data.mode == 0){
		HYN_DEBUG("gesture is disabled");
		HYN_FUNC_EXIT();
		return -1;
	}

    hyn_gesture_data.active = 0;
	hyn_irq_normal_mode();
	//stop gesture scan
	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){
		u8 buf[3];
		buf[0]=HYN_REG_MUT_GESTURE_EN>>8;
		buf[1]=HYN_REG_MUT_GESTURE_EN&0xff;
		buf[2]=0;
		cst3xx_i2c_write(hyn_ts_data->client, buf, 3);

	}else{
    	hyn_i2c_write_byte(hyn_ts_data->client, HYN_REG_CAP_GESTURE_EN, DISABLE);
	}
    HYN_FUNC_EXIT();
    return 0;
}


/*****************************************************************************
*   Name: hyn_gesture_init
*  Brief:
*  Input:
* Output: None
* Return: None
*****************************************************************************/
int hyn_gesture_init(struct input_dev *input_dev, struct i2c_client *client)
{
    HYN_FUNC_ENTER();
    input_set_capability(input_dev, EV_KEY, KEY_POWER);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_U);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_UP);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_DOWN);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_LEFT);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_RIGHT);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_O);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_E);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_M);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_L);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_W);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_S);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_V);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_Z);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_C);

    __set_bit(KEY_GESTURE_RIGHT, input_dev->keybit);
    __set_bit(KEY_GESTURE_LEFT,  input_dev->keybit);
    __set_bit(KEY_GESTURE_UP,    input_dev->keybit);
    __set_bit(KEY_GESTURE_DOWN,  input_dev->keybit);
    __set_bit(KEY_GESTURE_U, input_dev->keybit);
    __set_bit(KEY_GESTURE_O, input_dev->keybit);
    __set_bit(KEY_GESTURE_E, input_dev->keybit);
    __set_bit(KEY_GESTURE_M, input_dev->keybit);
    __set_bit(KEY_GESTURE_W, input_dev->keybit);
    __set_bit(KEY_GESTURE_L, input_dev->keybit);
    __set_bit(KEY_GESTURE_S, input_dev->keybit);
    __set_bit(KEY_GESTURE_V, input_dev->keybit);
    __set_bit(KEY_GESTURE_C, input_dev->keybit);
    __set_bit(KEY_GESTURE_Z, input_dev->keybit);

    hyn_create_gesture_sysfs(client);
    hyn_gesture_data.mode = 1;
    hyn_gesture_data.active = 0;
	hyn_gesture_data.report_key = 0;
	hyn_gesture_data.gestrue_id = 0;
    HYN_FUNC_EXIT();
    return 0;
}

/************************************************************************
* Name: hyn_gesture_exit
* Brief:  remove sys
* Input: i2c info
* Output: no
* Return: no
***********************************************************************/
int hyn_gesture_exit(void)
{
	HYN_FUNC_ENTER();
	if(hyn_ts_data->k_obj==NULL)  return -1;
	sysfs_remove_group(hyn_ts_data->k_obj, &hyn_gesture_group);
	HYN_FUNC_EXIT();
    return 0;
}

#endif



