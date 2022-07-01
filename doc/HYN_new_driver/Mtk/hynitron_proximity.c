#ifndef __HYNITRON_EX_FUN_H__
#define __HYNITRON_EX_FUN_H__


#include "hynitron_core.h"





#define MTK_PS_ENABLE      0
#define SPRD_PS_ENABLE     0

#define HYN_CAP_PROXIMITY_FARAWAY_VALUE   0xC0
#define HYN_CAP_PROXIMITY_NEAR_VALUE      0xE0

#define HYN_MUT_PROXIMITY_FARAWAY_VALUE   0x00
#define HYN_MUT_PROXIMITY_NEAR_VALUE      0x01

#if HYN_PSENSOR_EN


#if MTK_PS_ENABLE
#include <hwmsensor.h>
#include <hwmsen_dev.h>
#include <sensors_io.h>
#endif


#define TP_PS_DEVICE 		"ltr_558als"
#define TP_PS_INPUT_DEV	    "alps_pxy"
#define PROXIMITY_DEBUG(fmt,arg...) printk("<<hyn proximity>> "fmt"\n",##arg)

struct hynitron_ps_struct {
	u8 proximity_enable ;
	u8 proximity_enable_lock;
	u8 proximity_detect_value ;
	u8 suspend_flag;
	/*  kernel-4.14 do not wake_lock */
	//struct wake_lock proximity_lock;
#if MTK_PS_ENABLE
	struct hwmsen_object obj_ps;
#endif
	struct class *firmware_class;
	struct device *firmware_cmd_dev;
	struct input_dev *ps_input_dev;

};
struct hynitron_ps_struct  *hyn_proximity_data;
static int hyn_proximity_get_value(void);

/************************************************
MTK平台:
触控IC----绑定hwmsen_attach(ID_PROXIMITY)----上报hwm_sensor_data

*************************************************/

#if MTK_PS_ENABLE

#define SENSOR_DELAY      				 0
#define SENSOR_ENABLE     				 0
#define SENSOR_GET_DATA   				 0
#define SENSOR_STATUS_ACCURACY_MEDIUM    0

