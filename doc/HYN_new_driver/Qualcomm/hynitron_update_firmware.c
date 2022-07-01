/*
 *Name        : hynintron_ex_fun.c
 *Author      : steven
 *Version     : V2.0
 *Create      : 2019-11-11
 *Copyright   : zxzz
 */

#include <linux/netdevice.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/namei.h>
#include <linux/dcache.h>
#include <linux/string.h>

#include "hynitron_core.h"
#include "hynitron_common.h"
#include "hynitron_update_firmware.h" 


#include "firmware/capacitive_hynitron_cst0xx_update.h"
#include "firmware/capacitive_hynitron_cst2xx_update.h"
#include "firmware/capacitive_hynitron_cst2xxse_update.h"
#include "firmware/capacitive_hynitron_cst3xx_update.h"
#include "firmware/capacitive_hynitron_cst3xxse_update.h"
#include "firmware/capacitive_hynitron_cst6xx_update.h"
#include "firmware/capacitive_hynitron_cst8xx_update.h"
#include "firmware/capacitive_hynitron_cst9xx_update.h"

//please config the chip series before using.
struct hynitron_fw_array hynitron_fw_grp[] = {
	//0-name; 1-fw; 2-project_id; 3-module_id; 4-chip_type; 5-fw_length;
	{ "capacitive_hynitron_cst0xx_update",   cst0xx_fw,   0x2843,0x01, CST016, (sizeof(cst0xx_fw))},	
	{ "capacitive_hynitron_cst2xx_update",   cst2xx_fw,   0x0501,0x01, CST226, (sizeof(cst2xx_fw))},
	{ "capacitive_hynitron_cst2xxse_update", cst2xxse_fw, 0x0501,0x01, CST226SE, (sizeof(cst2xxse_fw))},
	{ "capacitive_hynitron_cst3xx_update",   cst3xx_fw,   0x2117,0x11, CST348, (sizeof(cst3xx_fw))},
	{ "capacitive_hynitron_cst6xx_update",   cst6xx_fw,   0x2117,0x11, CST6928S, (sizeof(cst6xx_fw))},
	{ "capacitive_hynitron_cst3xxse_update", cst3xxse_fw, 0x0501,0x01, CST328SE, (sizeof(cst3xxse_fw))},
	{ "capacitive_hynitron_cst8xx_update",   cst8xx_fw,   0x0501,0x01, CST836, (sizeof(cst8xx_fw))},
  	{ "capacitive_hynitron_cst9xx_update",   cst9xx_fw,   0x2208,0x01, CST918, (sizeof(cst9xx_fw))},
  
};


struct hynitron_chip_type_array hynitron_chip_type_grp[] = {
	{CST016   ,HYN_CHIP_CST0XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},	
	{CST026   ,HYN_CHIP_CST0XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},
    {CST036   ,HYN_CHIP_CST0XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},
		
  	{CST126   ,HYN_CHIP_CST1XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST128   ,HYN_CHIP_CST1XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST128SE ,HYN_CHIP_CST1XXSE,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x5A},
	{CST140   ,HYN_CHIP_CST1XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST148   ,HYN_CHIP_CST1XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},

	{CST226   ,HYN_CHIP_CST2XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST226SE ,HYN_CHIP_CST2XXSE,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x5A},
	{CST237   ,HYN_CHIP_CST2XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST240   ,HYN_CHIP_CST2XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},

	{CST326   ,HYN_CHIP_CST1XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST328   ,HYN_CHIP_CST1XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST328SE ,HYN_CHIP_CST3XXSE,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x5A},
	{CST340   ,HYN_CHIP_CST3XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST348   ,HYN_CHIP_CST3XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST356   ,HYN_CHIP_CST3XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST6928S ,HYN_CHIP_CST6XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},

	{CST716   ,HYN_CHIP_CST7XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},	
	{CST726   ,HYN_CHIP_CST7XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},
    {CST736   ,HYN_CHIP_CST7XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},

	{CST816   ,HYN_CHIP_CST8XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},	
	{CST826   ,HYN_CHIP_CST8XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},
    {CST836   ,HYN_CHIP_CST8XX  ,  HYN_CHIP_PRODUCT_LINE_SEL_CAP, 0x15},

