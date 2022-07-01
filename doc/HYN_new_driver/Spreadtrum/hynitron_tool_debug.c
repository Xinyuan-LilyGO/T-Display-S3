/*
 *Name        : hynintron_ex_fun.c
 *Author      : steven
 *Version     : V2.0
 *Create      : 2019-11-11
 *Copyright   : zxzz
 */

#include <linux/netdevice.h>
#include <linux/mount.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/syscalls.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#include <linux/file.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

#include "hynitron_core.h"



#if HYN_AUTO_FACTORY_TEST_EN	

#define HYN_TP_OPEN_VALUE_MAX       2500
#define HYN_TP_OPEN_VALUE_MIN       1300
#define HYN_TP_OPEN_LOW_VALUE_MIN   200
#define HYN_TP_OPEN_DRV_VALUE_MIN   1200
#define HYN_TP_OPEN_DROP_RATIO      15

#define SWITCH_ESD_OFF                  0
#define SWITCH_ESD_ON                   1



#define HYN_TP_SHORT_RX_TX_RESISTANCE  600
char factory_test[256];
int hyn_factory_touch_test(void);
#endif

#if ANDROID_TOOL_SURPORT


 //proc/touchscreen/ctp_openshort_test
 
#define HYNITRON_PROC_TEST_DIR_NAME		    "touchscreen"
#define HYNITRON_PROC_TEST_FILE_NAME		"ctp_openshort_test"

#define HYNITRON_PROC_DIR_NAME		"cst1xx_ts"
#define HYNITRON_PROC_FILE_NAME		"cst1xx-update"

#define HYNITRON_PROC_DIR_NAME_2	"cst8xx_ts"
#define HYNITRON_PROC_FILE_NAME_2	"cst8xx-update"

extern int hyn_boot_update_fw(struct i2c_client * client);


DECLARE_WAIT_QUEUE_HEAD(debug_waiter);
static struct proc_dir_entry *g_proc_dir;
static struct proc_dir_entry *g_update_file;
static struct proc_dir_entry *g_proc_test_dir;
static struct proc_dir_entry *g_update_test_file;
static int CMDIndex = 0;

static struct file *hynitron_open_fw_file(char *path, mm_segment_t * old_fs_p)
{
	struct file * filp;
	int ret;
	
	*old_fs_p = get_fs();
	//set_fs(KERNEL_DS);
	filp = filp_open(path, O_RDONLY, 0);
	if (IS_ERR(filp)) 
	{   
        ret = PTR_ERR(filp);
		HYN_INFO("hynitron_open_fw_file:filp_open error.\n");
        return NULL;
    }
    filp->f_op->llseek(filp, 0, 0);
	
    return filp;
}

static void hynitron_close_fw_file(struct file * filp,mm_segment_t old_fs)
{
	//set_fs(old_fs);
	if(filp)
	    filp_close(filp,NULL);
}