void hyn_proximity_mtk_report(int proximity_value)
{
	int err;
	struct hwm_sensor_data sensor_data;
	sensor_data.values[0] = proximity_value; 
	sensor_data.value_divide = 1;
	sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
	if ((err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
	{
		PROXIMITY_DEBUG(" proxi call hwmsen_get_interrupt_data failed= %d\n", err);	
	}

}
static int hyn_proximiy_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;

	PROXIMITY_DEBUG("[proxi]command = 0x%02X\n", command);		
	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int))){
				PROXIMITY_DEBUG("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int))){
				PROXIMITY_DEBUG("Enable sensor parameter error!\n");
				err = -EINVAL;
			}else{				
				value = *(int *)buff_in;
				if(value){		
					if((hyn_proximity_enable(1) != 0)){
						PROXIMITY_DEBUG("enable ps fail: %d\n", err); 
						return -1;
					}
				}else{
					if((hyn_proximity_enable(0) != 0)){
						PROXIMITY_DEBUG("disable ps fail: %d\n", err); 
						return -1;
					}
				}
			}
			break;

		case SENSOR_GET_DATA:
			if((buff_out == NULL)){
				PROXIMITY_DEBUG("get sensor data parameter error!\n");
				err = -EINVAL;
			}else{									
				hyn_proximity_readdata();					
			}
			break;
		default:
			PROXIMITY_DEBUG("proxmy sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;	
}
void hyn_proximity_mtk_init(void)
{

    int ret;
	hyn_ts_data->client->obj_ps.polling = 0;//interrupt mode
	hyn_ts_data->client->obj_ps.sensor_operate = hyn_proximiy_operate;
	if((ret = hwmsen_attach(ID_PROXIMITY, &hyn_ts_data->client->obj_ps))){
		PROXIMITY_DEBUG("proxi_hyn attach fail = %d\n", ret);
	}else{
		PROXIMITY_DEBUG("proxi_hyn attach ok = %d\n", ret);
	}
}

#endif



/************************************************
Spreadtrum平台:
触控IC----创建节点proximity-----上报输入设备接近或者远离

*************************************************/


#if SPRD_PS_ENABLE  


static ssize_t show_proximity_sensor(struct device *dev, struct device_attribute *attr, char *buf)
{
	PROXIMITY_DEBUG("show_proximity_sensor get prox state \n");
	if (buf != NULL)
		sprintf(buf, "proximity_enable is:%d, proximity_detect_value:%d.\n",hyn_proximity_data->proximity_enable ,hyn_proximity_data->proximity_detect_value);
    return 0;
}

static ssize_t store_proximity_sensor(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{

	unsigned int on_off = simple_strtoul(buf, NULL, 10);

    PROXIMITY_DEBUG("store_proximity_sensor buf=%d,size=%d,on_off=%d\n", *buf, size, on_off);
	if(buf != NULL && size != 0){
		if (0 == on_off)
			hyn_proximity_enable(0);
		else if (1 == on_off)
			hyn_proximity_enable(1);
	}

    return size;
}

static DEVICE_ATTR(proximity, S_IRUGO | S_IWUSR, show_proximity_sensor, store_proximity_sensor);

void hyn_proximity_sprd_report(int proximity_value)
{
	input_report_abs(hyn_proximity_data->ps_input_dev, ABS_DISTANCE, proximity_value);
	input_sync(hyn_proximity_data->ps_input_dev);

}
void hyn_proximity_sprd_init(void)
{
// node :/sys/class/sprd-tpd/device/proximity
	hyn_ts_data->client->firmware_class = class_create(THIS_MODULE,"sprd-tpd");//client->name  
	if(IS_ERR(hyn_ts_data->client->firmware_class))
	PROXIMITY_DEBUG("Failed to create class(firmware)!\n");
	hyn_ts_data->client->firmware_cmd_dev = device_create(hyn_ts_data->client->firmware_class, NULL, 0, NULL, "device");//device

	if(IS_ERR(hyn_ts_data->client->firmware_cmd_dev))
	PROXIMITY_DEBUG("Failed to create device(firmware_cmd_dev)!\n");

	if(device_create_file(hyn_ts_data->client->firmware_cmd_dev, &dev_attr_proximity) < 0){
		PROXIMITY_DEBUG("Failed to create device file(%s)!\n", dev_attr_proximity.attr.name);
	}

}
#endif


/************************************************************************
* Name: hyn_proximity_mode_show
* Brief:  no
* Input: device, device attribute, char buf
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_proximity_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    u8 val = 0;
    //struct i2c_client *client = container_of(dev, struct i2c_client, dev);
	HYN_FUNC_ENTER();
    mutex_lock(&hyn_ts_data->input_dev->mutex);
    count = sprintf(buf, "proximity Mode: %s\n", hyn_proximity_data->proximity_enable ? "On" : "Off");
    count += sprintf(buf + count, "Reg(0xD0) = %d\n", val);
    mutex_unlock(&hyn_ts_data->input_dev->mutex);
	HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_proximity_mode_store
* Brief:  no
* Input: device, device attribute, char buf, char count
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_proximity_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	HYN_FUNC_ENTER();
	mutex_lock(&hyn_ts_data->input_dev->mutex);

    if (buf[0]==1)
    {
        HYN_INFO("[proximity]enable ");
        hyn_proximity_data->proximity_enable = ENABLE;
    }
    else if (buf[0]==0)
    {
        HYN_INFO("[proximity]disable");
        hyn_proximity_data->proximity_enable = DISABLE;
    }

    mutex_unlock(&hyn_ts_data->input_dev->mutex);

	HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_proximity_buf_show
* Brief:  no
* Input: device, device attribute, char buf
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_proximity_buf_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
   
	HYN_FUNC_ENTER();
	
    mutex_lock(&hyn_ts_data->input_dev->mutex);
    count = snprintf(buf, PAGE_SIZE, "proximity: 0x%x\n", hyn_proximity_data->proximity_enable);
    count += snprintf(buf + count, PAGE_SIZE, "proximity: %d\n", hyn_proximity_data->proximity_detect_value);
    count += snprintf(buf + count, PAGE_SIZE, "proximity Buf:\n");
    count += snprintf(buf + count, PAGE_SIZE, "\n");
    mutex_unlock(&hyn_ts_data->input_dev->mutex);

	HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_proximity_buf_store
* Brief:  no
* Input: device, device attribute, char buf, char count
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_proximity_buf_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    /* place holder for future use */
    return -EPERM;
}



/* sysfs proximity node
 *   read example: cat  hyn_proximity_mode        ---read proximity mode
 *   write example:echo 01 > hyn_proximity_mode   ---write proximity mode to 01
 *
 */
static DEVICE_ATTR (hyn_proximity_mode, S_IRUGO|S_IWUSR, hyn_proximity_mode_show, hyn_proximity_mode_store);
/*
 *   read example: cat  hyn_proximity_buf        ---read proximity buf
 */
static DEVICE_ATTR (hyn_proximity_buf, S_IRUGO|S_IWUSR, hyn_proximity_buf_show, hyn_proximity_buf_store);
static struct attribute *hyn_proximity_mode_attrs[] =
{

    &dev_attr_hyn_proximity_mode.attr,
    &dev_attr_hyn_proximity_buf.attr,
    NULL,
};

static struct attribute_group hyn_proximity_group =
{
    .attrs = hyn_proximity_mode_attrs,
};

int hyn_create_proximity_sysfs(struct i2c_client *client)
{
    int ret = 0;
	HYN_FUNC_ENTER();

	if(hyn_ts_data->k_obj==NULL)
		return 0;

    ret = sysfs_create_group(hyn_ts_data->k_obj, &hyn_proximity_group);
    if ( ret != 0)
    {
        HYN_ERROR( "hyn_create_proximity_sysfs(sysfs) create failed!");
        sysfs_remove_group(hyn_ts_data->k_obj, &hyn_proximity_group);
        return ret;
    }
	HYN_FUNC_EXIT();
    return 0;
}

int hyn_proximity_readdata(void)
{
	int err;
	HYN_FUNC_ENTER();
	if (hyn_proximity_data->proximity_enable == 1)
	{		
		err=hyn_proximity_get_value();
#if MTK_PS_ENABLE
		hyn_proximity_mtk_report(hyn_proximity_data->proximity_detect_value);
#endif
#if SPRD_PS_ENABLE
		hyn_proximity_sprd_report(hyn_proximity_data->proximity_detect_value);
#endif
	}
	HYN_FUNC_EXIT();
	return err; 
}

static int hyn_proximity_enable_real(int enable)
{
	u8 state;
	int ret = -1;
	HYN_FUNC_ENTER();
	
	if (enable){
		state = ENABLE;
		hyn_proximity_data->proximity_enable = ENABLE;
		/*  kernel-4.14 do not wake_lock */
		//wake_lock(&hyn_proximity_data->proximity_lock);
		PROXIMITY_DEBUG(" proximity function is on.\n");	
	}else{
		if(hyn_proximity_data->suspend_flag) return 0;
		state = DISABLE;	
		/*  kernel-4.14 do not wake_lock */
		//wake_unlock(&hyn_proximity_data->proximity_lock);
		hyn_proximity_data->proximity_enable = DISABLE;
		PROXIMITY_DEBUG(" proximity function is off.\n");
	}

	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){

		u8 buf[3];
		buf[0]=HYN_REG_MUT_PROXIMITY_EN>>8;
		buf[1]=HYN_REG_MUT_PROXIMITY_EN&0xff;
		buf[2]=state<<7;
	  	ret=cst3xx_i2c_write(hyn_ts_data->client, buf, 3);
		
	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){
		ret = hyn_i2c_write_byte(hyn_ts_data->client, HYN_REG_CAP_PROXIMITY_EN, state);
	}
	PROXIMITY_DEBUG("proximity write state is 0x%02X\n", state);
	HYN_FUNC_EXIT();
	return 0;
}