	{CST912   ,HYN_CHIP_CST9XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	{CST918   ,HYN_CHIP_CST9XX  ,  HYN_CHIP_PRODUCT_LINE_MUT_CAP, 0x1A},
	
};

extern struct hynitron_fw_array hynitron_fw_grp[];

static unsigned char update_fw_flag;
#if HYN_EN_AUTO_UPDATE


static int cst3xx_firmware_info(struct i2c_client * client);

#if HYN_EN_AUTO_UPDATE_CST78xx
static unsigned char dev_addr;
static unsigned char chip_sumok_flag;
/*
 *
 */
static int cst78xx_enter_bootmode(void){
     char retryCnt = 10;

     hyn_reset_proc(1);
     mdelay(5);
     while(retryCnt--){
         u8 cmd[3];
         cmd[0] = 0xAA;
         if (-1 == hyn_i2c_write_bytes(0xA001,cmd,1,REG_LEN_2B)){  // enter program mode
             mdelay(2); // 4ms
             continue;                   
         }
         if (-1 == hyn_i2c_read_bytes(0xA003,cmd,1,REG_LEN_2B)) { // read flag
             mdelay(2); // 4ms
             continue;                           
         }else{
             if (cmd[0] != 0x55){
                 msleep(2); // 4ms
                 continue;
             }else{
                 return 0;
             }
         }
     }
     return -1;
 }
 /*
  *
  */
static int cst78xx_update(u16 startAddr,u16 len,u8* src){
    u16 sum_len;
    u8 cmd[10];
	int ret;

	ret = 0;
	HYN_FUNC_ENTER();
    if (cst78xx_enter_bootmode() == -1){
       return -1;
    }
    sum_len = 0;
	
 #define PER_LEN	512
    do{
        if (sum_len >= len){
            return -1;
        }
        
        // send address
        cmd[1] = startAddr>>8;
        cmd[0] = startAddr&0xFF;
        hyn_i2c_write_bytes(0xA014,cmd,2,REG_LEN_2B);
 
#if HYN_IIC_TRANSFER_LIMIT
	{       
		u8 temp_buf[8];
		u16 j,iic_addr;
		iic_addr=0;
		for(j=0; j<128; j++){
			
	    	temp_buf[0] = *((u8*)src+iic_addr+0);
	    	temp_buf[1] = *((u8*)src+iic_addr+1);
			temp_buf[2] = *((u8*)src+iic_addr+2);
			temp_buf[3] = *((u8*)src+iic_addr+3);

	    	hyn_i2c_write_bytes((0xA018+iic_addr),(u8* )temp_buf,4,REG_LEN_2B);
			iic_addr+=4;
			if(iic_addr==512) break;
		}

	}
#else
		hyn_i2c_write_bytes(0xA018,src,PER_LEN,REG_LEN_2B);
#endif 	
        cmd[0] = 0xEE;
        hyn_i2c_write_bytes(0xA004,cmd,1,REG_LEN_2B);
		
		if(hyn_ts_data->apk_upgrade_flag==0)
			msleep(300);
		else
			msleep(100);
		
        {
            u8 retrycnt = 50;
            while(retrycnt--){
                cmd[0] = 0;
                hyn_i2c_read_bytes(0xA005,cmd,1,REG_LEN_2B);
                if (cmd[0] == 0x55){
                    // success 
                    break;
                }
                msleep(10);
            }

			if(cmd[0]!=0x55)
			{
				ret = -1;
			}
        }
        startAddr += PER_LEN;
        src       += PER_LEN;
        sum_len   += PER_LEN;
    }while(len);
    
    // exit program mode
    cmd[0] = 0x00;
    hyn_i2c_write_bytes(0xA003,cmd,1,REG_LEN_2B);
	HYN_FUNC_EXIT();
	return ret;
}
/*
 *
 */
static u32 cst78xx_read_checksum(u16 startAddr,u16 len){
    union{
        u32 sum;
        u8 buf[4];
    }checksum;
    char cmd[3];
    char readback[4] = {0};

    if (cst78xx_enter_bootmode() == -1){
       return -1;
    }
    
    cmd[0] = 0;
    if (-1 == hyn_i2c_write_bytes(0xA003,cmd,1,REG_LEN_2B)){
        return -1;
    }
    msleep(500);
    
    if (-1 == hyn_i2c_read_bytes(0xA000,readback,1,REG_LEN_2B)){
        return -1;   
    }
    if (readback[0] != 1){
        return -1;
    }
    if (-1 == hyn_i2c_read_bytes(0xA008,checksum.buf,4,REG_LEN_2B)){
        return -1;
    }
	chip_sumok_flag  = 1;
	
    return checksum.sum;
}


int cst8xx_boot_update_fw(struct i2c_client *mclient){
    unsigned short startAddr;
    unsigned short length;
    unsigned short checksum; 
	unsigned short chipchecksum;

	HYN_FUNC_ENTER();
	update_fw_flag  = 1;	
	chip_sumok_flag = 0;
	
    hyn_ts_data->client = mclient;


	if(hyn_ts_data->apk_upgrade_flag==0)
	cst8xx_firmware_info(mclient);
	
    dev_addr  = hyn_ts_data->client->addr;

    hyn_ts_data->client->addr = 0x6A;

    if (cst78xx_enter_bootmode() == 0){ 
		
        if((hyn_ts_data->fw_length) > 10){
			
            startAddr = *(hyn_ts_data->p_hynitron_upgrade_firmware+1);
			
            length =*(hyn_ts_data->p_hynitron_upgrade_firmware+3);
			
            checksum = *(hyn_ts_data->p_hynitron_upgrade_firmware+5);
			
            startAddr <<= 8; 
			startAddr |= *(hyn_ts_data->p_hynitron_upgrade_firmware+0);

            length <<= 8; 
			length |= *(hyn_ts_data->p_hynitron_upgrade_firmware+2);
			
            checksum <<= 8; 
			checksum |= *(hyn_ts_data->p_hynitron_upgrade_firmware+4);
			
			chipchecksum = cst78xx_read_checksum(startAddr, length);
			
			if(update_fw_flag||chip_sumok_flag==0)
			{	
				HYN_INFO("\r\ncst8xx_boot_update_fw:  low version,  updating!!!r\n");
				HYN_INFO("\r\n CTP cst78xx File, start-0x%04x len-0x%04x fileCheck-0x%04x\r\n",startAddr,length,checksum);
				if(chipchecksum != checksum){				
					cst78xx_update(startAddr, length, (hyn_ts_data->p_hynitron_upgrade_firmware+6));
					length = cst78xx_read_checksum(startAddr, length);
					HYN_INFO("\r\nCTP cst78xx update %s, checksum-0x%04x",((length==checksum) ? "success" : "fail"),length);
					
					
				}else{
					HYN_INFO("\r\nCTP cst78xx check pass...");
				}
			}
			else
			{
				HYN_INFO("\r\ncst8xx_boot_update_fw:  high version  not update!!!r\n");
			}
        }
        goto re;
    }
	else
	{
		hyn_ts_data->client->addr = dev_addr;
		return -1;
	}
    HYN_FUNC_EXIT();
re:
    hyn_ts_data->client->addr = dev_addr;
    hyn_reset_proc(10);
    return 0;
}


#endif

#if HYN_EN_AUTO_UPDATE_CST2xx

#define  CST2XX_BASE_ADDR        (24 * 1024)
#define  CST2XX_FW_BIN_SIZE       (6*1024)
static int cst2xx_enter_download_mode (void)
{
    int ret;
    unsigned char buf[2];

    buf[0] = 0xD1;
    buf[1] = 0x11;
    ret = cst3xx_i2c_write (hyn_ts_data->client, buf, 2);
    if (ret < 0) {
         return -1;
    }

    mdelay (1); //wait enter download mode

    buf[0] = 0xD0;
    buf[1] = 0x01;
    ret = cst3xx_i2c_read_register (hyn_ts_data->client, buf, 1);
    if (ret < 0) {
         return -1;
    }
    if (buf[0] != 0x55) {
        HYN_DEBUG ("[HYN]reciev 0x55 failed.\r\n");
        return -1;
    }

    buf[0] = 0xD1;
    buf[1] = 0x10;   //enter writer register mode
    ret = cst3xx_i2c_write (hyn_ts_data->client, buf, 2);
    if (ret < 0) {
        HYN_DEBUG ("[HYN]Send cmd 0xD110 failed. \r\n");
        return -1;
    }

    return 0;
}

static int cst2xx_download_program (unsigned char* data, int len)
{
    int ret;
    int i, j;
    unsigned int wr_addr;
    unsigned char* pData;
    unsigned char* pSrc;
    unsigned char* pDst;
    unsigned char i2c_buf[8];

    pData = kmalloc (sizeof (unsigned char) * (1024 + 4), GFP_KERNEL);
    if (NULL == pData) {
        HYN_ERROR ("malloc data buffer failed.\n");
        return -1;
    }

    pSrc = data;
    HYN_DEBUG ("write program data begain:0x%x.\n", len);

    for (i = 0; i < (len / 1024); i++) {
        wr_addr  = (i << 10) + CST2XX_BASE_ADDR;
        pData[0] = (wr_addr >> 24) & 0xFF;
        pData[1] = (wr_addr >> 16) & 0xFF;
        pData[2] = (wr_addr >> 8) & 0xFF;
        pData[3] =  wr_addr & 0xFF;
        pDst = pData + 4;

        for (j = 0; j < 256; j++) {
            *pDst = * (pSrc + 3);
            * (pDst + 1) = * (pSrc + 2);
            * (pDst + 2) = * (pSrc + 1);
            * (pDst + 3) = *pSrc;
            pDst += 4;
            pSrc += 4;
        }

#if (!HYN_IIC_TRANSFER_LIMIT)
        for (j = 0; j < 256; j++) {
            i2c_buf[0] = (wr_addr >> 24) & 0xFF;
            i2c_buf[1] = (wr_addr >> 16) & 0xFF;
            i2c_buf[2] = (wr_addr >> 8) & 0xFF;
            i2c_buf[3] = wr_addr & 0xFF;
            i2c_buf[4] = pData[j * 4 + 4 + 0];
            i2c_buf[5] = pData[j * 4 + 4 + 1];
            i2c_buf[6] = pData[j * 4 + 4 + 2];
            i2c_buf[7] = pData[j * 4 + 4 + 3];
            ret = cst3xx_i2c_write (hyn_ts_data->client, i2c_buf, 8);
            if (ret < 0) {
                HYN_ERROR ("program failed.\n");
                goto ERR_OUT;
            }
            wr_addr += 4;
        }
#else
        ret = cst3xx_i2c_write (hyn_ts_data->client, pData, 1024 + 4);
        if (ret < 0) {
            HYN_ERROR ("program failed.\n");
            goto ERR_OUT;
        }

#endif
    }

    //clear update key
    pData[3] = 0x20000FFC & 0xFF;
    pData[2] = (0x20000FFC >> 8)  & 0xFF;
    pData[1] = (0x20000FFC >> 16) & 0xFF;
    pData[0] = (0x20000FFC >> 24) & 0xFF;
    pData[4] = 0x00;
    pData[5] = 0x00;
    pData[6] = 0x00;
    pData[7] = 0x00;
    ret = cst3xx_i2c_write (hyn_ts_data->client, pData, 8);
    if (ret < 0) {
        HYN_ERROR ("clear update key failed.\n");
        goto ERR_OUT;
    }

    pData[3] = 0xD013D013 & 0xFF;
    pData[2] = (0xD013D013 >> 8)  & 0xFF;
    pData[1] = (0xD013D013 >> 16) & 0xFF;
    pData[0] = (0xD013D013 >> 24) & 0xFF;
    ret = cst3xx_i2c_write (hyn_ts_data->client, pData, 4);
    if (ret < 0) {
        HYN_ERROR ("exit register read/write mode failed.\n");
        goto ERR_OUT;
    }

    HYN_DEBUG("--------write program data end--------.\r\n");

    if (pData != NULL) {
        kfree (pData);
        pData = NULL;
    }

    return 0;

ERR_OUT:
    if (pData != NULL) {
        kfree (pData);
        pData = NULL;
    }

    return -1;
}

static int cst2xx_read_checksum (void)
{
    int ret;
    int i;
    unsigned int checksum;
    unsigned int bin_checksum;
    unsigned char buf[4];
    unsigned char* pData;

    for (i = 0; i < 10; i++) {
        buf[0] = 0xD0;
        buf[1] = 0x00;
        ret = cst3xx_i2c_read_register (hyn_ts_data->client, buf, 1);
        if (ret < 0) {
            mdelay (2);
            continue;
        }

        if ((buf[0] == 0x01) || (buf[0] == 0x02))
            break;

        mdelay (2);
    }

    if ((buf[0] == 0x01) || (buf[0] == 0x02)) {
        buf[0] = 0xD0;
        buf[1] = 0x08;
        ret = cst3xx_i2c_read_register (hyn_ts_data->client, buf, 4);
        if (ret < 0)
            return -1;

        //handle read data  --> checksum
        checksum = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
        pData = hyn_ts_data->p_hynitron_upgrade_firmware+ 6160; //6*1024+16
        bin_checksum = pData[0] + (pData[1] << 8) + (pData[2] << 16) + (pData[3] << 24);
        if (checksum != bin_checksum)
            HYN_DEBUG ("Check sum error.\n");

        HYN_DEBUG ("checksum ic:0x%x. bin:0x%x-----\n", checksum, bin_checksum);

        buf[0] = 0xD0;
        buf[1] = 0x01;
        buf[2] = 0xA5;
        ret = cst3xx_i2c_write (hyn_ts_data->client, buf, 3);
        if (ret < 0)
            return -1;
    } else {
        HYN_DEBUG ("No checksum.\n");
        return -1;
    }

    return 0;
}

static int cst2xx_update_firmware (struct i2c_client* client, unsigned char* pdata, int data_len)
{
    int ret;
    int retry;
    retry = 0;
    HYN_DEBUG ("enter the update firmware.\n");
start_flow:

	client->addr = hyn_ts_data->chip_ic_main_addr;
	hyn_reset_proc(5+retry);
	client->addr = hyn_ts_data->chip_ic_bootloader_addr;
	hyn_ts_data->fw_updating=1;
    ret = cst2xx_enter_download_mode();
    if (ret < 0) {
        HYN_ERROR ("enter download mode failed.\n");
        goto fail_retry;
    }
	hyn_ts_data->fw_updating=2;
    ret = cst2xx_download_program (pdata, data_len);
    if (ret < 0) {
        HYN_ERROR ("download program failed.\n");
        goto fail_retry;
    }

    mdelay (3);
	hyn_ts_data->fw_updating=3;
    ret = cst2xx_read_checksum();
    if (ret < 0) {
        HYN_ERROR ("checksum failed.\n");
        goto fail_retry;
    }
	client->addr = hyn_ts_data->chip_ic_main_addr;

    HYN_DEBUG ("Download firmware succesfully.\n");

    return 0;

fail_retry:
    if (retry < 20) {
        retry++;
		mdelay (20);
        goto start_flow;
    }else{
    	client->addr = hyn_ts_data->chip_ic_main_addr;
	}
    return -1;
}

static int cst2xx_boot_update_fw (struct i2c_client* client)
{
    return cst2xx_update_firmware (client, hyn_ts_data->p_hynitron_upgrade_firmware, CST2XX_FW_BIN_SIZE);
}

#endif

#if (HYN_EN_AUTO_UPDATE_CST3xxSE||HYN_EN_AUTO_UPDATE_CST2xxSE||HYN_EN_AUTO_UPDATE_CST1xxSE)

#define  CST3XXSE_BASE_ADDR		(0x00000000)
#define  CST3XXSE_FW_BIN_SIZE    (7*1024+512)

static int cst3xxse_enter_download_mode(void)
{
	int ret;
	unsigned char buf[4];
	
	buf[0] = 0xA0;
	buf[1] = 0x01;
	buf[2] = 0xAA;
	ret = cst3xx_i2c_write(hyn_ts_data->client, buf, 3);
	if (ret < 0){
		return -1;
	}	
	mdelay(5); //wait enter download mode
	
	buf[0] = 0xA0;
	buf[1] = 0x03; //check whether into program mode
	ret = cst3xx_i2c_read_register(hyn_ts_data->client, buf, 1);
	if(ret < 0){
		return -1;	
	}	
	if(buf[0] != 0x55){
		HYN_ERROR("hyn reciev 0x55 failed.\n");
		return -1;
	}

	return 0;
}

static int cst3xxse_download_program(unsigned char *pdata, int len)
{	
	int i, ret, j,retry;
	unsigned char *i2c_buf;
	unsigned char temp_buf[8];
	unsigned short eep_addr, iic_addr;
    int total_kbyte;

	i2c_buf = kmalloc(sizeof(unsigned char)*(512 + 2), GFP_KERNEL);
	if (i2c_buf == NULL) 
	{
		return -1;
	}
	
	//make sure fwbin len is N*1K

	total_kbyte = len / 512;
	

	for (i=0; i<total_kbyte; i++) {
		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x14;
		eep_addr = i << 9;		//i * 512
		i2c_buf[2] = eep_addr;
		i2c_buf[3] = eep_addr>>8;
		ret = cst3xx_i2c_write(hyn_ts_data->client, i2c_buf, 4);
		if (ret < 0)
			goto error_out;

        #if (!HYN_IIC_TRANSFER_LIMIT)
		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x18;
		memcpy(i2c_buf + 2, pdata + eep_addr, 512);
		ret = cst3xx_i2c_write(hyn_ts_data->client, i2c_buf, 514);
		if (ret < 0)
			goto error_out;
		#else
		
		memcpy(i2c_buf, pdata + eep_addr, 512);
		for(j=0; j<128; j++) {
			iic_addr = (j<<2);
    	    temp_buf[0] = (iic_addr+0xA018)>>8;
    	    temp_buf[1] = (iic_addr+0xA018)&0xFF;
			temp_buf[2] = i2c_buf[iic_addr+0];
			temp_buf[3] = i2c_buf[iic_addr+1];
			temp_buf[4] = i2c_buf[iic_addr+2];
			temp_buf[5] = i2c_buf[iic_addr+3];
    		ret = cst3xx_i2c_write(hyn_ts_data->client, temp_buf, 6);
    		if (ret < 0)
    			goto error_out;		
		}
    	#endif
		
		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x04;
		i2c_buf[2] = 0xEE;
		ret = cst3xx_i2c_write(hyn_ts_data->client, i2c_buf, 3);
		if (ret < 0)
			goto error_out;
		
		mdelay(100);	 // 60

        for(retry=0;retry<10;retry++)
        {
            i2c_buf[0] = 0xA0;
			i2c_buf[1] = 0x05;
			ret = cst3xx_i2c_read_register(hyn_ts_data->client, i2c_buf, 1);	
			
			if (ret < 0){
				mdelay(100);
				continue;
			}    
			else
			{
				if (i2c_buf[0] != 0x55)          
	            {
					mdelay(100);	 
					continue;
				}
				else
				{
	 				break;
				}


			}
		    

		}
		if(retry==10){
			
			goto error_out;
		}
        
	}

	i2c_buf[0] = 0xA0;
	i2c_buf[1] = 0x01;
	i2c_buf[2] = 0x00;
	ret = cst3xx_i2c_write(hyn_ts_data->client, i2c_buf, 3);
	if (ret < 0)
		goto error_out;
	
	i2c_buf[0] = 0xA0;
	i2c_buf[1] = 0x03;
	i2c_buf[2] = 0x00;
	ret = cst3xx_i2c_write(hyn_ts_data->client, i2c_buf, 3);
	if (ret < 0)
		goto error_out;	
	
	if (i2c_buf != NULL) {
		kfree(i2c_buf);
		i2c_buf = NULL;
	}

	return 0;
	
error_out:
	if (i2c_buf != NULL) {
		kfree(i2c_buf);
		i2c_buf = NULL;
	}
	return -1;	
}

static int cst3xxse_read_checksum(unsigned char *p_fw)
{
	int ret;
	int i;
	unsigned int  checksum;
	unsigned int  bin_checksum;
	unsigned char buf[4];
	const unsigned char *pData;

	for(i=0; i<10; i++)
	{
		buf[0] = 0xA0;
		buf[1] = 0x00;
		ret = cst3xx_i2c_read_register(hyn_ts_data->client, buf, 1);
		if(ret < 0)
		{
			mdelay(2);
			continue;
		}

		if(buf[0]!=0)
			break;
		else
		mdelay(2);
	}
    mdelay(4);
	
	if(buf[0]==0x01)
	{
		buf[0] = 0xA0;
		buf[1] = 0x08;
		ret = cst3xx_i2c_read_register(hyn_ts_data->client, buf, 4);
		
		if(ret < 0)	return -1;
		checksum = buf[0] + (buf[1]<<8) + (buf[2]<<16) + (buf[3]<<24);
        pData=(unsigned char  *)p_fw +7680-4;   //7*1024 +512
		bin_checksum = pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);

        HYN_INFO("hyn checksum ic:0x%x. bin:0x%x------\n", checksum, bin_checksum);  
        if(checksum!=bin_checksum)
		{
			 HYN_ERROR("hyn check sum error.\n");		
			return -1;
			
		}
		
	}
	else
	{
		HYN_ERROR("hyn No checksum. buf[0]:%d.\n", buf[0]);
		return -1;
	}
	