static int hynitron_read_fw_file(unsigned char *filename, unsigned char *pdata, int *plen)
{
	struct file *fp;
	mm_segment_t old_fs;
	//int size;
	//int length;
	int ret = -1;
	loff_t pos;
	off_t fsize;
	struct inode *inode;
	unsigned long magic;
	
	HYN_INFO("hynitron_read_fw_file enter.\n");

	if((pdata == NULL) || (strlen(filename) == 0)) 
		return ret;
		
	fp = hynitron_open_fw_file(filename, &old_fs);
	if(fp == NULL) 
	{		
        HYN_INFO("Open bin file faild.path:%s.\n", filename);
		goto clean;
	}
 

	if (IS_ERR(fp)) {
		HYN_INFO("error occured while opening file %s.\n", filename);
		return -EIO;
	}
	inode = fp->f_inode;
	magic = inode->i_sb->s_magic;
	fsize = inode->i_size;		
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	pos = 0;
	ret=vfs_read(fp, pdata, fsize, &pos);
	if(ret==fsize){
		HYN_INFO("vfs_read success.ret:%d.\n",ret);
	}else{
		HYN_INFO("vfs_read fail.ret:%d.\n",ret);
	}
	filp_close(fp, NULL);
	set_fs(old_fs);
	
	HYN_INFO("vfs_read done.\n");


clean:
	hynitron_close_fw_file(fp, old_fs);
	return ret;
}
static int hynitron_apk_fw_dowmload(struct i2c_client *client,
		unsigned char *pdata, int length) 
{ 
	
	hyn_ts_data->apk_upgrade_flag=1;
	hyn_ts_data->p_hynitron_upgrade_firmware=(unsigned char *)pdata;
	HYN_INFO("hynitron_apk_fw_dowmload enter.\n");
	hyn_boot_update_fw(client);
	hyn_ts_data->apk_upgrade_flag=0;		
	return 0;
}
void hynitron_wake_up_wait(void)
{
	if(	hyn_ts_data->work_mode > 0){
		wake_up_interruptible(&debug_waiter);
		hyn_ts_data->hyn_irq_flag = 1;
		HYN_INFO("hynitron_wake_up_wait exit********hyn_ts_data->hyn_irq_flag:%d. \n",hyn_ts_data->hyn_irq_flag);
	}
}
void hynitron_wait_event_wait(void)
{
	HYN_INFO("hynitron_wait_event_wait exter11111*******. \n");
	wait_event_interruptible(debug_waiter,  hyn_ts_data->hyn_irq_flag!=0);
	HYN_INFO("hynitron_wake_up_wait exit22222********. \n");

}
static ssize_t hynitron_proc_test_read_foobar(struct file *page,char __user *user_buf, size_t count, loff_t *data)
{
	int ret=0;

	HYN_INFO("hynitron_proc_test_read_foobar********CMDIndex:%d. \n",CMDIndex);
#if HYN_AUTO_FACTORY_TEST_EN	
	ret = copy_to_user(user_buf,factory_test,256);
#endif
	return ret;
	
}
static ssize_t hynitron_proc_test_write_foobar(struct file *file, const char __user *buffer,size_t count, loff_t *data)
{
    unsigned char cmd[258];
	int len;
  
	if (count > 256) 
		len = 256;
	else 
		len = count;

	if (copy_from_user(cmd, buffer, len))  
	{
		HYN_INFO("copy data from user space failed.\n");
		return -EFAULT;
	}
#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_OFF);
#endif


	HYN_INFO("hynitron_proc_test_write_foobar*********cmd:%d*****%d******len:%d .\r\n", cmd[0], cmd[1], len);
	if (cmd[0] == 1) 
	{
#if HYN_AUTO_FACTORY_TEST_EN
	hyn_factory_touch_test();
#endif

	}

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_ON);
#endif


	return count;

}
static ssize_t hynitron_proc_read_foobar(struct file *page,char __user *user_buf, size_t count, loff_t *data)
{	
	unsigned char buf[1020];
	int len = 0;	
	int ret;
	struct i2c_client *client = (struct i2c_client *)PDE_DATA(file_inode(page));

	HYN_INFO("hynitron_proc_read_foobar********CMDIndex:%d. \n",CMDIndex);

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_OFF);
#endif
	
	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){

		if (CMDIndex == 0) {
	        
			sprintf(buf,"Hynitron MUTCAP touchscreen driver 1.0.\n");
			//strcpy(page,buf);	
			len = strlen(buf);
			ret = copy_to_user(user_buf,buf,len);
		}
		else if (CMDIndex == 1) {

			buf[0] = 0xD1;
			buf[1] = 0x01;
			ret = cst3xx_i2c_write(client, buf, 2);
			if (ret < 0) return -1;
			
			mdelay(10);

			buf[0] = 0xD1;
			buf[1] = 0xF4;
			ret = cst3xx_i2c_read_register(client, buf, 28);
			if (ret < 0) return -1;	

			hyn_ts_data->sensor_tx=buf[0];
			hyn_ts_data->sensor_rx=buf[2];

			HYN_INFO("  cst3xx_proc_read_foobar:g_cst3xx_tx:%d,g_cst3xx_rx:%d.\n",hyn_ts_data->sensor_tx,hyn_ts_data->sensor_rx);
			
			len = 28;
			ret = copy_to_user(user_buf,buf,len);

			buf[0] = 0xD1;
			buf[1] = 0x09;
			ret = cst3xx_i2c_write(client, buf, 2);
			
		}
		else if(CMDIndex == 2 || CMDIndex == 3||CMDIndex == 4) {		
			unsigned short rx,tx;
			int data_len;
			
			rx = hyn_ts_data->sensor_rx;
			tx = hyn_ts_data->sensor_tx;
			data_len = rx*tx*2 + 4 + (tx+rx)*2 + rx + rx;
			
			hyn_ts_data->hyn_irq_flag = 0;
			if(CMDIndex == 2)  //read diff
			{
				buf[0] = 0xD1;
				buf[1] = 0x0D;
				hyn_ts_data->work_mode = HYN_WORK_MODE_DIFF;

			}
			else  if(CMDIndex == 3)        //rawdata
			{  
				buf[0] = 0xD1;
				buf[1] = 0x0A;
				hyn_ts_data->work_mode = HYN_WORK_MODE_RAWDATA;

			}
			else if(CMDIndex == 4)          //factory test
			{  
				buf[0] = 0xD1;
				buf[1] = 0x19;
				data_len = rx*tx*4 +(4 + tx + rx)*2;
				hyn_ts_data->work_mode = HYN_WORK_MODE_FACTORY;

			}
					
			ret = i2c_master_send(client, buf, 2);  
			if(ret < 0) {			
				HYN_ERROR(" cst3xx Write command raw/diff mode failed.error:%d.\n", ret);
				goto END;
			}

			mdelay(5);			
			hynitron_wait_event_wait();
			hyn_ts_data->hyn_irq_flag = 0;
			
			HYN_INFO(" cst3xx Read wait_event interrupt");

			if(hyn_ts_data->config_chip_series==HYN_CHIP_CST3XX){
				
				buf[0] = 0x10;
				buf[1] = 0x00;
				data_len = rx*tx*2;
			}else{
				if(CMDIndex == 4){
					buf[0] = 0x12;
					buf[1] = 0x15;
				}else{
					buf[0] = 0x80;
					buf[1] = 0x01;
				}
			}						
			ret = cst3xx_i2c_write(client, buf, 2);
			if(ret < 0) {				
				HYN_ERROR(" cst3xx Write command(0x8001) failed.error:%d.\n", ret);
				goto END;
			}		
			ret = cst3xx_i2c_read(client, &buf[2], data_len);
			if(ret < 0) {
				HYN_ERROR(" cst3xx Read raw/diff data failed.error:%d.\n", ret);
				goto END;
			}

			if(hyn_ts_data->config_chip_series==HYN_CHIP_CST3XX){

				if(CMDIndex == 4){
					buf[data_len+2] = 0x30;
					buf[data_len+3] = 0x00;
					ret = cst3xx_i2c_write(client, &buf[data_len+2], 2);
					if (ret < 0) {
						HYN_ERROR("Write command failed.error:%d.\n", ret);
						goto END;
					}
					ret = cst3xx_i2c_read(client, &buf[data_len+2], data_len);
					if(ret < 0){
						HYN_ERROR("Read factory data failed.error:%d.\n", ret);
						goto END;
					}
					data_len+=data_len;	
				}
			}

			mdelay(1);
			
			buf[0] = 0xD1;
			buf[1] = 0x09;		
			ret = cst3xx_i2c_write(client, buf, 2); 		
			if(ret < 0) {				
				HYN_ERROR(" cst3xx Write command normal mode failed.error:%d.\n", ret);
				goto END;
			}	
			hyn_ts_data->work_mode = HYN_WORK_MODE_NORMAL;	
			buf[0] = rx;
			buf[1] = tx;	
			ret = copy_to_user(user_buf,buf,data_len + 2);
	    	len = data_len + 2;
			
			if(CMDIndex == 4)
			{
				hyn_reset_proc(10);
			}
		}


	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){

		if (CMDIndex == 0) {
			sprintf(buf,"Hynitron SELCAP touchscreen driver 1.0.\n");
			//strcpy(page,buf);	
			len = strlen(buf);
			ret = copy_to_user(user_buf,buf,len);
			
		}
		else if (CMDIndex == 1)
		{   
			buf[0]=0xA6;
		    ret=hyn_i2c_read(client,(u8 *)buf,1, (u8 *)buf,8);
			if(ret<0){
				HYN_INFO("hynitron_proc_read_foobar hyn_i2c_read fail. \n");
			}else{
				ret = copy_to_user(user_buf,buf,8);
		    	len = 8;
			}

		}
		if(CMDIndex == 2 || CMDIndex == 3||CMDIndex == 4){

			int data_len=80;
			int report_count=0;

			if(CMDIndex == 2)  //read diff
			{
				buf[0] = 0x00;
				buf[1] = 0x07;
				hyn_ts_data->work_mode = HYN_WORK_MODE_DIFF;	

			}
			else if(CMDIndex == 3)         //rawdata
			{  
				buf[0] = 0x00;
				buf[1] = 0x06;
				hyn_ts_data->work_mode = HYN_WORK_MODE_RAWDATA;	

			}
			else if(CMDIndex == 4)         //factory
			{  
				buf[0] = 0x00;
				buf[1] = 0x04;
				hyn_ts_data->work_mode = HYN_WORK_MODE_FACTORY;	

			}
			ret = hyn_i2c_write(client, buf, 2);  
			if(ret < 0) 
			{			
				HYN_INFO("Write command raw/diff mode failed.error:%d.\n", ret);
				goto END;
			}

			mdelay(10);
		
			for(report_count=0;report_count<16;report_count++){
				unsigned char temp_buf[7];
				ret = i2c_master_recv(client, temp_buf, 6);
				if(ret < 0) 		
				{
					HYN_INFO("Read raw/diff data failed.error:%d.\n", ret);
					goto END;
				}
				memcpy((unsigned char *)buf+2+report_count*5,(unsigned char *)temp_buf+1,5);
			}	

			buf[0] = 0x00;
			buf[1] = 0x00;
			ret = hyn_i2c_write(client, buf, 2);  
			if(ret < 0) 
			{			
				HYN_INFO("Write command raw/diff mode failed.error:%d.\n", ret);
				goto END;
			}

			buf[0] = 4;
			buf[1] = 10;	
	    	ret = copy_to_user(user_buf,buf,data_len+2);
	    	len = data_len + 2;
			

		}

	}

