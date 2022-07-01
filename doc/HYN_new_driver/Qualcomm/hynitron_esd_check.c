/**
 *Name        : hynitron_esd_check.c
 *Author      : steven
 *Version     : V2.0
 *Create      : 2018-1-23
 *Copyright   : zxzz
 */


#include "hynitron_core.h"

/*****************************************************************************/
#if  HYN_ESDCHECK_EN

#define SWITCH_ESD_OFF                  0
#define SWITCH_ESD_ON                   1


extern int hyn_reset_proc(int hdelayms);

/************************************************************************
* Name: hyn_esd_show
* Brief:  no
* Input: device, device attribute, char buf
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_esd_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
   
    //struct i2c_client *client = container_of(dev, struct i2c_client, dev);
    HYN_FUNC_ENTER();
    mutex_lock(&hyn_ts_data->hyn_esd_lock);
    count = sprintf(buf,"ESD Mode: %d,ESD running = %d\n", hyn_ts_data->hyn_esd_mode,hyn_ts_data->hyn_esd_running);
    mutex_unlock(&hyn_ts_data->hyn_esd_lock);
    HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_esd_store
* Brief:  no
* Input: device, device attribute, char buf, char count
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_esd_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	HYN_FUNC_ENTER();
	mutex_lock(&hyn_ts_data->hyn_esd_lock);

    if (buf[0]==1)
    {
        HYN_INFO("[ESD]enable ESD");
        hyn_ts_data->hyn_esd_mode = ENABLE;
    }
    else if (buf[0]==0)
    {
        HYN_INFO("[ESD]disable ESD");
        hyn_ts_data->hyn_esd_mode  = DISABLE;
    }

    mutex_unlock(&hyn_ts_data->hyn_esd_lock);

	HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_esd_buf_show
* Brief:  no
* Input: device, device attribute, char buf
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_esd_buf_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    HYN_FUNC_ENTER();
	
    mutex_lock(&hyn_ts_data->hyn_esd_lock);
    count = snprintf(buf, PAGE_SIZE, "ESD running: 0x%x\n", hyn_ts_data->hyn_esd_running);
    count += snprintf(buf + count, PAGE_SIZE, "ESD pre_counter:0x%x\n", hyn_ts_data->hyn_esd_pre_counter);
    count += snprintf(buf + count, PAGE_SIZE, "ESD cur_counter:0x%x\n", hyn_ts_data->hyn_esd_cur_counter);
    count += snprintf(buf + count, PAGE_SIZE, "ESD work_cycle:0x%x\n", hyn_ts_data->hyn_esd_work_cycle);
    mutex_unlock(&hyn_ts_data->hyn_esd_lock);

	HYN_FUNC_EXIT();
    return count;
}

/************************************************************************
* Name: hyn_esd_buf_store
* Brief:  no
* Input: device, device attribute, char buf, char count
* Output: no
* Return:
***********************************************************************/
static ssize_t hyn_esd_buf_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    /* place holder for future use */
    return -EPERM;
}

static DEVICE_ATTR (hyn_esd_mode, S_IRUGO|S_IWUSR, hyn_esd_show, hyn_esd_store);
/*
 *   read example: cat  hyn_esd_buf        ---read esd buf
 */
static DEVICE_ATTR (hyn_esd_buf, S_IRUGO|S_IWUSR, hyn_esd_buf_show, hyn_esd_buf_store);
static struct attribute *hyn_esd_mode_attrs[] =
{

    &dev_attr_hyn_esd_mode.attr,
    &dev_attr_hyn_esd_buf.attr,
    NULL,
};

static struct attribute_group hyn_esd_group =
{
    .attrs = hyn_esd_mode_attrs,
};
void hyn_create_esd_sysfs(struct i2c_client *client)
{
    int ret = 0;
	HYN_FUNC_ENTER();
	
	if(hyn_ts_data->k_obj==NULL)
		return;
	
    ret = sysfs_create_group(hyn_ts_data->k_obj, &hyn_esd_group);
    if ( ret != 0)
    {
        HYN_ERROR( "[GESTURE]hyn_gesture_mode_group(sysfs) create failed!");
        sysfs_remove_group(hyn_ts_data->k_obj, &hyn_esd_group);
        return;
    }
	HYN_FUNC_EXIT();
    return;
}