	return 0;
}



static int cst3xxse_update_firmware(struct i2c_client * client,unsigned char *pdata)
{
	int ret;
	int retry;
	unsigned char buf[4];

	retry = 0;
	HYN_INFO("hyn cst3xxse_update_firmware enter.\n");	
	mdelay(20);
	
start_flow:
	
	client->addr = hyn_ts_data->chip_ic_main_addr;
	hyn_reset_proc(5+retry);
	client->addr = hyn_ts_data->chip_ic_bootloader_addr;
	hyn_ts_data->fw_updating=1;
	ret = cst3xxse_enter_download_mode();
	if (ret < 0)
	{
		 HYN_ERROR("hyn cst3xxse_enter_download_mode enter failed.\n");
		goto fail_retry;
	}
	
	hyn_ts_data->fw_updating=2;
	ret = cst3xxse_download_program(pdata, CST3XXSE_FW_BIN_SIZE);
	if (ret < 0)
	{
		 HYN_ERROR("hyn cst3xxse_download_program failed.\n");
		goto fail_retry;
	}

	mdelay(5);
	hyn_ts_data->fw_updating=3;	
	ret = cst3xxse_read_checksum(pdata);
	if (ret < 0)
	{
		 HYN_ERROR("hyn cst3xxse_read_checksum failed.\n");
		goto fail_retry;
	}
	else
	{
		buf[0] = 0xA0;  //exit program
		buf[1] = 0x06;
		buf[2] = 0xEE;
		ret = cst3xx_i2c_write(client, buf, 3);
		
		//if(ret < 0) 	
		//	goto fail_retry;;
		
	}	

	HYN_INFO("hyn cst3xxse_update_firmware succesfully.\n");

	mdelay(10);
	client->addr = hyn_ts_data->chip_ic_main_addr;

	hyn_reset_proc(20);	

	return 0;
	
fail_retry:
	if (retry < 30)
	{
		retry++;
		mdelay(20);
		goto start_flow;
	}else{
	client->addr = hyn_ts_data->chip_ic_main_addr;
	}
	
	return -1;
}
static int cst3xxse_update_judge(unsigned char *fw , int strict)
{
	unsigned int  bin_checksum,bin_version;
	unsigned char *pData;
	unsigned int *pData_check;
	int i;

	bin_checksum = 0x55;
	pData_check = (unsigned int *)fw;
	for (i=0; i<(CST3XXSE_FW_BIN_SIZE-4); i+=4) {
		bin_checksum += (*pData_check);
		pData_check++;
	}
	
	if (bin_checksum != (*pData_check)) {
		HYN_ERROR("calculated checksum error:0x%x not equal 0x%x.\n", bin_checksum, *pData_check);
		return -1;	//bad fw, so do not update
	}	
	 //checksum
     	
	pData=(unsigned char  *)fw +7680-4;   //7*1024 +512
	bin_checksum = pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);




	if (strict > 0) {

			if(hyn_ts_data->chip_ic_checksum!=bin_checksum)
			{
				HYN_ERROR("hyn checksum is different******bin_checksum:0x%x, g_cst3xx_ic_checksum:0x%x. \r\n",bin_checksum,hyn_ts_data->chip_ic_checksum);	
#if HYN_UPDATE_FIRMWARE_FORCE
	    	   HYN_INFO("update firmware online force.\n");
	    	   return 0;
#endif

				if(hyn_ts_data->chip_ic_checkcode ==0xffffffff){
					HYN_ERROR("chip ic have no firmware,start update config firmware.\n");
					return 0;
				}else if((hyn_ts_data->chip_ic_type!=ic_type)
					||(hyn_ts_data->chip_ic_project_id!=project_id)){
		
					HYN_ERROR("chip_ic_type:0x%04x,chip_ic_project_id:0x%04x,not match.\n",hyn_ts_data->chip_ic_type, hyn_ts_data->chip_ic_project_id );
					return -1;
				} 
			     //chip version
				pData=(unsigned char  *)fw +7680-8;   //7*1024 +512
				bin_version = pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);

				HYN_INFO("hyn bin_version is different******bin_version:0x%x, fw_version:0x%x. \r\n",bin_version,hyn_ts_data->chip_ic_fw_version);	

				mdelay(5);		

				if(bin_version<hyn_ts_data->chip_ic_fw_version)
				{	
					HYN_ERROR("hyn g_cst3xx_ic_version is higher ,no need to update firmware.\n");
					return -1;
				}	
			}else{
					HYN_ERROR("bin_checksum(0x%x), g_cst3xx_ic_checksum(0x%x).\n",bin_checksum, hyn_ts_data->chip_ic_checksum);
					return -1;
			}
	}
	return 0;	
	
		
}
static int cst3xxse_boot_update_fw(struct i2c_client * client)
{
	int ret=-1;

	if(hyn_ts_data->apk_upgrade_flag==0){
		ret = cst3xxse_update_judge(hyn_ts_data->p_hynitron_upgrade_firmware,1);
		if (ret < 0) {
			HYN_ERROR("cst3xxse_update_judge fail ,apk_upgrade_flag = %d.\n",hyn_ts_data->apk_upgrade_flag);
			return 0;
		}
	}else {
		ret = cst3xxse_update_judge(hyn_ts_data->p_hynitron_upgrade_firmware,0);
		if (ret < 0) {
			HYN_ERROR("cst3xxse_update_judge fail ,apk_upgrade_flag = %d.\n",hyn_ts_data->apk_upgrade_flag);
			return 0;
		}	
	}
	return cst3xxse_update_firmware(client, hyn_ts_data->p_hynitron_upgrade_firmware);
}

