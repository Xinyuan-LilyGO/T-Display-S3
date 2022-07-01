/**
 *Name        : hynitron_common.h
 *Author      : steven
 *Version     : V2.0
 *Create      : 2019-11-11
 *Copyright   : zxzz
 */


#ifndef HYNITRON_COMMON_H__
#define HYNITRON_COMMON_H__


#include "hynitron_config.h"

/*****************************************************************************
* Macro definitions using #define
*****************************************************************************/
#define HYN_DRIVER_VERSION                  "Hynitron TouchScreen Allwinner Driver V2.1 20201222"

/************THE CHIP PRODUCT LINE SUPPORT********************/
#define HYN_CHIP_PRODUCT_LINE_MUT_CAP						1
#define HYN_CHIP_PRODUCT_LINE_SEL_CAP						2
/*************************************************************/

/************THE CHIP SERIES SUPPORT********************/
#define HYN_CHIP_CST0XX						10
#define HYN_CHIP_CST1XX						100
#define HYN_CHIP_CST1XXSE					101
#define HYN_CHIP_CST2XX						200
#define HYN_CHIP_CST2XXSE					201
#define HYN_CHIP_CST3XX						300
#define HYN_CHIP_CST3XXSE					301
#define HYN_CHIP_CST6XX						692
#define HYN_CHIP_CST7XX						700
#define HYN_CHIP_CST8XX						800
#define HYN_CHIP_CST9XX						900
/******************************************************/

/************THE CHIP TYPE OF SERIES SUPPORT********************/
#define CST016						16
#define CST026						26
#define CST036						36

#define CST126						126
#define CST128						128
#define CST128SE					129
#define CST130						130
#define CST140						140
#define CST148						148

#define CST226						226
#define CST226SE					168
#define CST237						237
#define CST240						240

#define CST326						326
#define CST328						328
#define CST328SE					329
#define CST340						340
#define CST348						348
#define CST356						356
#define CST6928S					692


#define CST716						716
#define CST726						726
#define CST736						736

#define CST816						816
#define CST826						826
#define CST836						836

#define CST912						912
#define CST918						918
/******************************************************/

#define FLAGBIT(x)              (0x00000001 << (x))
#define FLAGBITS(x, y)          ((0xFFFFFFFF >> (32 - (y) - 1)) << (x))

#define I2C_BUFFER_LENGTH_MAXINUM           256
#define FILE_NAME_LENGTH                    128
#define ENABLE                              1
#define DISABLE                             0
#define REG_LEN_1B    						1
#define REG_LEN_2B   						2



/*register address*/
/********selfcap register address start *****************/

#define HYN_REG_CAP_POWER_MODE                  0xA5
#define HYN_REG_CAP_POWER_MODE_SLEEP_VALUE      0x03
#define HYN_REG_CAP_FW_VER                      0xA6 
#define HYN_REG_CAP_VENDOR_ID                   0xA8
#define HYN_REG_CAP_PROJECT_ID                  0xA9
#define HYN_REG_CAP_CHIP_ID                    	0xAA
#define HYN_REG_CAP_CHIP_CHECKSUM              	0xAC


#define HYN_REG_CAP_GESTURE_EN                  0xD0
#define HYN_REG_CAP_GESTURE_OUTPUT_ADDRESS      0xD3

#define HYN_REG_CAP_PROXIMITY_EN                0xB0
#define HYN_REG_CAP_PROXIMITY_OUTPUT_ADDRESS    0x01

#define HYN_REG_CAP_ESD_SATURATE                0xE0
/********selfcap register address end *****************/




/********mutcap register address start *****************/

//Myabe change 
#define HYN_REG_MUT_ESD_VALUE                   0xD040
#define HYN_REG_MUT_ESD_CHECKSUM                0xD046
#define HYN_REG_MUT_PROXIMITY_EN                0xD04B
#define HYN_REG_MUT_PROXIMITY_OUTPUT_ADDRESS    0xD04B
#define HYN_REG_MUT_GESTURE_EN                  0xD04C
#define HYN_REG_MUT_GESTURE_OUTPUT_ADDRESS      0xD04C

//workmode
#define HYN_REG_MUT_DEBUG_INFO_MODE             0xD101
#define HYN_REG_MUT_RESET_MODE            		0xD102
#define HYN_REG_MUT_DEBUG_RECALIBRATION_MODE    0xD104
#define HYN_REG_MUT_DEEP_SLEEP_MODE    			0xD105
#define HYN_REG_MUT_DEBUG_POINT_MODE	    	0xD108
#define HYN_REG_MUT_NORMAL_MODE                 0xD109

#define HYN_REG_MUT_DEBUG_RAWDATA_MODE          0xD10A
#define HYN_REG_MUT_DEBUG_DIFF_MODE             0xD10D
#define HYN_REG_MUT_DEBUG_FACTORY_MODE          0xD119
#define HYN_REG_MUT_DEBUG_FACTORY_MODE_2        0xD120

