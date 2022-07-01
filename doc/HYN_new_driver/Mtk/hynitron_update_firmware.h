#ifndef __HYNITRON_UPDATE_FIRMWARE_H__
#define __HYNITRON_UPDATE_FIRMWARE_H__

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
//#include <mach/irqs.h>

#include <linux/syscalls.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>

#include <linux/file.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>



int hyn_firmware_info(struct i2c_client * client);
int hyn_boot_update_fw(struct i2c_client * client);
int hyn_detect_main_iic(struct i2c_client *client);
int hyn_detect_bootloader(struct i2c_client *client);
void hyn_init_factory_test_init(struct i2c_client *client);
#if HYN_AUTO_FACTORY_TEST_EN
int hyn_factory_touch_test(void);
#endif
#if HYN_SYS_AUTO_SEARCH_FIRMWARE
int hyn_sys_auto_search_firmware(void);
#endif
#endif