#endif

#if (HYN_EN_AUTO_UPDATE_CST6xx)			

/*******************************************************
Function:
    read checksum in bootloader mode
Input:
    client: i2c client
    strict: check checksum value
Output:
    success: 0
    fail:	-1
*******************************************************/

#define CST6XX_BIN_SIZE    (24*1024 + 28)

static int cst6xx_check_checksum(struct i2c_client * client)
{
	int ret;
	int i;
	unsigned int  checksum;
	unsigned int  bin_checksum;
	unsigned char buf[4];
	const unsigned char *pData;

	for(i=0; i<5; i++)
	{
		buf[0] = 0xA0;
		buf[1] = 0x00;
		ret = cst3xx_i2c_read_register(client, buf, 1);
		if(ret < 0)
		{
			mdelay(2);
			continue;
		}

		if(buf[0]!=0)
			break;
		else
		mdelay(2);
	}
    mdelay(2);


    if(buf[0]==0x01)
	{
		buf[0] = 0xA0;
		buf[1] = 0x08;
		ret = cst3xx_i2c_read_register(client, buf, 4);

		if(ret < 0)	return -1;

		// read chip checksum
		checksum = buf[0] + (buf[1]<<8) + (buf[2]<<16) + (buf[3]<<24);

        pData=(unsigned char  *)hyn_ts_data->p_hynitron_upgrade_firmware +24*1024+16+4;   //7*1024 +512
		bin_checksum = pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);

        HYN_INFO(" the updated ic checksum is :0x%x. the updating firmware checksum is:0x%x------\n", checksum, bin_checksum);

        if(checksum!=bin_checksum)
		{
			HYN_ERROR("hyn check sum error.\n");		
			return -1;

		}

	}
	else
	{
		HYN_INFO("hyn No checksum.\n");
		return -1;
	}
	return 0;
}
static int cst6xx_into_program_mode(struct i2c_client * client)
{
	int ret;
	unsigned char buf[4];

	buf[0] = 0xA0;
	buf[1] = 0x01;
	buf[2] = 0xAB;	//set cmd to enter program mode
	ret = cst3xx_i2c_write(client, buf, 3);
	if (ret < 0)  return -1;
	mdelay(2);

	buf[0] = 0xA0;
	buf[1] = 0x02;	//check whether into program mode
	ret = cst3xx_i2c_read_register(client, buf, 1);
	if (ret < 0)  return -1;

	if (buf[0] != 0x55) return -1;
	
	return 0;
}

static int cst6xx_exit_program_mode(struct i2c_client * client)
{
	int ret;
	unsigned char buf[3];

	buf[0] = 0xA0;
	buf[1] = 0x06;
	buf[2] = 0xEE;
	ret = cst3xx_i2c_write(client, buf, 3);
	if (ret < 0)
		return -1;

	mdelay(10);	//wait for restart

	return 0;
}

static int cst6xx_erase_program_area(struct i2c_client * client)
{
	int ret;
	unsigned char buf[3];

	buf[0] = 0xA0;
	buf[1] = 0x02;
	buf[2] = 0x00;		//set cmd to erase main area
	ret = cst3xx_i2c_write(client, buf, 3);
	if (ret < 0) return -1;

	mdelay(5);

	buf[0] = 0xA0;
	buf[1] = 0x03;
	ret = cst3xx_i2c_read_register(client, buf, 1);
	if (ret < 0)  return -1;

	if (buf[0] != 0x55) return -1;

	return 0;
}

static int cst6xx_write_program_data(struct i2c_client * client,
		const unsigned char *pdata)
{
	int i, ret;
	unsigned char *i2c_buf;
	unsigned short eep_addr;
	int total_kbyte;
#if HYN_IIC_TRANSFER_LIMIT
	unsigned char temp_buf[8];
	unsigned short iic_addr;
	int  j;

#endif

	i2c_buf = kmalloc(sizeof(unsigned char)*(1024 + 2), GFP_KERNEL);
	if (i2c_buf == NULL)
		return -1;

	//make sure fwbin len is N*1K
	//total_kbyte = len / 1024;
	total_kbyte = 24;
	for (i=0; i<total_kbyte; i++) {
		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x14;
		eep_addr = i << 10;		//i * 1024
		i2c_buf[2] = eep_addr;
		i2c_buf[3] = eep_addr>>8;
		ret = cst3xx_i2c_write(client, i2c_buf, 4);
		if (ret < 0)
			goto error_out;

#if HYN_IIC_TRANSFER_LIMIT
		memcpy(i2c_buf, pdata + eep_addr, 1024);
		for(j=0; j<256; j++) {
			iic_addr = (j<<2);
    	temp_buf[0] = (iic_addr+0xA018)>>8;
    	temp_buf[1] = (iic_addr+0xA018)&0xFF;
		temp_buf[2] = i2c_buf[iic_addr+0];
		temp_buf[3] = i2c_buf[iic_addr+1];
		temp_buf[4] = i2c_buf[iic_addr+2];
		temp_buf[5] = i2c_buf[iic_addr+3];
    	ret = cst3xx_i2c_write(client, temp_buf, 6);
    		if (ret < 0)
    			goto error_out;
		}
#else

		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x18;
		memcpy(i2c_buf + 2, pdata + eep_addr, 1024);
		ret = cst3xx_i2c_write(client, i2c_buf, 1026);
		if (ret < 0)
			goto error_out;
#endif

		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x04;
		i2c_buf[2] = 0xEE;
		ret = cst3xx_i2c_write(client, i2c_buf, 3);
		if (ret < 0)
			goto error_out;

		mdelay(60);

		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x05;
		ret = cst3xx_i2c_read_register(client, i2c_buf, 1);
		if (ret < 0)
			goto error_out;

		if (i2c_buf[0] != 0x55)
			goto error_out;

	}

	i2c_buf[0] = 0xA0;
	i2c_buf[1] = 0x03;
	i2c_buf[2] = 0x00;
	ret = cst3xx_i2c_write(client, i2c_buf, 3);
	if (ret < 0)
		goto error_out;

	mdelay(8);

	if (i2c_buf != NULL) {
		kfree(i2c_buf);
		i2c_buf = NULL;
	}

	return 0;

error_out:
	if (i2c_buf != NULL) {
		kfree(i2c_buf);
		i2c_buf = NULL;
	}
	return -1;
}

static int cst6xx_update_firmware(struct i2c_client * client, const unsigned char *pdata)
{
	int ret;
	int retry_timer = 0;
	int retry = 0;

	HYN_INFO("----------upgrade cst3xx begain------------\n");
	mdelay(20);
		
START_FLOW:
	hyn_reset_proc(5+retry_timer);
	client->addr = hyn_ts_data->chip_ic_bootloader_addr;
	hyn_ts_data->fw_updating=1;
	ret = cst6xx_into_program_mode(client);
	if (ret < 0) {
		HYN_ERROR("into program mode failed.\n");
		if(retry_timer<20) retry_timer++;
 		goto err_out;
	}
	hyn_ts_data->fw_updating=2;
	ret = cst6xx_erase_program_area(client);
	if (ret<0) {
		HYN_ERROR("erase main area failed.\n");
		goto err_out;
	}
	hyn_ts_data->fw_updating=3;
	ret = cst6xx_write_program_data(client, pdata);
	if (ret < 0) {
		HYN_ERROR("write program data into cstxxx failed.\n");
		goto err_out;
	}
	hyn_ts_data->fw_updating=4;
    ret =cst6xx_check_checksum(client);
	if (ret < 0) {
		HYN_ERROR("after write program cst6xx_check_checksum failed.\n");
		goto err_out;
	}
	hyn_ts_data->fw_updating=5;
	ret = cst6xx_exit_program_mode(client);
	if (ret < 0) {
		HYN_ERROR("exit program mode failed.\n");
		//goto err_out;
	}
	client->addr = hyn_ts_data->chip_ic_main_addr;
	hyn_reset_proc(30);
	
	hyn_ts_data->fw_updating=6;
	HYN_INFO(" hyn----------cst3xx_update_firmware  end------------\n");

	return 0;

err_out:
	if (retry < 30) {
		retry++;
		mdelay(30);
		goto START_FLOW;
	}
	else {	
		client->addr = hyn_ts_data->chip_ic_main_addr;
		return -1;
	}
}

static int cst6xx_update_judge( unsigned char *pdata, int strict)
{
	unsigned short ic_type, project_id;
	unsigned int fw_checksum, fw_version;
	const unsigned int *p;
	int i;
	unsigned char *pBuf;

	fw_checksum = 0x55;
	p = (const unsigned int *)pdata;
	for (i=0; i<(CST6XX_BIN_SIZE-4); i+=4) {
		fw_checksum += (*p);
		p++;
	}

	if (fw_checksum != (*p)) {
		HYN_ERROR("calculated checksum error:0x%x not equal 0x%x.\n", fw_checksum, *p);
		return -1;	//bad fw, so do not update
	}

	pBuf = &pdata[CST6XX_BIN_SIZE-16];

	project_id = pBuf[1];
	project_id <<= 8;
	project_id |= pBuf[0];

	ic_type = pBuf[3];
	ic_type <<= 8;
	ic_type |= pBuf[2];

	fw_version = pBuf[7];
	fw_version <<= 8;
	fw_version |= pBuf[6];
	fw_version <<= 8;
	fw_version |= pBuf[5];
	fw_version <<= 8;
	fw_version |= pBuf[4];

	fw_checksum = pBuf[11];
	fw_checksum <<= 8;
	fw_checksum |= pBuf[10];
	fw_checksum <<= 8;
	fw_checksum |= pBuf[9];
	fw_checksum <<= 8;
	fw_checksum |= pBuf[8];

	HYN_INFO("the updating firmware:project_id:0x%04x,ic type:0x%04x,fw_version:0x%x,checksum:0x%x\n",
			project_id, ic_type, fw_version, fw_checksum);

#if HYN_UPDATE_FIRMWARE_FORCE
    HYN_INFO("update firmware online force.\n");
    return 0;
#endif

	if(hyn_ts_data->chip_ic_checkcode ==0xffffffff){
		HYN_ERROR("chip ic have no firmware,start update config firmware.\n");
		return 0;
	}else if((hyn_ts_data->chip_ic_type!=ic_type)
		||(hyn_ts_data->chip_ic_project_id!=project_id)){
		
		HYN_ERROR("chip_ic_type:0x%04x,chip_ic_project_id:0x%04x,not match.\n",hyn_ts_data->chip_ic_type, hyn_ts_data->chip_ic_project_id );
		return -1;
	} 
	
	if (strict > 0) {
		if (hyn_ts_data->chip_ic_checksum != fw_checksum){
			
			if (hyn_ts_data->chip_ic_fw_version >fw_version){
				HYN_ERROR("fw version(%d), ic version(%d).\n",fw_version, hyn_ts_data->chip_ic_fw_version );
				return -1;
			}
		}else{
			HYN_ERROR("fw checksum(0x%x), ic checksum(0x%x).\n",fw_checksum, hyn_ts_data->chip_ic_checksum );
			return -1;
		}

	}

	return 0;
}