//debug info
/****************HYN_REG_MUT_DEBUG_INFO_MODE address start***********/
#define HYN_REG_MUT_DEBUG_INFO_IC_CHECKSUM             0xD208
#define HYN_REG_MUT_DEBUG_INFO_FW_VERSION              0xD204
#define HYN_REG_MUT_DEBUG_INFO_IC_TYPE			       0xD202
#define HYN_REG_MUT_DEBUG_INFO_PROJECT_ID			   0xD200 
#define HYN_REG_MUT_DEBUG_INFO_BOOT_TIME               0xD1FC
#define HYN_REG_MUT_DEBUG_INFO_RES_Y            	   0xD1FA
#define HYN_REG_MUT_DEBUG_INFO_RES_X            	   0xD1F8
#define HYN_REG_MUT_DEBUG_INFO_KEY_NUM            	   0xD1F7
#define HYN_REG_MUT_DEBUG_INFO_TP_NRX            	   0xD1F6
#define HYN_REG_MUT_DEBUG_INFO_TP_NTX            	   0xD1F4

/****************HYN_REG_MUT_DEBUG_INFO_MODE address end***********/

#define HYN_WORK_MODE_NORMAL     0
#define HYN_WORK_MODE_FACTORY    1
#define HYN_WORK_MODE_RAWDATA    2
#define HYN_WORK_MODE_DIFF  	 3
#define HYN_WORK_MODE_UPDATE     4


/********mutcap register address end *****************/



/************i2c communication********************/
int cst3xx_i2c_read(struct i2c_client *client, unsigned char *buf, int len); 
int cst3xx_i2c_write(struct i2c_client *client, unsigned char *buf, int len) ;
int cst3xx_i2c_read_register(struct i2c_client *client, unsigned char *buf, int len); 
int hyn_i2c_write_bytes(unsigned short reg,unsigned char *buf,unsigned short len,unsigned char reg_len);
int hyn_i2c_read_bytes(unsigned short reg,unsigned char* buf,unsigned short len,unsigned char reg_len);
int hyn_i2c_write_byte(struct i2c_client *client, u8 regaddr, u8 regvalue);
int hyn_i2c_read_byte(struct i2c_client *client, u8 regaddr, u8 *regvalue);
int hyn_i2c_read(struct i2c_client *client, char *writebuf,int writelen, char *readbuf, int readlen);
int hyn_i2c_write(struct i2c_client *client, char *writebuf, int writelen);
/***************************************/

/************HYN_PSENSOR_EN********************/
#if HYN_PSENSOR_EN
extern int hyn_proximity_readdata(void);
extern void hyn_proximity_init(void);
extern void hyn_proximity_exit(void);
extern void hyn_proximity_suspend(void);
extern void hyn_proximity_resume(void);
#endif
/***************************************/

/************HYN_GESTURE_EN********************/
#if HYN_GESTURE_EN
extern int hyn_gesture_init(struct input_dev *input_dev, struct i2c_client *client);
extern int hyn_gesture_exit(void);
extern int hyn_gesture_suspend(void);
extern int hyn_gesture_resume(void);
extern int hyn_gesture_readdata(void);
#endif
/***************************************/

/************HYN_ESDCHECK_EN********************/
#if HYN_ESDCHECK_EN
extern void hyn_create_esd_sysfs(struct i2c_client *client);
extern void hyn_esd_switch(s32 on);
extern void hyn_init_esd_protect(void);
extern void hyn_esd_resume(void);
extern void hyn_esd_suspend(void);
extern void hyn_esd_init(void);
extern void hyn_esd_exit(void);
#endif
/***************************************/


/* ADB functions */
#if HYN_SYSFS_NODE_EN
extern int hyn_create_sysfs(struct i2c_client *client);
extern void hyn_release_sysfs(struct i2c_client *client);
#endif

#if ANDROID_TOOL_SURPORT
extern int hynitron_proc_fs_init(void);
extern void hynitron_proc_fs_exit(void);
#endif

extern int hyn_reset_proc(int hdelayms);

void hyn_irq_disable(void);
void hyn_irq_enable(void);

#if HYN_DEBUG_EN
#define HYN_DEBUG_LEVEL     1

#if (HYN_DEBUG_LEVEL == 2)
#define HYN_DEBUG(fmt, args...) printk(KERN_ERR "[HYN][%s]"fmt"\n", __func__, ##args)
#define HYN_INFO(fmt, args...)  printk(KERN_ERR "[HYN][Info][%s]"fmt"\n",__func__, ##args)
#define HYN_ERROR(fmt, args...) printk(KERN_ERR "[HYN][Error][%s]"fmt"\n",__func__, ##args)
#else
#define HYN_DEBUG(fmt, args...) printk(KERN_ERR "[HYN]"fmt"\n", ##args)
#define HYN_INFO(fmt, args...)  printk(KERN_ERR "[HYN][Info]"fmt"\n", ##args)
#define HYN_ERROR(fmt, args...) printk(KERN_ERR "[HYN][Error]"fmt"\n", ##args)
#endif

#define HYN_FUNC_ENTER() printk(KERN_ERR "[HYN]%s: Enter\n", __func__)
#define HYN_FUNC_EXIT()  printk(KERN_ERR "[HYN]%s: Exit(%d)\n", __func__, __LINE__)
#else
#define HYN_DEBUG(fmt, args...)
#define HYN_INFO(fmt, args...)
#define HYN_ERROR(fmt, args...)
#define HYN_FUNC_ENTER()
#define HYN_FUNC_EXIT()
#endif

#endif /*Hynitron_COMMON_H__*/