END:	
	hyn_ts_data->work_mode = HYN_WORK_MODE_NORMAL;
	CMDIndex = 0;	

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_ON);
#endif

	return len;
}

static ssize_t hynitron_proc_write_foobar(struct file *file, const char __user *buffer,size_t count, loff_t *data)
{
    unsigned char cmd[258];
	unsigned char buf[4];
    unsigned char *pdata = NULL;
	int len;
	int ret;
    int length;
	struct i2c_client *client = (struct i2c_client *)PDE_DATA(file_inode(file));


	if (count > 256) 
		len = 256;
	else 
		len = count;

	if (copy_from_user(cmd, buffer, len))  
	{
		HYN_INFO("copy data from user space failed.\n");
		return -EFAULT;
	}
#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_OFF);
#endif


	HYN_INFO("hynitron_proc_write_foobar*********cmd:%d*****%d******len:%d .\r\n", cmd[0], cmd[1], len);
	
	if(client==NULL){
		client=hyn_ts_data->client;
		HYN_INFO("client is null.\n");
	}
 
	if (cmd[0] == 0) 
	{   
	    if(hyn_ts_data->fw_length!=0){
			length=hyn_ts_data->fw_length;
		}else{
			HYN_ERROR("hyn_ts_data->fw_length error:%d.\n",hyn_ts_data->fw_length);
			return -ENOMEM;
		}

		HYN_INFO("config length:%d.\n",length);
	    pdata = kzalloc(sizeof(char)*length, GFP_KERNEL);
	    if(pdata == NULL) 
		{
	        HYN_INFO("zalloc GFP_KERNEL memory fail.\n");
	        return -ENOMEM;
	    }
		ret = hynitron_read_fw_file(&cmd[1], pdata, &length);
	  	if(ret < 0) 
	  	{
			HYN_INFO("hynitron_read_fw_file fail.\n");
			if(pdata != NULL) 
			{
				kfree(pdata);
				pdata = NULL;	
			}				
			return -EPERM;
	  	}
		ret = hynitron_apk_fw_dowmload(client, pdata, length);
	  	if(ret < 0)
	  	{
	        HYN_INFO("update firmware failed.\n");
			if(pdata != NULL) 
			{
				kfree(pdata);
				pdata = NULL;	
			}	
	        return -EPERM;
		}
		
	}
	else if (cmd[0] == 2) 
	{					
		//hynitron_touch_release();		
		CMDIndex = cmd[1];			
	}			
	else if (cmd[0] == 3)
	{				
		CMDIndex = 0;
		
		buf[0] = 0x00;
		buf[1] = 0x00;
		ret = hyn_i2c_write(client, buf, 2);  
		if(ret < 0) 
		{			
			HYN_INFO("Write command raw/diff mode failed.error:%d.\n", ret);		
		}
	}else{

		HYN_INFO("cmd[0] error:%d.\n",cmd[0]);
	}

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_ON);
#endif

			
	return count;
}
static const struct file_operations proc_tool_debug_fops = {

	.owner		= THIS_MODULE,

	.read	    = hynitron_proc_read_foobar,

	.write		= hynitron_proc_write_foobar, 	

};
static const struct file_operations proc_tool_test_fops = {

	.owner		= THIS_MODULE,

	.read	    = hynitron_proc_test_read_foobar,

	.write		= hynitron_proc_test_write_foobar, 	

};
 int  hynitron_proc_fs_init(void)
{

	int ret;
	HYN_FUNC_ENTER();

	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){
        //mutcap
		g_proc_dir = proc_mkdir(HYNITRON_PROC_DIR_NAME, NULL);	
		if (g_proc_dir == NULL){
			ret = -ENOMEM;
			goto out;
		}

		g_proc_test_dir = proc_mkdir(HYNITRON_PROC_TEST_DIR_NAME, NULL);	
		if (g_proc_test_dir == NULL){
			ret = -ENOMEM;
			goto out;
		}
		
	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){
      //selfcap
		g_proc_dir = proc_mkdir(HYNITRON_PROC_DIR_NAME_2, NULL);	
	}

	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){
        //mutcap
		g_update_file = proc_create_data(HYNITRON_PROC_FILE_NAME, 0777 | S_IFREG, g_proc_dir, &proc_tool_debug_fops, (void *)hyn_ts_data->client);
		if (NULL == g_update_file) {
			ret = -ENOMEM;
	      	HYN_INFO("proc_create_data HYNITRON_PROC_FILE_NAME failed.\n");
	      	goto no_foo;
		}
		g_update_test_file = proc_create_data(HYNITRON_PROC_TEST_FILE_NAME, 0777 | S_IFREG, g_proc_test_dir, &proc_tool_test_fops, (void *)hyn_ts_data->client);
		if (NULL == g_update_test_file) {
			ret = -ENOMEM;
	      	HYN_INFO("proc_create_data HYNITRON_PROC_TEST_FILE_NAME failed.\n");
	      	goto no_foo;
		}


	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){
      //selfcap
		g_update_file = proc_create_data(HYNITRON_PROC_FILE_NAME_2, 0777 | S_IFREG, g_proc_dir, &proc_tool_debug_fops, (void *)hyn_ts_data->client);
	}
	
 	HYN_FUNC_EXIT();
	return 0;