static int hyn_proximity_get_value(void)
{
	u8 proximity_status=0;	
	HYN_FUNC_ENTER();
	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){

		u8 buf[3];
		buf[0]=HYN_REG_MUT_PROXIMITY_EN>>8;
		buf[1]=HYN_REG_MUT_PROXIMITY_EN&0xff;
	  	cst3xx_i2c_read_register(hyn_ts_data->client, buf, 1);
		proximity_status=buf[0]&0x7f;
		PROXIMITY_DEBUG("proximity value is 0x%02X\n", proximity_status);
		if (proximity_status == HYN_MUT_PROXIMITY_FARAWAY_VALUE){
			hyn_proximity_data->proximity_detect_value = 0;	
		}else if(proximity_status == HYN_MUT_PROXIMITY_NEAR_VALUE){
			hyn_proximity_data->proximity_detect_value = 1;
		}
	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){
	
		hyn_i2c_read_byte(hyn_ts_data->client, HYN_REG_CAP_PROXIMITY_OUTPUT_ADDRESS, &proximity_status);
		PROXIMITY_DEBUG("proximity value is 0x%02X\n", proximity_status);
	
		if (proximity_status == HYN_CAP_PROXIMITY_FARAWAY_VALUE){
			hyn_proximity_data->proximity_detect_value = 0;	
		}else if(proximity_status == HYN_CAP_PROXIMITY_NEAR_VALUE){
			hyn_proximity_data->proximity_detect_value = 1;
		}
	}
	PROXIMITY_DEBUG("proximity_detect_value = %d\n", hyn_proximity_data->proximity_detect_value);

	HYN_FUNC_EXIT();
	return hyn_proximity_data->proximity_detect_value;
}