static void hyn_esd_poweron_ic(unsigned int ms)
{
	HYN_FUNC_ENTER();
	hyn_reset_proc(10);
	HYN_FUNC_EXIT();
	   
}
 void hyn_esd_suspend(void)
{
	HYN_FUNC_ENTER();
	if(hyn_ts_data->hyn_esd_mode)
	hyn_esd_switch(SWITCH_ESD_OFF);
	HYN_FUNC_EXIT();
	   
}
 void hyn_esd_resume(void)
{
	HYN_FUNC_ENTER();
	if(hyn_ts_data->hyn_esd_mode)
	hyn_esd_switch(SWITCH_ESD_ON);
	HYN_FUNC_EXIT();
	   
}

void hyn_esd_switch(s32 on)
{
	HYN_FUNC_ENTER();
	if(!hyn_ts_data->hyn_esd_mode)
	return;

	mutex_lock(&hyn_ts_data->hyn_esd_lock);
	if (SWITCH_ESD_ON == on) {	/* switch on esd check */
		if (!hyn_ts_data->hyn_esd_running) {
			hyn_ts_data->hyn_esd_running = 1;
			hyn_ts_data->hyn_esd_pre_counter = 0;
			hyn_ts_data->hyn_esd_cur_counter = 0;
			HYN_DEBUG(" Esd protector started!");
			queue_delayed_work(hyn_ts_data->hyn_esd_workqueue, &hyn_ts_data->hyn_esd_check_work, hyn_ts_data->hyn_esd_work_cycle);
		}
	} else {		/* switch off esd check */
		if (hyn_ts_data->hyn_esd_running) {
			hyn_ts_data->hyn_esd_running = 0;
			hyn_ts_data->hyn_esd_pre_counter = 0;
			hyn_ts_data->hyn_esd_cur_counter = 0;
			HYN_DEBUG("  Esd protector stopped!");
			cancel_delayed_work(&hyn_ts_data->hyn_esd_check_work);
		}
	}
	mutex_unlock(&hyn_ts_data->hyn_esd_lock);
	HYN_FUNC_EXIT();

}

static void hyn_esd_check_func(struct work_struct *work)
{
    int retry = 0;
	int ret;
	int check_sum;
	unsigned char buf[8];

	HYN_FUNC_ENTER();

	if(!hyn_ts_data->hyn_esd_mode){
		HYN_DEBUG("Esd protector hyn_esd_mode is disable !");
		return;
	}
	if(!hyn_ts_data->work_mode!=0){
		HYN_DEBUG("Esd protector work_mode is false !");
		return;
	}
	if (!hyn_ts_data->hyn_esd_running) {
		HYN_DEBUG("Esd protector hyn_esd_running is not running!");
		return;
	}

	if((hyn_ts_data->hyn_esd_i2c_lock_flag != 0)||(hyn_ts_data->apk_rdwr_flag!=0)||(hyn_ts_data->fw_updating!=0)) 
		goto END;
	else hyn_ts_data->hyn_esd_i2c_lock_flag = 1;

	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){

		while(retry++ < 5) {
			buf[0] = HYN_REG_MUT_ESD_VALUE>>8;
			buf[1] = HYN_REG_MUT_ESD_VALUE&0xff;
			ret = cst3xx_i2c_read_register(hyn_ts_data->client, buf, 6);
			if (ret > 0){		
				check_sum = buf[0]+buf[1]+buf[2]+buf[3]+0xA5;
				if(check_sum != ( (buf[4]<<8)+ buf[5])){
		            HYN_ERROR("hyn_esd_check_func iic read check_sum error.\n");
				}else{
					break;
				}		
			}
			mdelay(2);
		}

	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){

	}
	
    if((retry==6)||(retry==0)|| (ret<0)){
		HYN_ERROR("hyn_esd_check_func iic read esd data error,please check firmware,driver and hardware.\n");
		goto END;
    }
	hyn_ts_data->hyn_esd_cur_counter = buf[3]+(buf[2]<<8)+(buf[1]<<16)+(buf[0]<<24);
	if(((hyn_ts_data->hyn_esd_cur_counter-hyn_ts_data->hyn_esd_pre_counter)<20)
	&&(hyn_ts_data->hyn_esd_pre_counter>400))
	{
           HYN_ERROR("hyn_esd_check_func hyn_esd_cur_counter is :0x%x. hyn_esd_pre_counter is:0x%x.------\n",hyn_ts_data->hyn_esd_cur_counter,hyn_ts_data->hyn_esd_pre_counter);
           goto hyn_esd_check_init;
	}
	goto END;