no_foo:
if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){
	remove_proc_entry(HYNITRON_PROC_FILE_NAME, g_proc_dir);
	remove_proc_entry(HYNITRON_PROC_TEST_FILE_NAME, g_proc_test_dir);
}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){
	remove_proc_entry(HYNITRON_PROC_FILE_NAME_2, g_proc_dir);
}
out:
	return ret;
} 
void hynitron_proc_fs_exit(void)
{
	if(g_proc_dir==NULL) return;
	
	g_proc_dir = proc_mkdir(HYNITRON_PROC_DIR_NAME, NULL);
	if (g_proc_dir != NULL) {
		remove_proc_entry(HYNITRON_PROC_FILE_NAME, g_proc_dir);
	}
	g_proc_dir = proc_mkdir(HYNITRON_PROC_DIR_NAME_2, NULL);
	if (g_proc_dir != NULL) {
		remove_proc_entry(HYNITRON_PROC_DIR_NAME_2, g_proc_dir);
	}

	if(g_proc_test_dir==NULL) return;

	g_proc_test_dir = proc_mkdir(HYNITRON_PROC_TEST_DIR_NAME, NULL);
	if (g_proc_test_dir != NULL) {
		remove_proc_entry(HYNITRON_PROC_TEST_FILE_NAME, g_proc_test_dir);
	}

}

 
#endif



#if HYN_SYSFS_NODE_EN 
static struct mutex g_device_mutex;
static DEFINE_MUTEX(g_device_mutex);
static ssize_t hyn_tpfwver_show(struct device *dev,	struct device_attribute *attr,char *buf)
{
	ssize_t num_read_chars = 0;
	u8 buf1[10];
	unsigned int chip_version,module_version,project_version,chip_type,checksum;
	
	//struct i2c_client *client = container_of(dev, struct i2c_client, dev);

	memset((u8 *)buf1, 0, 10);
	mutex_lock(&g_device_mutex);

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_OFF);
#endif

	chip_version=0;
	module_version=0;
	project_version=0;
	chip_type=0;
	checksum=0;

	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP){
		int ret;
		buf1[0] = 0xD1;
		buf1[1] = 0x01;
		ret = cst3xx_i2c_write(hyn_ts_data->client, buf1, 2);
		if (ret < 0) return -1;
		
		mdelay(10);

		buf1[0] = 0xD2;
		buf1[1] = 0x04;
		ret = cst3xx_i2c_read_register(hyn_ts_data->client, buf1, 4);
		if (ret < 0) return -1;	


		chip_type = buf1[3];
		chip_type <<= 8;
		chip_type |= buf1[2];

		
		project_version |= buf1[1];
		project_version <<= 8;
		project_version |= buf1[0];

		buf1[0] = 0xD2;
		buf1[1] = 0x08;
		ret = cst3xx_i2c_read_register(hyn_ts_data->client, buf1, 4);
		if (ret < 0) return -1;	


		chip_version = buf1[3];
		chip_version <<= 8;
		chip_version |= buf1[2];
		chip_version <<= 8;
		chip_version |= buf1[1];
		chip_version <<= 8;
		chip_version |= buf1[0];

		buf1[0] = 0xD2;
		buf1[1] = 0x0C;
		ret = cst3xx_i2c_read_register(hyn_ts_data->client, buf1, 4);
		if (ret < 0) return -1;	


		checksum = buf1[3];
		checksum <<= 8;
		checksum |= buf1[2];
		checksum <<= 8;
		checksum |= buf1[1];
		checksum <<= 8;
		checksum |= buf1[0];	

		buf1[0] = 0xD1;
		buf1[1] = 0x09;
		ret = cst3xx_i2c_write(hyn_ts_data->client, buf1, 2);

		num_read_chars = snprintf(buf, 128, "chip_version: 0x%02X,module_version:0x%02X,project_version:0x%02X,chip_type:0x%02X,checksum:0x%02X .\n",chip_version,module_version, project_version,chip_type,checksum);
	
	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){
	
		buf1[0]=0xA6;
		if (hyn_i2c_read(hyn_ts_data->client,(u8 *)buf1, 1, (u8 *)buf1,8) < 0)
			num_read_chars = snprintf(buf, 128,"get tp fw version fail!\n");
		else{
			chip_version  =buf1[0];
			chip_version |=buf1[1]<<8;

			module_version=buf1[2];
			project_version=buf1[3];

			chip_type  =buf1[4];
			chip_type |=buf1[5]<<8;

			checksum  =buf1[6];
			checksum |=buf1[7]<<8;
			
			num_read_chars = snprintf(buf, 128, "chip_version: 0x%02X,module_version:0x%02X,project_version:0x%02X,chip_type:0x%02X,checksum:0x%02X .\n",chip_version,module_version, project_version,chip_type,checksum);
		}
	}
	mutex_unlock(&g_device_mutex);