static int cst6xx_boot_update_fw(struct i2c_client *client)
{
	int ret;
	
	HYN_FUNC_ENTER();

	if(hyn_ts_data->apk_upgrade_flag==0)
		ret = cst6xx_update_judge(hyn_ts_data->p_hynitron_upgrade_firmware, 1);
	else
		ret = cst6xx_update_judge(hyn_ts_data->p_hynitron_upgrade_firmware, 0);	
	if (ret < 0) {
		HYN_ERROR(" update_judge fail,no need to update firmware.\n");
		return 0;
	}
	
	ret = cst6xx_update_firmware(client, hyn_ts_data->p_hynitron_upgrade_firmware);
	if (ret < 0){
		HYN_ERROR("update firmware failed,fw_updating:%d .\n",hyn_ts_data->fw_updating);
		return -1;
	}
	hyn_ts_data->fw_updating=0;

    mdelay(50);

	ret = cst3xx_firmware_info(client);
	if (ret < 0) {
		HYN_ERROR(" after update read version and checksum fail.\n");
		return -1;
	}

	HYN_FUNC_EXIT();
	return 0;
}
#endif


#if (HYN_EN_AUTO_UPDATE_CST3xx||HYN_EN_AUTO_UPDATE_CST1xx||HYN_EN_AUTO_UPDATE_CST9xx)			

/*******************************************************
Function:
    read checksum in bootloader mode
Input:
    client: i2c client
    strict: check checksum value
Output:
    success: 0
    fail:	-1
*******************************************************/

#define CST3XX_BIN_SIZE    (24*1024 + 24)



static int cst3xx_check_checksum(struct i2c_client * client)
{
	int ret;
	int i;
	unsigned int  checksum;
	unsigned int  bin_checksum;
	unsigned char buf[4];
	const unsigned char *pData;

	for(i=0; i<5; i++)
	{
		buf[0] = 0xA0;
		buf[1] = 0x00;
		ret = cst3xx_i2c_read_register(client, buf, 1);
		if(ret < 0)
		{
			mdelay(2);
			continue;
		}

		if(buf[0]!=0)
			break;
		else
		mdelay(2);
	}
    mdelay(2);


    if(buf[0]==0x01)
	{
		buf[0] = 0xA0;
		buf[1] = 0x08;
		ret = cst3xx_i2c_read_register(client, buf, 4);

		if(ret < 0)	return -1;

		// read chip checksum
		checksum = buf[0] + (buf[1]<<8) + (buf[2]<<16) + (buf[3]<<24);

        pData=(unsigned char  *)hyn_ts_data->p_hynitron_upgrade_firmware +24*1024+16;   //7*1024 +512
		bin_checksum = pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);

        HYN_INFO(" the updated ic checksum is :0x%x. the updating firmware checksum is:0x%x------\n", checksum, bin_checksum);

        if(checksum!=bin_checksum)
		{
			HYN_ERROR("hyn check sum error.\n");		
			return -1;

		}

	}
	else
	{
		HYN_INFO("hyn No checksum.\n");
		return -1;
	}
	return 0;
}
static int cst3xx_into_program_mode(struct i2c_client * client)
{
	int ret;
	unsigned char buf[4];

	buf[0] = 0xA0;
	buf[1] = 0x01;
	buf[2] = 0xAA;	//set cmd to enter program mode
	ret = cst3xx_i2c_write(client, buf, 3);
	if (ret < 0)  return -1;
	mdelay(2);

	buf[0] = 0xA0;
	buf[1] = 0x02;	//check whether into program mode
	ret = cst3xx_i2c_read_register(client, buf, 1);
	if (ret < 0)  return -1;


	if(hyn_ts_data->config_chip_series==HYN_CHIP_CST3XX){
		if (buf[0] != 0xAC) return -1;
	}else{
		if (buf[0] != 0x55) return -1;
	}

	
	return 0;
}

static int cst3xx_exit_program_mode(struct i2c_client * client)
{
	int ret;
	unsigned char buf[3];

	buf[0] = 0xA0;
	buf[1] = 0x06;
	buf[2] = 0xEE;
	ret = cst3xx_i2c_write(client, buf, 3);
	mdelay(10);	//wait for restart

	return 0;
}

static int cst3xx_erase_program_area(struct i2c_client * client)
{
	int ret;
	unsigned char buf[3];

	buf[0] = 0xA0;
	buf[1] = 0x02;
	buf[2] = 0x00;		//set cmd to erase main area
	ret = cst3xx_i2c_write(client, buf, 3);
	if (ret < 0) return -1;

	mdelay(5);

	buf[0] = 0xA0;
	buf[1] = 0x03;
	ret = cst3xx_i2c_read_register(client, buf, 1);
	if (ret < 0)  return -1;

	if (buf[0] != 0x55) return -1;

	return 0;
}

static int cst3xx_write_program_data(struct i2c_client * client,
		const unsigned char *pdata)
{
	int i, ret;
	unsigned char *i2c_buf;
	unsigned short eep_addr;
	int total_kbyte;
#if HYN_IIC_TRANSFER_LIMIT
	unsigned char temp_buf[8];
	unsigned short iic_addr;
	int  j;

#endif

	i2c_buf = kmalloc(sizeof(unsigned char)*(1024 + 2), GFP_KERNEL);
	if (i2c_buf == NULL)
		return -1;

	//make sure fwbin len is N*1K
	//total_kbyte = len / 1024;
	total_kbyte = 24;
	for (i=0; i<total_kbyte; i++) {
		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x14;
		eep_addr = i << 10;		//i * 1024
		i2c_buf[2] = eep_addr;
		i2c_buf[3] = eep_addr>>8;
		ret = cst3xx_i2c_write(client, i2c_buf, 4);
		if (ret < 0)
			goto error_out;

#if HYN_IIC_TRANSFER_LIMIT
		memcpy(i2c_buf, pdata + eep_addr, 1024);
		for(j=0; j<256; j++) {
			iic_addr = (j<<2);
    	temp_buf[0] = (iic_addr+0xA018)>>8;
    	temp_buf[1] = (iic_addr+0xA018)&0xFF;
		temp_buf[2] = i2c_buf[iic_addr+0];
		temp_buf[3] = i2c_buf[iic_addr+1];
		temp_buf[4] = i2c_buf[iic_addr+2];
		temp_buf[5] = i2c_buf[iic_addr+3];
    	ret = cst3xx_i2c_write(client, temp_buf, 6);
    		if (ret < 0)
    			goto error_out;
		}
#else

		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x18;
		memcpy(i2c_buf + 2, pdata + eep_addr, 1024);
		ret = cst3xx_i2c_write(client, i2c_buf, 1026);
		if (ret < 0)
			goto error_out;
#endif

		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x04;
		i2c_buf[2] = 0xEE;
		ret = cst3xx_i2c_write(client, i2c_buf, 3);
		if (ret < 0)
			goto error_out;

		mdelay(60);

		i2c_buf[0] = 0xA0;
		i2c_buf[1] = 0x05;
		ret = cst3xx_i2c_read_register(client, i2c_buf, 1);
		if (ret < 0)
			goto error_out;

		if (i2c_buf[0] != 0x55)
			goto error_out;

	}

	i2c_buf[0] = 0xA0;
	i2c_buf[1] = 0x03;
	i2c_buf[2] = 0x00;
	ret = cst3xx_i2c_write(client, i2c_buf, 3);
	if (ret < 0)
		goto error_out;

	mdelay(8);

	if (i2c_buf != NULL) {
		kfree(i2c_buf);
		i2c_buf = NULL;
	}

	return 0;

error_out:
	if (i2c_buf != NULL) {
		kfree(i2c_buf);
		i2c_buf = NULL;
	}
	return -1;
}

static int cst3xx_update_firmware(struct i2c_client * client, const unsigned char *pdata)
{
	int ret;
	int retry_timer = 0;
	int retry = 0;

	HYN_INFO("----------upgrade cst3xx begain------------\n");
	mdelay(20);
		
START_FLOW:
	hyn_reset_proc(5+retry_timer);
	client->addr = hyn_ts_data->chip_ic_bootloader_addr;
	hyn_ts_data->fw_updating=1;
	ret = cst3xx_into_program_mode(client);
	if (ret < 0) {
		HYN_ERROR("into program mode failed.\n");
		if(retry_timer<20) retry_timer++;
 		goto err_out;
	}
	hyn_ts_data->fw_updating=2;
	ret = cst3xx_erase_program_area(client);
	if (ret<0) {
		HYN_ERROR("erase main area failed.\n");
		goto err_out;
	}
	hyn_ts_data->fw_updating=3;
	ret = cst3xx_write_program_data(client, pdata);
	if (ret < 0) {
		HYN_ERROR("write program data into cstxxx failed.\n");
		goto err_out;
	}
	hyn_ts_data->fw_updating=4;
    ret =cst3xx_check_checksum(client);
	if (ret < 0) {
		HYN_ERROR("after write program cst3xx_check_checksum failed.\n");
		goto err_out;
	}
	hyn_ts_data->fw_updating=5;
	ret = cst3xx_exit_program_mode(client);
	if (ret < 0) {
		HYN_ERROR("exit program mode failed.\n");
		//goto err_out;
	}
	client->addr = hyn_ts_data->chip_ic_main_addr;
	hyn_reset_proc(30);
	
	hyn_ts_data->fw_updating=6;
	HYN_INFO(" hyn----------cst3xx_update_firmware  end------------\n");

	return 0;

err_out:
	if (retry < 30) {
		retry++;
		mdelay(30);
		goto START_FLOW;
	}
	else {	
		client->addr = hyn_ts_data->chip_ic_main_addr;
		return -1;
	}
}