static int hyn_proximity_enable(int enable)
{
	HYN_FUNC_ENTER();
	PROXIMITY_DEBUG("proximity suspend_flag=0x%02X, enable= 0x%02X.\n",hyn_proximity_data->suspend_flag,enable );		
	if (hyn_proximity_data->suspend_flag && enable) {
		
	}else{
		hyn_proximity_enable_real(enable);
	}
	HYN_FUNC_EXIT();
	return 0;
}
void hyn_proximity_suspend(void)
{
	HYN_FUNC_ENTER();
	hyn_proximity_data->suspend_flag++;
	if (hyn_proximity_data->proximity_enable == 1)    hyn_proximity_data->proximity_enable_lock = 1;	 
	PROXIMITY_DEBUG("proximity suspend_flag=0x%02X, enable= 0x%02X,enable_lock = 0x%02X.\n",
	hyn_proximity_data->suspend_flag,hyn_proximity_data->proximity_enable,hyn_proximity_data->proximity_enable_lock );		
	HYN_FUNC_EXIT();
}
void hyn_proximity_resume(void)
{
	HYN_FUNC_ENTER();
	hyn_proximity_data->suspend_flag = 0;
	if (hyn_proximity_data->proximity_enable == 1){
		if(hyn_proximity_data->proximity_enable_lock == 1){
			hyn_proximity_data->proximity_enable_lock = 0;	
			PROXIMITY_DEBUG(" proximity_enable_lock. \n"); 	
		}
	}
	PROXIMITY_DEBUG("proximity suspend_flag=0x%02X, enable= 0x%02X,enable_lock = 0x%02X.\n",
	hyn_proximity_data->suspend_flag,hyn_proximity_data->proximity_enable,hyn_proximity_data->proximity_enable_lock );		

	HYN_FUNC_EXIT();

}

void hyn_proximity_init(void)
{
	int ret;
	HYN_FUNC_ENTER();
	/* malloc memory for global struct variable,init 0 */
	hyn_proximity_data = kzalloc(sizeof(*hyn_proximity_data), GFP_KERNEL);
	if (!hyn_proximity_data) {
		PROXIMITY_DEBUG("proximity allocate memory for hyn_proximity_data fail");
		return;
	}
	
	hyn_proximity_data->ps_input_dev = input_allocate_device();
	if (!hyn_proximity_data->ps_input_dev) 
	{
		PROXIMITY_DEBUG(" %s: input allocate device failed\n", __func__);
		ret = -ENOMEM;
		goto exit_input_dev_allocate_failed;
	}
    hyn_proximity_data->ps_input_dev->name = TP_PS_INPUT_DEV;
	hyn_proximity_data->ps_input_dev->phys  = TP_PS_INPUT_DEV;
	hyn_proximity_data->ps_input_dev->id.bustype = BUS_I2C;
	hyn_proximity_data->ps_input_dev->dev.parent = &hyn_ts_data->client->dev;

	__set_bit(EV_ABS, hyn_proximity_data->ps_input_dev->evbit);	
	input_set_abs_params(hyn_proximity_data->ps_input_dev, ABS_DISTANCE, 0, 1, 0, 0);

	ret= input_register_device(hyn_proximity_data->ps_input_dev);
	if (ret < 0)
	{
	    PROXIMITY_DEBUG("%s: input device regist failed\n", __func__);
	    goto exit_input_register_failed;
	}
   	hyn_create_proximity_sysfs(hyn_ts_data->client);

	hyn_proximity_enable(0);

        /* kernel-4.14 do not wake_lock */
	//wake_lock_init(&hyn_proximity_data->proximity_lock, WAKE_LOCK_SUSPEND, "proximity wakelock");


#if MTK_PS_ENABLE
	hyn_proximity_mtk_init();
#endif

#if SPRD_PS_ENABLE
	hyn_proximity_sprd_init();
#endif
	HYN_FUNC_EXIT();
	return;
	
exit_input_dev_allocate_failed:
	input_free_device(hyn_proximity_data->ps_input_dev );
exit_input_register_failed:
	input_free_device(hyn_proximity_data->ps_input_dev);
	
}
void hyn_proximity_exit(void)
{   
	HYN_FUNC_ENTER();
	if(hyn_ts_data->k_obj==NULL) return;
	sysfs_remove_group(hyn_ts_data->k_obj, &hyn_proximity_group);
	if(hyn_proximity_data==NULL) return;
	kfree(hyn_proximity_data);
	//wake_lock_destroy(&hyn_proximity_data->proximity_lock);  /*  kernel-4.14 do not wake_lock */

	HYN_FUNC_EXIT();
}
#endif

#endif