#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_ON);
#endif
	return num_read_chars;
}

static ssize_t hyn_tpfwver_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t count)
{
	/*place holder for future use*/
	return -EPERM;
}


static ssize_t hyn_tprwreg_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	/*place holder for future use*/
	return -EPERM;
}

static ssize_t hyn_tprwreg_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t count)
{
	//struct i2c_client *client = container_of(dev, struct i2c_client, dev);
	ssize_t num_read_chars = 0;
	int retval;
	long unsigned int wmreg = 0;
	u8 regaddr = 0xff, regvalue = 0xff;
	u8 valbuf[5] = {0};

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_OFF);
#endif
	memset(valbuf, 0, sizeof(valbuf));
	mutex_lock(&g_device_mutex);
	num_read_chars = count - 1;

	HYN_INFO(" input character:%d.\n",num_read_chars);

	memcpy(valbuf, buf, num_read_chars);
	retval = kstrtoul(valbuf, 16, &wmreg);

	if (0 != retval) {
		HYN_INFO("%s() - ERROR: The given input was: \"%s\"\n",__func__, buf);
		goto error_return;
	}
	if (1 == num_read_chars){

		regaddr = valbuf[0];
		HYN_INFO("register(0x%x).\n",regaddr);

/**********************************************
		//0-ascll-0x30:2??¨¬|????????¡ì?¨¨?o?
		//1-ascll-0x31:1???¡ì?¨¨??D??
		//2-ascll-0x32:????¨¬???a?D??
		//3-ascll-0x33:?????D???
		//4-ascll-0x34:??¨¬|y??????¨¬??¨¬?1??¨¬??t
		//5-ascll-0x35:
		//6-ascll-0x36:
		//7-ascll-0x37:
		//8-ascll-0x38:
		//9-ascll-0x39:
****************************************/
		if(regaddr==0x30){   
			cst3xx_firmware_info(hyn_ts_data->client);
		}else if(regaddr==0x31){
			HYN_INFO("hyn_irq_disable enter.\n");
			hyn_irq_disable();
		}else if(regaddr==0x32){
			HYN_INFO("hyn_irq_enable enter.\n");
			hyn_irq_enable();
		}else if(regaddr==0x33){
			HYN_INFO("hyn_reset_proc enter.\n");
			hyn_reset_proc(10);
		}else if(regaddr==0x34){
			HYN_INFO("hyn_boot_update_fw enter.\n");
#if  HYN_ESDCHECK_EN
			hyn_esd_switch(SWITCH_ESD_OFF);
#endif
			hyn_ts_data->apk_upgrade_flag=0;
			hyn_boot_update_fw(hyn_ts_data->client);
#if  HYN_ESDCHECK_EN
			hyn_esd_switch(SWITCH_ESD_ON);
#endif
			
		}else if(regaddr==0x35){
		
		}else if(regaddr==0x36){
		
		}else if(regaddr==0x37){
			
		}
	}
	else if (2 == num_read_chars){
		/*read register*/

		regaddr = valbuf[0]<<8;
		regaddr |= valbuf[1];
		HYN_INFO("register(0x%02x).\n",regaddr);
		
/**********************************************
		//11-ascll-0x3131: ??¨¬|??¨¬??¨¬??1??¡ì?¡ì??¡ì?¨¦?????¡ìo??¨¬o?1
		***
		//19-ascll-0x3139: ??¨¬|??¨¬??¨¬??1??¡ì?¡ì??¡ì?¨¦?????¡ìo??¨¬o?9
****************************************/
		if(regaddr==0x3131){  
			
		}else if(regaddr==0x3132){
			
		}else if(regaddr==0x3133){
			
		}else if(regaddr==0x3134){
			
		}
	}else if(num_read_chars>2)
	{
		regaddr = valbuf[0]<<8;
		regaddr |= valbuf[1];
		HYN_INFO("register(0x%02x).\n",regaddr);

		if (hyn_i2c_read_bytes(regaddr,  &regvalue,2,2) < 0)
			HYN_INFO("Could not write the register(0x%02x)\n",regaddr);
		else
			HYN_INFO("Write 0x%02x into register(0x%02x) successful\n",regvalue, regaddr);
	}

error_return:
	mutex_unlock(&g_device_mutex);
#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_ON);
#endif
	

	return count;
}

static ssize_t hyn_fwupdate_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	/* place holder for future use */
	return -EPERM;
}