hyn_esd_check_init:
	HYN_ERROR("hyn_esd_check_func reset.\n");
	hyn_esd_poweron_ic(10);

END:
	hyn_ts_data->hyn_esd_i2c_lock_flag = 0;
	hyn_ts_data->hyn_esd_pre_counter=hyn_ts_data->hyn_esd_cur_counter;
	mutex_lock(&hyn_ts_data->hyn_esd_lock);
	if (hyn_ts_data->hyn_esd_running)
		queue_delayed_work(hyn_ts_data->hyn_esd_workqueue, &hyn_ts_data->hyn_esd_check_work, hyn_ts_data->hyn_esd_work_cycle);
	else
		HYN_DEBUG(" Esd protector suspended!");
	mutex_unlock(&hyn_ts_data->hyn_esd_lock);

	HYN_FUNC_EXIT();
}
int hyn_esd_get_status(void)
{
	u8 buf[7];
	int ret=-1;
	u16 check_sum=0;
	HYN_FUNC_ENTER();
	
	buf[0] = HYN_REG_MUT_ESD_VALUE>>8;
	buf[1] = HYN_REG_MUT_ESD_VALUE&0xff;
	ret = cst3xx_i2c_read_register(hyn_ts_data->client, buf, 6);
	check_sum = buf[0]+buf[1]+buf[2]+buf[3]+0xA5;
	if(check_sum != ( (buf[4]<<8)+ buf[5])){
        HYN_ERROR(" esd check sum error.\n ");
		return -1;
	}	
	
	HYN_FUNC_EXIT();
	return 1;
}
	
void hyn_esd_init(void)
{
	HYN_FUNC_ENTER();
	
	if(hyn_esd_get_status() < 0) {
		HYN_ERROR(" hyn_esd_get_status failed!");
		return;
	}
	hyn_ts_data->hyn_esd_workqueue = create_singlethread_workqueue("hyn_esd_workqueue");
	if (hyn_ts_data->hyn_esd_workqueue == NULL)
		HYN_ERROR(" hyn_esd_workqueue failed!");
	else{
		hyn_init_esd_protect();
		hyn_esd_switch(SWITCH_ESD_ON);
		hyn_ts_data->hyn_esd_running=ENABLE;
	}
	hyn_create_esd_sysfs(hyn_ts_data->client);
	HYN_FUNC_EXIT();
}
void hyn_init_esd_protect(void)
{
	HYN_FUNC_ENTER();
	hyn_ts_data->hyn_esd_work_cycle=1000;
	hyn_ts_data->hyn_esd_mode=ENABLE;  
	HYN_DEBUG(" Clock ticks for an esd cycle: %d", hyn_ts_data->hyn_esd_work_cycle);
	INIT_DELAYED_WORK(&hyn_ts_data->hyn_esd_check_work, hyn_esd_check_func);
	mutex_init(&hyn_ts_data->hyn_esd_lock);
	HYN_FUNC_EXIT();
}

void hyn_esd_exit(void)
{
	HYN_FUNC_ENTER();
    if (hyn_ts_data->hyn_esd_workqueue)
    destroy_workqueue(hyn_ts_data->hyn_esd_workqueue);
	HYN_FUNC_EXIT();
}

#endif


