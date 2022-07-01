/**
 *Name        : hynitron_core.h
 *Author      : steven
 *Version     : V2.0
 *Create      : 2019-11-11
 *Copyright   : zxzz
 */


#ifndef HYNITRON_CORE_H__
#define HYNITRON_CORE_H__


#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/i2c.h>
#include <linux/vmalloc.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
//#include <mach/irqs.h>
#include <linux/jiffies.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <linux/version.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
//#include <linux/rtpm_prio.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/file.h>
#include <linux/proc_fs.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>

#include <linux/wait.h>
#include <linux/time.h>
#include <linux/dma-mapping.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
//#include <linux/wakelock.h>
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/err.h>
#include <linux/irq.h>

//#include "wakelock.h"

#include <linux/sysfs.h>
#include <linux/init.h>
//#include <linux/spinlock.h>
#include <linux/firmware.h>
#include <linux/regulator/consumer.h>

#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/moduleparam.h>

#include <linux/platform_device.h>
#include <linux/miscdevice.h>

#include "hynitron_common.h"

//please config the chip series before using.

#define HYN_CHIP_TYPE_CONFIG						CST340
#define HYN_IRQ_TRIGGER_RISING_CONFIG	    	    0x01 
#define HYN_MAIN_IIC_ADDR_CONFIG		    	    0x1A 

#define HYN_X_DISPLAY_DEFAULT              	 	    720
#define HYN_Y_DISPLAY_DEFAULT               	    1280
#define HYN_X_REVERT		              	 	    0
#define HYN_Y_REVERT		               	        0
#define HYN_XY_EXCHANGE		               	        0

#define HYN_MAX_KEYS						        3
#define HYN_MAX_POINTS                           	5
#define HYN_MAX_SELFCAP_ID  						2

/**********************************************************/
#define HYN_RESET_SOFTWARE				   			0
#define HYN_UPDATE_FIRMWARE_FORCE					0
#define HYN_IIC_TRANSFER_LIMIT						0

/******************************************/

#define HYN_UPDATE_FIRMWARE_PATH_1 					"/data"
#define HYN_UPDATE_FIRMWARE_PATH_2 					"/sdcard"

#define HYN_UPDATE_FIRMWARE_FILTNAME_1 				"hyn.bin"

#define HYN_COORDS_ARR_SIZE						2
#define HYN_TOUCH_STEP                          6
#define HYN_TOUCH_X_H_POS                       3
#define HYN_TOUCH_X_L_POS                       4
#define HYN_TOUCH_Y_H_POS                       5
#define HYN_TOUCH_Y_L_POS                       6
#define HYN_TOUCH_EVENT_POS                     3
#define HYN_TOUCH_ID_POS                        5
#define HYN_TOUCH_POINT_NUM                     2
#define HYN_TOUCH_XY_POS                        7
#define HYN_TOUCH_MISC                          8
#define POINT_READ_BUF                          (3 + HYN_TOUCH_STEP * HYN_MAX_POINTS)
#define HYN_SYSFS_ECHO_ON(buf)      ((strnicmp(buf, "1", 1)  == 0) || \
                                        (strnicmp(buf, "on", 2) == 0))
#define HYN_SYSFS_ECHO_OFF(buf)     ((strnicmp(buf, "0", 1)  == 0) || \
                                        (strnicmp(buf, "off", 3) == 0))


/*****************************************************************************
* Private enumerations, structures and unions using typedef
*****************************************************************************/

struct hynitron_chip_type_array {
	const u16 chip_type;
	const u16 chip_series;	
	const u8 chip_product_line;
	const u8 chip_boot_addr;
};


struct hynitron_fw_array {
	char* name;
    unsigned char *fw;
    u16 project_id;
	u16 module_id;
	u16 chip_type;
	u16 fw_length;		
};


struct touch_info
{
    int y[HYN_MAX_POINTS];
    int x[HYN_MAX_POINTS];
    int p[HYN_MAX_POINTS];
    int id[HYN_MAX_POINTS];
    int count;
};