/*upgrade from *.i*/
static ssize_t hyn_fwupdate_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t count)
{
	
	//struct i2c_client *client = container_of(dev, struct i2c_client, dev);
	HYN_INFO("hyn_fwupdate_store enter.\n");
	mutex_lock(&g_device_mutex);

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_OFF);
#endif
	
	hyn_ts_data->apk_upgrade_flag=0;
	hyn_boot_update_fw(hyn_ts_data->client);
	mutex_unlock(&g_device_mutex);

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_ON);
#endif
	return count;
}

static ssize_t hyn_fwupgradeapp_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	/*place holder for future use*/
	return -EPERM;
}


/*upgrade from app.bin*/
static ssize_t hyn_fwupgradeapp_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t count)
{
	char fwname[256];
	int ret;
	unsigned char *pdata = NULL;
	int length;
	//struct i2c_client *client = container_of(dev, struct i2c_client, dev);

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_OFF);
#endif
	HYN_INFO("hyn_fwupgradeapp_store enter.\n");

	memset(fwname, 0, sizeof(fwname));
	sprintf(fwname, "/mnt/%s", buf);	
	fwname[count-1+5] = '\0';


    if(hyn_ts_data->fw_length!=0){
		length=hyn_ts_data->fw_length;
	}else{
		HYN_ERROR("hyn_ts_data->fw_length error:%d.\n",hyn_ts_data->fw_length);
		return -ENOMEM;
	}
	HYN_INFO("fwname:%s.length:%d.\n",fwname,length);
	
	pdata = kzalloc(sizeof(char)*length, GFP_KERNEL);
    if(pdata == NULL) 
	{
        HYN_INFO("hyn_fwupgradeapp_store GFP_KERNEL memory fail.\n");
        return -ENOMEM;
    }

	mutex_lock(&g_device_mutex);

	ret = hynitron_read_fw_file(fwname, pdata, &length);
  	if(ret < 0) 
  	{
		HYN_INFO("hynitron_read_fw_file fail.\n");
		if(pdata != NULL) 
		{
			kfree(pdata);
			pdata = NULL;	
		}			
  	}else{

		ret = hynitron_apk_fw_dowmload(hyn_ts_data->client, pdata, length);
	  	if(ret < 0)
	  	{
	        HYN_INFO("hynitron_apk_fw_dowmload failed.\n");
			if(pdata != NULL) 
			{
				kfree(pdata);
				pdata = NULL;	
			}	
		}
	}
	mutex_unlock(&g_device_mutex);

#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_ON);
#endif	
	HYN_INFO("hyn_fwupgradeapp_store exit.\n");
	
	return count;
}

static ssize_t hyntpfactorytest_show (struct device* dev, struct device_attribute* attr, char* buf)
{
    /* place holder for future use */
 	ssize_t num_read_chars = 0;
	HYN_INFO ("hyntpfactory_show enter.\n");
#if HYN_AUTO_FACTORY_TEST_EN	
        num_read_chars = sprintf (buf, "%s.\n",factory_test);	
#endif
    return num_read_chars;
}

/*upgrade from *.i*/
static ssize_t hyntpfactorytest_store (struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
    //struct i2c_client *client = container_of(dev, struct i2c_client, dev);
    HYN_INFO ("hyntpfactory_store enter.\n");
#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_OFF);
#endif
#if HYN_AUTO_FACTORY_TEST_EN
	hyn_factory_touch_test();
#endif
#if  HYN_ESDCHECK_EN
	hyn_esd_switch(SWITCH_ESD_ON);
#endif
    return count;
}

/*sysfs */
/*get the fw version
*example:cat hyntpfwver
*/
static DEVICE_ATTR(hyntpfwver, S_IRUGO | S_IWUSR, hyn_tpfwver_show,
			hyn_tpfwver_store);

/*upgrade from *.i
*example: echo 1 > hynfwupdate
*/
static DEVICE_ATTR(hynfwupdate, S_IRUGO | S_IWUSR, hyn_fwupdate_show,
			hyn_fwupdate_store);

/*read and write register
*read example: echo 88 > hyntprwreg ---read register 0x88
*write example:echo 8807 > hyntprwreg ---write 0x07 into register 0x88
*
*note:the number of input must be 2 or 4.if it not enough,please fill in the 0.
*/
static DEVICE_ATTR(hyntprwreg, S_IRUGO | S_IWUSR, hyn_tprwreg_show,
			hyn_tprwreg_store);


/*upgrade from app.bin
*example:echo "*_app.bin" > hynfwupgradeapp
*/
static DEVICE_ATTR(hynfwupgradeapp, S_IRUGO | S_IWUSR, hyn_fwupgradeapp_show,
			hyn_fwupgradeapp_store);

/*factory test 
*example:cat hynfactorytest
*/
static DEVICE_ATTR(hyntpfactorytest, S_IRUGO | S_IWUSR, hyntpfactorytest_show,
			hyntpfactorytest_store);

/*add your attr in here*/
static struct attribute *hyn_attributes[] = {
	&dev_attr_hyntpfwver.attr,
	&dev_attr_hynfwupdate.attr,
	&dev_attr_hyntprwreg.attr,
	&dev_attr_hynfwupgradeapp.attr,
	&dev_attr_hyntpfactorytest.attr,
	NULL
};

static struct attribute_group hyn_attribute_group = {
	.attrs = hyn_attributes
};
/*create sysfs for debug*/