static int cst3xx_update_judge( unsigned char *pdata, int strict)
{
	unsigned short ic_type, project_id;
	unsigned int fw_checksum, fw_version;
	const unsigned int *p;
	int i;
	unsigned char *pBuf;

	fw_checksum = 0x55;
	p = (const unsigned int *)pdata;
	for (i=0; i<(CST3XX_BIN_SIZE-4); i+=4) {
		fw_checksum += (*p);
		p++;
	}

	if (fw_checksum != (*p)) {
		HYN_ERROR("calculated checksum error:0x%x not equal 0x%x.\n", fw_checksum, *p);
		return -1;	//bad fw, so do not update
	}

	pBuf = &pdata[CST3XX_BIN_SIZE-16];

	project_id = pBuf[1];
	project_id <<= 8;
	project_id |= pBuf[0];

	ic_type = pBuf[3];
	ic_type <<= 8;
	ic_type |= pBuf[2];

	fw_version = pBuf[7];
	fw_version <<= 8;
	fw_version |= pBuf[6];
	fw_version <<= 8;
	fw_version |= pBuf[5];
	fw_version <<= 8;
	fw_version |= pBuf[4];

	fw_checksum = pBuf[11];
	fw_checksum <<= 8;
	fw_checksum |= pBuf[10];
	fw_checksum <<= 8;
	fw_checksum |= pBuf[9];
	fw_checksum <<= 8;
	fw_checksum |= pBuf[8];

	HYN_INFO("the updating firmware:project_id:0x%04x,ic type:0x%04x,fw_version:0x%x,checksum:0x%x\n",
			project_id, ic_type, fw_version, fw_checksum);

#if HYN_UPDATE_FIRMWARE_FORCE
    HYN_INFO("update firmware online force.\n");
    return 0;
#endif


	if(hyn_ts_data->chip_ic_checkcode ==0xffffffff){
		HYN_ERROR("chip ic have no firmware,start update config firmware.\n");
		return 0;
	}else if((hyn_ts_data->chip_ic_type!=ic_type)
		||(hyn_ts_data->chip_ic_project_id!=project_id)){
		
		HYN_ERROR("chip_ic_type:0x%04x,chip_ic_project_id:0x%04x,not match.\n",hyn_ts_data->chip_ic_type, hyn_ts_data->chip_ic_project_id );
		return -1;
	} 
	
	if (strict > 0) {
		if (hyn_ts_data->chip_ic_checksum != fw_checksum){
			
			if (hyn_ts_data->chip_ic_fw_version >fw_version){
				HYN_ERROR("fw version(%d), ic version(%d).\n",fw_version, hyn_ts_data->chip_ic_fw_version );
				return -1;
			}
		}else{
			HYN_ERROR("fw checksum(0x%x), ic checksum(0x%x).\n",fw_checksum, hyn_ts_data->chip_ic_checksum );
			return -1;
		}

	}

	return 0;
}

static int cst3xx_boot_update_fw(struct i2c_client *client)
{
	int ret;
	
	HYN_FUNC_ENTER();

	if(hyn_ts_data->apk_upgrade_flag==0)
		ret = cst3xx_update_judge(hyn_ts_data->p_hynitron_upgrade_firmware, 1);
	else
		ret = cst3xx_update_judge(hyn_ts_data->p_hynitron_upgrade_firmware, 0);	
	if (ret < 0) {
		HYN_ERROR(" update_judge fail,no need to update firmware.\n");
		return 0;
	}
	
	ret = cst3xx_update_firmware(client, hyn_ts_data->p_hynitron_upgrade_firmware);
	if (ret < 0){
		HYN_ERROR("update firmware failed,fw_updating:%d .\n",hyn_ts_data->fw_updating);
		return -1;
	}
	hyn_ts_data->fw_updating=0;

    mdelay(50);

	ret = cst3xx_firmware_info(client);
	if (ret < 0) {
		HYN_ERROR(" after update read version and checksum fail.\n");
		return -1;
	}

	HYN_FUNC_EXIT();
	return 0;
}
#endif

#endif  

#if HYN_SYS_AUTO_SEARCH_FIRMWARE
//system irrelevant
#define ERROR_HN_VER   _ERROR(10)	//HotKnot version error.
#define ERROR_CHECK    _ERROR(11)	//Compare src and dst error.
#define ERROR_RETRY    _ERROR(12)	//Too many retries.
#define ERROR_PATH     _ERROR(13)	//Mount path error
#define ERROR_FW       _ERROR(14)
#define ERROR_FILE     _ERROR(15)
#define ERROR_VALUE    _ERROR(16)	//Illegal value of variables

s32 hyn_check_fs_mounted(char *path_name)
{
	struct path root_path;
	struct path path;
	s32 err;

	err = kern_path("/", LOOKUP_FOLLOW, &root_path);
	if (err)
		return -1;

	err = kern_path(path_name, LOOKUP_FOLLOW, &path);
	if (err) {
		err = -1;
		goto check_fs_fail;
	}

	if (path.mnt->mnt_sb == root_path.mnt->mnt_sb) {
		// not mounted
		err = -1;
	} else {
		err = 0;
	}

	path_put(&path);
check_fs_fail:
	path_put(&root_path);
	return err;
}


int hyn_auto_read_file(char *file_name, u8 **file_buf)
{
	int ret = 0;
	char file_path[FILE_NAME_LENGTH] = { 0 };
	struct file *filp = NULL;
	struct inode *inode;
	mm_segment_t old_fs;
	loff_t pos;
	loff_t file_len = 0;
	int retry=20;

	HYN_FUNC_ENTER();

	if ((file_name == NULL) || (file_buf == NULL)) {
		HYN_ERROR("filename/filebuf is NULL");
		return -EINVAL;
	}

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	snprintf(file_path, FILE_NAME_LENGTH, "%s%s", HYN_UPDATE_FIRMWARE_PATH_1, file_name);
	HYN_INFO("file_path:%s.\n",file_path);	

	while (retry-- > 0) {
		msleep(500);
		// check if rootfs is ready
		if (hyn_check_fs_mounted("/data")) {
			HYN_INFO("filesystem is not ready");
			continue;
		}
	}
	
	filp = filp_open(file_path, O_RDONLY, 0);
	if (IS_ERR(filp)) {
		HYN_ERROR("open %s file fail, try open /sdcard/firmware/", file_path);
		snprintf(file_path, FILE_NAME_LENGTH, "%s%s",HYN_UPDATE_FIRMWARE_PATH_2 , file_name);
		HYN_INFO("file_path:%s.\n",file_path);
		filp = filp_open(file_path, O_RDONLY, 0);
		if (IS_ERR(filp)) {
			HYN_ERROR("open %s file fail", file_path);
			goto ERROR;
		}
	}
	inode = filp->f_inode;

	file_len = inode->i_size;
	*file_buf = vmalloc(file_len);
	if (*file_buf == NULL) {
		HYN_ERROR("file buf malloc fail");
		filp_close(filp, NULL);
		goto ERROR;
	}

	pos = 0;
	ret = vfs_read(filp, *file_buf, file_len, &pos);
	if (ret < 0)
		HYN_ERROR("read file fail");
	HYN_INFO("file len:%d read len:%d pos:%d", (u32) file_len, ret, (u32) pos);
	filp_close(filp, NULL);
	set_fs(old_fs);
	
	HYN_FUNC_EXIT();

	return ret;
	
ERROR:
	filp_close(filp, NULL);
	set_fs(old_fs);
	return -1;
}

int hyn_sys_auto_search_firmware(void)
{
	u8 *fw_file_buf = NULL;
	int ret;
	char *fw_name=	NULL;
	char fwname[FILE_NAME_LENGTH] = { 0 };

	HYN_FUNC_ENTER();

	memset(fwname, 0, sizeof(fwname));
	snprintf(fwname, PAGE_SIZE, "%s", HYN_UPDATE_FIRMWARE_FILTNAME_1);
	fwname[sizeof(HYN_UPDATE_FIRMWARE_FILTNAME_1) - 1] = '\0';
	HYN_INFO("fwname:%s.\n",fwname);	
	ret = hyn_auto_read_file(fw_name, &fw_file_buf);
	if (ret < 0) {
		HYN_ERROR("hyn_auto_read_file.\n");
		return -1;
	}
	if(fw_file_buf!=NULL){
		hyn_ts_data->apk_upgrade_flag=1;
		hyn_ts_data->p_hynitron_upgrade_firmware=fw_file_buf;
	}else{
		HYN_ERROR("hyn_sys_auto_search_firmware fw_file_buf is null.\n");
	}

	HYN_FUNC_EXIT();

	return 0;

}
#endif