/*touch event info*/
struct ts_event
{
    u16 au16_x[HYN_MAX_POINTS];               /* x coordinate */
    u16 au16_y[HYN_MAX_POINTS];               /* y coordinate */
    u8 au8_touch_event[HYN_MAX_POINTS];       /* touch event: 0 -- down; 1-- up; 2 -- contact */
    u8 au8_finger_id[HYN_MAX_POINTS];         /* touch ID */
    u16 pressure[HYN_MAX_POINTS];
    u16 area[HYN_MAX_POINTS];
    u8 touch_point;
    int touchs;
    u8 touch_point_num;
};
struct hynitron_ts_platform_data{
	int irq_gpio;
	u32 irq_gpio_flags;
	int reset_gpio;
	u32 reset_gpio_flags;
	u32 x_resolution;
	u32 y_resolution;
	u32  max_key_num;
	u32  max_touch_num;

	u32  key_x_coords[8]; // max support 8 keys
	u32  key_y_coords[8];
	u32  key_code[8];

//erobbing add for check if overturn axis of x and y
    bool xy_exchange;
	bool x_overturn;
	bool y_overturn;
//end
	bool have_key;
	bool report_key_position;
	bool i2c_pull_up;
	bool enable_power_off;
	bool enable_slot_report;
	bool enable_esd;

};

struct hynitron_ts_data{
    struct hynitron_ts_platform_data *pdata;
    struct i2c_client *client;
	struct device *dev;
  	struct input_dev  *input_dev;
	struct task_struct *thread_tpd;
   	struct hrtimer timer;
   	struct work_struct  work;
	struct workqueue_struct *hyn_workqueue;
	//spinlock_t irq_lock;
	u8  device_id;
	u8  irq_is_disable;
	s32 use_irq;
	u8  int_trigger_type;
	u8  sensor_tx;
	u8  sensor_rx;
	u8  hyn_irq_flag;
	u8  report_flag;
	u8  key_index;
	u8 work_mode;
	
	u8  hyn_esd_mode    :1 ;
	u8  hyn_esd_running :1 ;
	u8  hyn_esd_i2c_lock_flag: 1;
	s32 hyn_esd_pre_counter;
	s32 hyn_esd_cur_counter;
 	s32 hyn_esd_work_cycle;
	struct mutex hyn_esd_lock;
	struct delayed_work hyn_esd_check_work;
	struct workqueue_struct *hyn_esd_workqueue;
	

	u16  chip_ic_type;
	u16  chip_ic_series;
	u8   chip_ic_product_line;
	u32  chip_ic_checksum;
	u32  chip_ic_checkcode;
	u32  chip_ic_fw_version;
  	u16  chip_ic_project_id;	
	u16  chip_ic_module_id;
	u16  chip_ic_workmode;
	u8   chip_ic_bootloader_addr;
	u8   chip_ic_main_addr;

	u16  config_chip_type;
	u16  config_chip_series;
	u8   config_chip_product_line;
	
	u8	 fw_updating;
	u8*  fw_name;
	u32  fw_length;
	u8   *p_hynitron_upgrade_firmware;
	u8   apk_upgrade_flag;
	u8   apk_rdwr_flag;	
	
	bool    in_glove_mode;
    bool    power_on;
   // struct mutex report_mutex;
	struct kobject *k_obj;

	
    struct regulator *vdd;
	struct regulator *vcc_i2c;
	
#if defined(CONFIG_FB)
    struct notifier_block fb_notif;
    struct work_struct resume_work;
#elif defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend early_suspend;
#endif
	struct pinctrl *ts_pinctrl;
	struct pinctrl_state *gpio_state_active;
	struct pinctrl_state *gpio_state_suspend;

};
extern struct hynitron_ts_data *hyn_ts_data ;



#endif /*HYNITRON_CORE_H__*/