int hyn_create_sysfs(struct i2c_client *client)
{
	int err;
	HYN_FUNC_ENTER();
	hyn_ts_data->client=client;
  	if ((hyn_ts_data->k_obj = kobject_create_and_add("hynitron_debug", NULL)) == NULL ) {
     	HYN_INFO("hynitron_debug sys node create error.\n"); 
		return  -EIO;
    }
	err = sysfs_create_group(hyn_ts_data->k_obj, &hyn_attribute_group);
	if (0 != err) {
		HYN_INFO("%s() - ERROR: sysfs_create_group() failed.\n",__func__);
		sysfs_remove_group(hyn_ts_data->k_obj, &hyn_attribute_group);
		return -EIO;
	} else {
		mutex_init(&g_device_mutex);
		HYN_INFO("%s() - sysfs_create_group() succeeded.\n",__func__);
	}
	HYN_FUNC_EXIT();
	return err;
}

void hyn_release_sysfs(struct i2c_client *client)
{
	if(hyn_ts_data->k_obj==NULL) return;
	
	sysfs_remove_group(hyn_ts_data->k_obj, &hyn_attribute_group);
	mutex_destroy(&g_device_mutex);		
}
#endif 


#if HYN_AUTO_FACTORY_TEST_EN

#define HYN_FACTORY_TEST_LOG_TXT  "/sdcard/hyn_factory_test.txt" 

int hyn_factory_test_judge(unsigned char *pdata)
{
  unsigned int open_low,open_high,short_value;
  unsigned int open_delta,open_delta_last;
  unsigned short rx,tx;
  unsigned char *p_data;
  unsigned char p_test_data[80];
  int test_fail_flag=0;

  struct file *fp;  
  mm_segment_t fs;  
  loff_t pos; 
  unsigned int data_len;
  
    p_data=pdata;
	test_fail_flag=0;

	open_delta=1;
	open_delta_last=1;

    hyn_ts_data->sensor_tx=26;
    hyn_ts_data->sensor_rx=13;

    data_len=hyn_ts_data->sensor_tx*hyn_ts_data->sensor_tx*4+(hyn_ts_data->sensor_tx+hyn_ts_data->sensor_tx)*2;

	HYN_DEBUG("enter factory test, start save test data.\n");


	HYN_DEBUG ("touch_test enter.\n");


	fp = filp_open(HYN_FACTORY_TEST_LOG_TXT, O_RDWR | O_CREAT, 0644);  
	if (IS_ERR(fp)) {  
	    HYN_ERROR("create test data file error.\n");  
	    return -1;  
	} 
	
  	fs = get_fs();  
    set_fs(KERNEL_DS);
    pos = fp->f_pos; 
//	pos=0;
	snprintf (p_test_data,80, "touch_test enter.\n");
    vfs_write(fp, p_test_data, sizeof(p_test_data), &pos); 
//update pos num
	snprintf (p_test_data,80, "test param:open_max=%d,open_min=%d,high_min=%d,ratio=%d.\n",HYN_TP_OPEN_VALUE_MAX,HYN_TP_OPEN_VALUE_MIN,HYN_TP_OPEN_DRV_VALUE_MIN,HYN_TP_OPEN_DROP_RATIO);
    vfs_write(fp, p_test_data, sizeof(p_test_data), &pos);	

  	HYN_DEBUG("enter factory test,end save test data.\n");

  	data_len=hyn_ts_data->sensor_tx*hyn_ts_data->sensor_tx*4+(hyn_ts_data->sensor_tx+hyn_ts_data->sensor_tx)*2;

  	HYN_DEBUG("enter factory test open.\n");
  	for(tx=0;tx<hyn_ts_data->sensor_tx;tx++){
  	for(rx=0;rx<hyn_ts_data->sensor_rx;rx++){		
		open_low =((*(p_data+1))<<8)+(*p_data);
		open_high=((*(p_data+1+hyn_ts_data->sensor_tx*hyn_ts_data->sensor_rx*2))<<8)+(*p_data+hyn_ts_data->sensor_tx*hyn_ts_data->sensor_rx*2);
		open_delta=open_high-open_low;

		snprintf (p_test_data,80, "tx=%d,rx=%d,open_low=%d,open_high=%d,open_delta=%d.\n",tx,rx,open_low,open_high,open_delta);
    	vfs_write(fp, p_test_data, sizeof(p_test_data), &pos);  

		if(open_low<HYN_TP_OPEN_LOW_VALUE_MIN)
		{
			HYN_DEBUG ("factory_test_judge test open fail,tx=%d,rx=%d,open_low=%d.\n",tx,rx,open_low);
			sprintf (factory_test, "0,factory_test_judge test open fail,tx=%d,rx=%d,open_low=%d.\n", tx,rx,open_low);
			test_fail_flag=1;
			//return -1;
		}
		if(open_high<HYN_TP_OPEN_DRV_VALUE_MIN){
			HYN_DEBUG ("factory_test_judge test open fail,tx=%d,rx=%d,open_high=%d.\n",tx,rx,open_high);
			sprintf (factory_test, "0,factory_test_judge test open fail,tx=%d,rx=%d,open_high=%d.\n", tx,rx,open_high);
			test_fail_flag=1;
			//return -1;
		}
		if((open_delta<HYN_TP_OPEN_VALUE_MIN)||(open_delta>HYN_TP_OPEN_VALUE_MAX)){
			HYN_DEBUG ("factory_test_judge test open fail,tx=%d,rx=%d,open_delta=%d.\n",tx,rx,open_delta);
			sprintf (factory_test, "0,factory_test_judge test open fail,tx=%d,rx=%d,open_delta=%d.\n", tx,rx,open_delta);
			test_fail_flag=1;	
			//return -1;
		}
		if((((open_delta_last*HYN_TP_OPEN_DROP_RATIO/10)<open_delta)
		||((open_delta_last*10/HYN_TP_OPEN_DROP_RATIO)>open_delta))&&(rx>0)){
			HYN_DEBUG ("factory_test_judge test open fail,tx=%d,rx=%d,open_delta=%d.open_delta_last=%d.\n",tx,rx,open_delta,open_delta_last);
			sprintf (factory_test, "0,factory_test_judge test open fail,tx=%d,rx=%d,open_delta=%d,open_delta_last=%d.\n", tx,rx,open_delta,open_delta_last);
			test_fail_flag=1;
			//return -1;
		}
	    open_delta_last=open_delta;
	    p_data+=2;
  }
  }
  HYN_DEBUG ("factory_test_judge test short.\n");
  p_data=pdata+(hyn_ts_data->sensor_tx*hyn_ts_data->sensor_rx*4);
  rx=0;
  for(tx=0;tx<(hyn_ts_data->sensor_tx+hyn_ts_data->sensor_rx);tx++)
  {   
	short_value =((*(p_data+1))<<8)+(*p_data);
	short_value=(2000/(short_value));
	snprintf (p_test_data,80, "tx=%d,rx=%d,short_value=%d.\n",tx,rx,short_value);
    vfs_write(fp, p_test_data, sizeof(p_test_data), &pos); 
    if(short_value<HYN_TP_SHORT_RX_TX_RESISTANCE){	
		HYN_DEBUG ("factory_test_judge test short fail,tx=%d,short_value=%d.\n",tx,short_value);
		sprintf (factory_test, "0,factory_test_judge test short fail,tx=%d,short_value=%d.\n", tx,short_value);
		test_fail_flag=1;
		//return -1;
	}	
	p_data+=2;

  }
  if(test_fail_flag==0)
  {
	sprintf (factory_test, "1,factory_test_judge pass=1.\n");
	
	snprintf (p_test_data,80, "1,factory_test_judge psss=1.\n");
    vfs_write(fp, p_test_data, sizeof(p_test_data), &pos); 
  }else{
    sprintf (factory_test, "0,factory_test_judge fail=0.\n"); 
	
	snprintf (p_test_data,80, "0,factory_test_judge fail=0.\n");
    vfs_write(fp, p_test_data, sizeof(p_test_data), &pos); 
  }
  snprintf (p_test_data,80, "******hyn_factory_test_judge done*****.\n");
  vfs_write(fp, p_test_data, sizeof(p_test_data), &pos); 
  
  fp->f_pos = pos;
  set_fs(fs);
  filp_close(fp, NULL);  
  HYN_DEBUG ("factory_test_judge test end .\n");
  return 0;
}