/*******************************************************
Function:
    get firmware version, ic type...
Input:
    client: i2c client
Output:
    success: 0
    fail:	-1
*******************************************************/
static int cst3xx_firmware_info(struct i2c_client * client)
{
	int ret;
	unsigned char buf[28];
	HYN_FUNC_ENTER();
	
	buf[0] = 0xD1;
	buf[1] = 0x01;
	ret = cst3xx_i2c_write(client, buf, 2);
	if (ret < 0) return -1;
	
	mdelay(10);

	buf[0] = 0xD1;
	buf[1] = 0xFC;
	ret = cst3xx_i2c_read_register(client, buf, 4);
	if (ret < 0) return -1;

	//0xCACA0000
	hyn_ts_data->chip_ic_checkcode = buf[3];
	hyn_ts_data->chip_ic_checkcode <<= 8;
	hyn_ts_data->chip_ic_checkcode |= buf[2];
	hyn_ts_data->chip_ic_checkcode <<= 8;
	hyn_ts_data->chip_ic_checkcode |= buf[1];
	hyn_ts_data->chip_ic_checkcode <<= 8;
	hyn_ts_data->chip_ic_checkcode |= buf[0];
	
	HYN_INFO("the chip chip_ic_checkcode:0x%x.\r\n",hyn_ts_data->chip_ic_checkcode);
	if((hyn_ts_data->chip_ic_checkcode&0xffff0000)!=0xCACA0000){
		HYN_ERROR("cst3xx_firmware_info read error .\r\n");
		hyn_ts_data->chip_ic_checkcode =0xffffffff;
		return -1;
	}

	mdelay(10);

	buf[0] = 0xD2;
	buf[1] = 0x04;
	ret = cst3xx_i2c_read_register(client, buf, 4);
	if (ret < 0) return -1;
	hyn_ts_data->chip_ic_type = buf[3];
	hyn_ts_data->chip_ic_type <<= 8;
	hyn_ts_data->chip_ic_type |= buf[2];

	
	hyn_ts_data->chip_ic_project_id = buf[1];
	hyn_ts_data->chip_ic_project_id <<= 8;
	hyn_ts_data->chip_ic_project_id |= buf[0];

	HYN_INFO("the chip ic chip_ic_type :0x%x, chip_ic_project_id:0x%x\r\n",
		hyn_ts_data->chip_ic_type, hyn_ts_data->chip_ic_project_id);

	mdelay(2);
	
	buf[0] = 0xD2;
	buf[1] = 0x08;
	ret = cst3xx_i2c_read_register(client, buf, 8);
	if (ret < 0) return -1;	

	hyn_ts_data->chip_ic_fw_version = buf[3];
	hyn_ts_data->chip_ic_fw_version <<= 8;
	hyn_ts_data->chip_ic_fw_version |= buf[2];
	hyn_ts_data->chip_ic_fw_version <<= 8;
	hyn_ts_data->chip_ic_fw_version |= buf[1];
	hyn_ts_data->chip_ic_fw_version <<= 8;
	hyn_ts_data->chip_ic_fw_version |= buf[0];

	hyn_ts_data->chip_ic_checksum = buf[7];
	hyn_ts_data->chip_ic_checksum <<= 8;
	hyn_ts_data->chip_ic_checksum |= buf[6];
	hyn_ts_data->chip_ic_checksum <<= 8;
	hyn_ts_data->chip_ic_checksum |= buf[5];
	hyn_ts_data->chip_ic_checksum <<= 8;
	hyn_ts_data->chip_ic_checksum |= buf[4];	


	HYN_INFO("chip_ic_fw_version:0x%x, chip_ic_checksum:0x%x\r\n",
		hyn_ts_data->chip_ic_fw_version, hyn_ts_data->chip_ic_checksum);

	if(hyn_ts_data->chip_ic_fw_version==0xA5A5A5A5)
	{
		HYN_ERROR(" the chip ic don't have firmware. \n");
		return -1;
	}


	buf[0] = 0xD0;
	buf[1] = 0x47;
	ret = cst3xx_i2c_read_register(client, buf, 1);
	if (ret < 0) return -1;

	hyn_ts_data->chip_ic_module_id=buf[0];	

    buf[0] = 0xD1;
	buf[1] = 0x09;
	ret = cst3xx_i2c_write(client, buf, 2);
	if (ret < 0) return -1;
    mdelay(5);
	
	HYN_FUNC_EXIT();
	return 0;
}


/*
 *
 */
 
int cst8xx_firmware_info(struct i2c_client *mclient)
{
	int ret=-1;
    unsigned char reg_buf[2];
	unsigned char buf[4];
	unsigned short fwversion,chipversion;
	
	HYN_FUNC_ENTER();
	msleep(100);	
	
    reg_buf[0] = 0xA6;
	
    ret = hyn_i2c_read(hyn_ts_data->client,reg_buf,1,buf,2);
	
	if (ret < 0){
		dev_err(&hyn_ts_data->client->dev, "f%s: i2c read error.\n",__func__);
		return -1;
	}
	
	fwversion   = *(hyn_ts_data->p_hynitron_upgrade_firmware+0x3BFD+6);
	fwversion <<= 8;
	fwversion  += *(hyn_ts_data->p_hynitron_upgrade_firmware+0x3BFC+6);
	HYN_INFO("\r\nhyn fwversion: %x\r\n",fwversion);
	
	chipversion   = buf[1];
	chipversion <<= 8;
	chipversion  += buf[0];
	hyn_ts_data->chip_ic_fw_version=chipversion;

	
	HYN_INFO("\r\nhyn chipversion: %x\r\n",chipversion);
	
	if(chipversion>fwversion)
	{
		update_fw_flag = 0;
		
		HYN_INFO("\r\nhyn update_fw_flag: %x\r\n",update_fw_flag);
	}
	 HYN_FUNC_EXIT();
	 return ret;
}

static int hyn_find_fw_idx(u8 check_project_id)
{
	int i = 0;
	HYN_FUNC_ENTER();

	if (0 != hyn_ts_data->config_chip_type ) {
		for (i=0; i<ARRAY_SIZE(hynitron_fw_grp); i++) {
			if(check_project_id==1){
				if((hyn_ts_data->chip_ic_project_id  ==hynitron_fw_grp[i].project_id)
				 &&(hyn_ts_data->config_chip_type	==hynitron_fw_grp[i].chip_type))
				{
					return i;	
				}
			}else{
				if(hyn_ts_data->config_chip_type ==hynitron_fw_grp[i].chip_type)
				{
					return i;
				}
			}
			

		}
	}
	HYN_FUNC_EXIT();
	return -1;

}
/*******************************************************
Function:
    get firmware version, ic type...
Input:
    client: i2c client
Output:
    success: 0
    fail:	-1
*******************************************************/
int hyn_firmware_info(struct i2c_client * client)
{
	int ret=-1;
	HYN_FUNC_ENTER();

	if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_MUT_CAP)
	{
    //mutcap
    	if(hyn_ts_data->config_chip_series != HYN_CHIP_CST2XX)
			ret=cst3xx_firmware_info(client);
		
		
	}else if(hyn_ts_data->config_chip_product_line==HYN_CHIP_PRODUCT_LINE_SEL_CAP){
	//selfcap
		ret=cst8xx_firmware_info(client);
	}
	HYN_FUNC_EXIT();
	return ret; 
}


int hyn_boot_update_fw(struct i2c_client * client)
{
	int ret=-1;
	int proj_id=0;
	int check_proj_id=1;

	HYN_FUNC_ENTER();

	if(hyn_ts_data->apk_upgrade_flag==0){	

		if(hyn_ts_data->chip_ic_checkcode ==0xffffffff){
			HYN_ERROR("hyn ic have no firmware,update default firmware.\n");
			check_proj_id=0;
			proj_id=hyn_find_fw_idx(check_proj_id);
			HYN_INFO("hyn_find_match chip type,fw_idx: proj_id:%d.\r\n",proj_id);
			if(proj_id<0){
				HYN_ERROR("hyn not find config chip type firmware,please check config chip type or firmware.\n");
				return 0;
			}else{
				hyn_ts_data->fw_name					    =hynitron_fw_grp[proj_id].name;
				hyn_ts_data->p_hynitron_upgrade_firmware   	=hynitron_fw_grp[proj_id].fw;
				hyn_ts_data->fw_length				        =hynitron_fw_grp[proj_id].fw_length;
				HYN_INFO("find default TP firmware(%s)!\n", hynitron_fw_grp[proj_id].name);
			}
			
		}else{
			check_proj_id=1;
			proj_id=hyn_find_fw_idx(check_proj_id);
			HYN_INFO("hyn_find_fw_idx: proj_id:%d.\r\n",proj_id);	
			if(proj_id<0){
				HYN_ERROR("not find matched TP firmware,please check chip type and chip project id. !\n"); 
				return 0;
			}else{	
				HYN_INFO("find matched TP firmware(%s)!\n", hynitron_fw_grp[proj_id].name);
				hyn_ts_data->fw_name					   =hynitron_fw_grp[proj_id].name;
				hyn_ts_data->p_hynitron_upgrade_firmware   =hynitron_fw_grp[proj_id].fw;
				hyn_ts_data->fw_length				       =hynitron_fw_grp[proj_id].fw_length;
			}
		}	
	}

	if(hyn_ts_data->p_hynitron_upgrade_firmware==NULL){
		HYN_ERROR("hyn_ts_data->p_hynitron_upgrade_firmware is NULL .\n");
		return 0;
		
	}

#if  HYN_ESDCHECK_EN
	hyn_esd_suspend();
#endif

	hyn_irq_disable();	

	hyn_ts_data->work_mode = HYN_WORK_MODE_UPDATE;	//add work mode

	switch(hyn_ts_data->config_chip_series){
		case HYN_CHIP_CST0XX: 			
			 break;
		case HYN_CHIP_CST2XX:
#if HYN_EN_AUTO_UPDATE_CST2xx
			 ret =cst2xx_boot_update_fw(client);
#endif
			break;
		case HYN_CHIP_CST6XX: 
#if (HYN_EN_AUTO_UPDATE_CST6xx)
			ret = cst6xx_boot_update_fw(client);
#endif
			break;
		case HYN_CHIP_CST1XX:
		case HYN_CHIP_CST3XX: 
		case HYN_CHIP_CST9XX: 
#if (HYN_EN_AUTO_UPDATE_CST1xx||HYN_EN_AUTO_UPDATE_CST3xx||HYN_EN_AUTO_UPDATE_CST9xx)
			ret = cst3xx_boot_update_fw(client);
#endif
			break;
		case HYN_CHIP_CST1XXSE:
		case HYN_CHIP_CST2XXSE:
		case HYN_CHIP_CST3XXSE:
#if (HYN_EN_AUTO_UPDATE_CST1xxSE||HYN_EN_AUTO_UPDATE_CST2xxSE||HYN_EN_AUTO_UPDATE_CST3xxSE)
			ret = cst3xxse_boot_update_fw(client);
#endif
			break;
		case HYN_CHIP_CST7XX:
		case HYN_CHIP_CST8XX:
#if HYN_EN_AUTO_UPDATE_CST78xx
			ret = cst8xx_boot_update_fw(client);
#endif
			break;
		default :
			break;

	}
	
	hyn_irq_enable();	

#if  HYN_ESDCHECK_EN
	hyn_esd_resume();
#endif

	hyn_ts_data->work_mode = HYN_WORK_MODE_NORMAL;	
	//add work mode

	HYN_FUNC_EXIT();
    return ret;

}



static int cst0xx_bootloader_enter(struct i2c_client *client)
{
	HYN_FUNC_ENTER();
	HYN_FUNC_EXIT();
	return 0;
}

static int cst2xx_bootloader_enter(struct i2c_client *client)
{

	int ret=-1;
	u8  retry;
	unsigned char buf[4];
	HYN_FUNC_ENTER();

	for(retry=0;retry<5;retry++){

		hyn_reset_proc(7+retry);
		if(hyn_ts_data->chip_ic_bootloader_addr!=client->addr){
			client->addr=hyn_ts_data->chip_ic_bootloader_addr;
		}	
		HYN_DEBUG(" bootloader mode client->addr:0x%x.\r\n",client->addr);
	    buf[0] = 0xD1;
	    buf[1] = 0x11;
	    ret = cst3xx_i2c_write (hyn_ts_data->client, buf, 2);
	    if (ret < 0) continue;

	    mdelay (1); //wait enter download mode

	    buf[0] = 0xD0;
	    buf[1] = 0x01;
	    ret = cst3xx_i2c_read_register (hyn_ts_data->client, buf, 1);
	    if (ret < 0)  continue;
	    if (buf[0] == 0x55) {
	        HYN_DEBUG ("[HYN]receive 0x55 success.\r\n");
	        break;
	    }
	}
    buf[0] = 0xD1;
    buf[1] = 0x10;   //enter writer register mode
    ret = cst3xx_i2c_write (hyn_ts_data->client, buf, 2);
    if (ret < 0) {
        HYN_DEBUG ("[HYN]Send cmd 0xD110 failed. \r\n");
        return -1;
    }
	HYN_FUNC_EXIT();
	return 0;
}

static int cst3xx_bootloader_enter(struct i2c_client *client)
{
	int ret=-1;
	u8  retry;
	unsigned char buf[4];

	HYN_FUNC_ENTER();
    for(retry=0;retry<5;retry++){

		hyn_reset_proc(7+retry);
		if(hyn_ts_data->chip_ic_bootloader_addr!=client->addr){
			client->addr=hyn_ts_data->chip_ic_bootloader_addr;
		}
		HYN_DEBUG(" bootloader mode client->addr:0x%x.\r\n",client->addr);
		buf[0] = 0xA0;
		buf[1] = 0x01;
		buf[2] = 0xAA;	//set cmd to enter program mode
		ret = cst3xx_i2c_write(client, buf, 3);
		if (ret < 0) continue;
		mdelay(2);

		buf[0] = 0xA0;
		buf[1] = 0x02;	//check whether into program mode
		ret = cst3xx_i2c_read_register(client, buf, 1);
		if (ret < 0)  continue;

 		if(hyn_ts_data->config_chip_series==HYN_CHIP_CST3XX){
			if (buf[0] == 0xAC) break;

		}else{
			if (buf[0] == 0x55) break;
		}
		
    }

	if(hyn_ts_data->config_chip_series==HYN_CHIP_CST3XX){
		if (buf[0] != 0xAC) 
		{
			HYN_ERROR("cst3xx_bootloader_enter error,please check VCC/RST/SDA/SCL and chip type.\r\n");
			return -1;
		}else{
	        	HYN_DEBUG("cst3xx_bootloader_enter detect success,buf[0]=%d.\n", buf[0]);	
		}
		
	}else{
		if (buf[0] != 0x55)
		{
			HYN_ERROR("cst1xx_bootloader_enter error,please check VCC/RST/SDA/SCL and chip type.\r\n");
			return -1;
		}else{
	        	HYN_DEBUG("cst1xx_bootloader_enter detect success,buf[0]=%d.\n", buf[0]);	
		}
		
	}
	
	HYN_FUNC_EXIT();
	return 0;
}

static int cst6xx_bootloader_enter(struct i2c_client *client)
{
	int ret=-1;
	u8  retry;
	unsigned char buf[4];

	HYN_FUNC_ENTER();
    for(retry=0;retry<5;retry++){

		hyn_reset_proc(7+retry);
		if(hyn_ts_data->chip_ic_bootloader_addr!=client->addr){
			client->addr=hyn_ts_data->chip_ic_bootloader_addr;
		}
		HYN_DEBUG(" bootloader mode client->addr:0x%x.\r\n",client->addr);
		buf[0] = 0xA0;
		buf[1] = 0x01;
		buf[2] = 0xAB;	//set cmd to enter program mode
		ret = cst3xx_i2c_write(client, buf, 3);
		if (ret < 0) continue;
		mdelay(2);

		buf[0] = 0xA0;
		buf[1] = 0x02;	//check whether into program mode
		ret = cst3xx_i2c_read_register(client, buf, 1);
		if (ret < 0)  continue;

		if (buf[0] == 0x55) break;
		
    }

	if (buf[0] != 0x55)
	{
		HYN_ERROR("cst6xx_bootloader_enter error,please check VCC/RST/SDA/SCL and chip type.\r\n");
		return -1;
	}else{
        	HYN_DEBUG("cst6xx_bootloader_enter detect success,buf[0]=0x%x.\n", buf[0]);	
	}
			
	HYN_FUNC_EXIT();
	return 0;
}

static int cst3xxse_bootloader_enter(struct i2c_client *client)
{
	int ret=-1;
	u8  retry;
	unsigned char buf[4];

	HYN_FUNC_ENTER();
    for(retry=0;retry<5;retry++){

		hyn_reset_proc(7+retry);
		if(hyn_ts_data->chip_ic_bootloader_addr!=client->addr){
			client->addr=hyn_ts_data->chip_ic_bootloader_addr;
		}
		HYN_DEBUG(" bootloader mode client->addr:0x%x.\r\n",client->addr);
		buf[0] = 0xA0;
		buf[1] = 0x01;
		buf[2] = 0xAA;	//set cmd to enter program mode
		ret = cst3xx_i2c_write(client, buf, 3);
		if (ret < 0) continue;
		mdelay(5);

		buf[0] = 0xA0;
		buf[1] = 0x03;	//check whether into program mode
		ret = cst3xx_i2c_read_register(client, buf, 1);
		if (ret < 0)  continue;

		if (buf[0] == 0x55) break;
    }
	if (buf[0] != 0x55){
		HYN_ERROR("cst3xx_bootloader_enter error,please check VCC/RST/SDA/SCL and chip type.\r\n");
		return -1;
	}else{
        HYN_DEBUG("cst3xx_bootloader_enter detect success,buf[0]=%d.\n", buf[0]);	
	}

	HYN_FUNC_EXIT();
	return 0;
}
static int cst8xx_bootloader_enter(struct i2c_client *client)
{
	char retryCnt = 10;
	HYN_FUNC_ENTER();
	     
     hyn_reset_proc(1);
     mdelay(5);
	 if(hyn_ts_data->chip_ic_bootloader_addr!=client->addr){
		client->addr=hyn_ts_data->chip_ic_bootloader_addr;
	 }
	 HYN_DEBUG(" bootloader mode client->addr:0x%x.\r\n",client->addr);
     while(retryCnt--){
         u8 cmd[3];
         cmd[0] = 0xAA;
         if (-1 == hyn_i2c_write_bytes(0xA001,cmd,1,REG_LEN_2B)){  // enter program mode
             mdelay(2); // 4ms
             continue;                   
         }
         if (-1 == hyn_i2c_read_bytes(0xA003,cmd,1,REG_LEN_2B)) { // read flag
             mdelay(2); // 4ms
             continue;                           
         }else{
             if (cmd[0] != 0x55){
                 mdelay(2); // 4ms
                 continue;
             }else{
                 HYN_DEBUG("cst8xx_bootloader_enter detect success,cmd[0]=%d.\n", cmd[0]);	
                 return 0;
             }
         }
     }
  

	HYN_FUNC_EXIT();
    return -1;
}

int hyn_detect_bootloader(struct i2c_client *client)
{
	int ret =-1;
	unsigned char scan_id;
	
	HYN_FUNC_ENTER();
	
	for(scan_id=0;scan_id<ARRAY_SIZE(hynitron_chip_type_grp);scan_id++){
		if(hyn_ts_data->config_chip_type == hynitron_chip_type_grp[scan_id].chip_type){
			hyn_ts_data->config_chip_series      =hynitron_chip_type_grp[scan_id].chip_series;
			hyn_ts_data->config_chip_product_line=hynitron_chip_type_grp[scan_id].chip_product_line;		
			hyn_ts_data->chip_ic_bootloader_addr   =hynitron_chip_type_grp[scan_id].chip_boot_addr;		
			break;
		}
	}
	if(scan_id==ARRAY_SIZE(hynitron_chip_type_grp))
	{
		HYN_ERROR("config chip type error,please check HYN_CHIP_TYPE_CONFIG(common.h).\r\n");
		return -1;
	}	
    switch(hyn_ts_data->config_chip_series){
		case HYN_CHIP_CST0XX: 
			 ret=cst0xx_bootloader_enter(client);
			 break;
		case HYN_CHIP_CST2XX:
			 ret=cst2xx_bootloader_enter(client);
			 break;
		case HYN_CHIP_CST6XX: 
			 ret=cst6xx_bootloader_enter(client);
			 break;
		case HYN_CHIP_CST1XX:
		case HYN_CHIP_CST3XX: 
		case HYN_CHIP_CST9XX: 
			 ret=cst3xx_bootloader_enter(client);
			 break;
		case HYN_CHIP_CST1XXSE:
		case HYN_CHIP_CST2XXSE:
		case HYN_CHIP_CST3XXSE:
			 ret=cst3xxse_bootloader_enter(client);
			 break;
		case HYN_CHIP_CST7XX:
		case HYN_CHIP_CST8XX:
			 ret=cst8xx_bootloader_enter(client);
			 break;
		default :
			 break;

	}
	if(hyn_ts_data->chip_ic_main_addr!=client->addr){
		client->addr=hyn_ts_data->chip_ic_main_addr;
	}
	HYN_DEBUG("exit bootloader mode,client->addr:0x%x.\r\n",client->addr);
	hyn_reset_proc(40); 
	HYN_FUNC_EXIT();
	return ret;
}