int hyn_factory_touch_test(void)
{
	unsigned char *buf;
	int ret;
    unsigned short rx,tx;
    int data_len;

	hyn_ts_data->sensor_rx=13;
	hyn_ts_data->sensor_tx=26;
	
    rx = hyn_ts_data->sensor_rx;
    tx = hyn_ts_data->sensor_tx;


	buf = kzalloc (sizeof (char) * 1800, GFP_KERNEL);
	
	HYN_DEBUG ("touch_test enter.\n");
	if (buf == NULL) {
		HYN_ERROR("touch_test GFP_KERNEL memory fail.\n");
		return -1;
	}
	hyn_ts_data->work_mode = HYN_WORK_MODE_FACTORY;
    {
        buf[0] = 0xD1;
        buf[1] = 0x19;
        data_len = rx*tx*2;
    }

    ret = cst3xx_i2c_write(hyn_ts_data->client, buf, 2);
    if(ret < 0)
    {
        HYN_ERROR("touch_test factory test mode failed.error:%d.\n", ret);
        return -1;
    }
    mdelay(14);
	hyn_ts_data->hyn_irq_flag = 0;
	hynitron_wait_event_wait();
	hyn_ts_data->hyn_irq_flag = 0;

	//lowDrv_data
	buf[0] = 0x10;
	buf[1] = 0x00;	
    ret = cst3xx_i2c_write(hyn_ts_data->client, buf, 2);
    if(ret < 0){
        HYN_ERROR("touch_test write register failed.error:%d.\n", ret);
		return -1;
    }

    ret = cst3xx_i2c_read(hyn_ts_data->client, &buf[2], data_len);
    if(ret < 0){
        HYN_ERROR("touch_test read fatory data failed.error:%d.\n", ret);
		return -1;
    }
	//HighDrv_Data
	buf[0] = 0x30;
	buf[1] = 0x00;	
    ret = cst3xx_i2c_write(hyn_ts_data->client, buf, 2);
    if(ret < 0){
        HYN_ERROR("touch_test write register failed.error:%d.\n", ret);
		return -1;
    }

    ret = cst3xx_i2c_read(hyn_ts_data->client, &buf[2+data_len], data_len);
    if(ret < 0){
        HYN_ERROR("touch_test read fatory data failed.error:%d.\n", ret);
		return -1;
    }

	buf[0] = 0x50;
	buf[1] = 0x00;	
	data_len=(rx+tx)*2;
    ret = cst3xx_i2c_write(hyn_ts_data->client, buf, 2);
    if(ret < 0){
        HYN_ERROR("touch_test write register failed.error:%d.\n", ret);
		return -1;
    }
   
    ret = cst3xx_i2c_read(hyn_ts_data->client, &buf[2+(rx*tx*2*2)], data_len);
    if(ret < 0){
        HYN_ERROR("touch_test read fatory data failed.error:%d.\n", ret);
		return -1;
    }

	hyn_ts_data->work_mode=HYN_WORK_MODE_NORMAL;
    hyn_reset_proc (40);
    mdelay(10);
    buf[0] = 0xD1;
    buf[1] = 0x09;
    ret = cst3xx_i2c_write(hyn_ts_data->client, buf, 2); 
	

    ret=hyn_factory_test_judge((unsigned char *)&buf[2]);
	if(ret < 0){
        HYN_ERROR("touch_test cst1xx_factory_test_judge failed.error:%d.\n", ret);
		return -1;
    }else{
 		HYN_DEBUG ("factory_test_judge test success .\n");
		return 0;
	}
 


}


#endif

