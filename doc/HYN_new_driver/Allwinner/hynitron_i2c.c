/**
 *Name        : cst0xx_i2c.c
 *Author      : gary
 *Version     : V1.0
 *Create      : 2018-1-23
 *Copyright   : zxzz
 */


#include "hynitron_core.h"

/*****************************************************************************/
static DEFINE_MUTEX(i2c_rw_access);

int hyn_i2c_read(struct i2c_client *client, char *writebuf, int writelen, char *readbuf, int readlen)
{
    int ret = -1;

    if (client == NULL)
    {
        HYN_ERROR("[IIC][%s]i2c_client==NULL!", __func__);
        return -1;
    }
	
	// client->addr = client->addr & I2C_MASK_FLAG;
    ret = i2c_master_send(client, writebuf, writelen);
	if(ret<0)
		HYN_ERROR("i2c_master_send error\n");
	
	
	// client->addr = (client->addr & I2C_MASK_FLAG) | I2C_WR_FLAG | I2C_RS_FLAG;
	ret = i2c_master_recv(client, readbuf, readlen);   
	if(ret < 0){
		HYN_ERROR("i2c_master_recv i2c read error.\n");
		return ret;
	}
	
    return ret;
}

/*
 *
 */
int hyn_i2c_write(struct i2c_client *client, char *writebuf, int writelen)
{
    int ret = -1;

    if (client == NULL)
    {
        HYN_ERROR("[IIC][%s]i2c_client==NULL!", __func__);
        return -1;
    }	
	// client->addr = client->addr & I2C_MASK_FLAG;
    ret = i2c_master_send(client, writebuf, writelen);
	if(ret<0)
		HYN_ERROR("i2c_master_send error\n");
    return ret;
}


/*
 *
 */
int hyn_i2c_write_byte(struct i2c_client *client, u8 regaddr, u8 regvalue)
{
    u8 buf[2] = {0};

    buf[0] = regaddr;
    buf[1] = regvalue;
    return hyn_i2c_write(client, buf, sizeof(buf));
}
/*
 *
 */
int hyn_i2c_read_byte(struct i2c_client *client, u8 regaddr, u8 *regvalue)
{
    return hyn_i2c_read(client, &regaddr, 1, regvalue, 1);
}

/*****************************************************************/
/*
 *
 */
int hyn_i2c_write_bytes(unsigned short reg,unsigned char *buf,unsigned short len,unsigned char reg_len)
{
	int ret;
    unsigned char mbuf[600];
    if (reg_len == 1){
        mbuf[0] = reg;
        memcpy(mbuf+1,buf,len);
    }else{
        mbuf[0] = reg>>8;
        mbuf[1] = reg;
        memcpy(mbuf+2,buf,len);    
    }

    ret = hyn_i2c_write(hyn_ts_data->client,mbuf,len+reg_len);
	if (ret < 0){
		HYN_ERROR("%s i2c write error.\n", __func__);
	}
    return ret;
}
/*
 *
 */
int hyn_i2c_read_bytes(unsigned short reg,unsigned char* buf,unsigned short len,unsigned char reg_len)
{
	int ret;
    unsigned char reg_buf[2];
    if (reg_len == 1){
        reg_buf[0] = reg;
    }else{
        reg_buf[0] = reg>>8;
        reg_buf[1] = reg;
    }
    ret = hyn_i2c_read(hyn_ts_data->client,reg_buf,reg_len,buf,len);
	if (ret < 0){
		HYN_ERROR("f%s: i2c read error.\n",__func__);
	}
    return ret;
}

/*****************************************************************/

#ifdef HIGH_SPEED_IIC_TRANSFER
 int cst3xx_i2c_read(struct i2c_client *client, unsigned char *buf, int len) 
{ 
	struct i2c_msg msg; 
	int ret = -1; 
	int retries = 0; 
	
	msg.flags |= I2C_M_RD; 
	msg.addr   = client->addr;
	msg.len    = len; 
	msg.buf    = buf;	

	while (retries < 2) { 
		ret = i2c_transfer(client->adapter, &msg, 1); 
		if(ret == 1)
			break; 
		retries++; 
	} 
	
	return ret; 
} 


/*******************************************************
Function:
    read data from register.
Input:
    buf: first two byte is register addr, then read data store into buf
    len: length of data that to read
Output:
    success: number of messages
    fail:	negative errno
*******************************************************/
 int cst3xx_i2c_read_register(struct i2c_client *client, unsigned char *buf, int len) 
{ 
	struct i2c_msg msgs[2]; 
	int ret = -1; 
	int retries = 0; 
	
	msgs[0].flags = client->flags & I2C_M_TEN;
	msgs[0].addr  = client->addr;  
	msgs[0].len   = 2;
	msgs[0].buf   = buf; 

	msgs[1].flags |= I2C_M_RD;
	msgs[1].addr   = client->addr; 
	msgs[1].len    = len; 
	msgs[1].buf    = buf;

	while (retries < 2) { 
		ret = i2c_transfer(client->adapter, msgs, 2); 
		if(ret == 2)
			break; 
		retries++; 
	} 
	
	return ret; 
} 

 int cst3xx_i2c_write(struct i2c_client *client, unsigned char *buf, int len) 
{ 
	struct i2c_msg msg; 
	int ret = -1; 
	int retries = 0;

	msg.flags = client->flags & I2C_M_TEN; 
	msg.addr  = client->addr; 
	msg.len   = len; 
	msg.buf   = buf;		  
	  
	while (retries < 2) { 
		ret = i2c_transfer(client->adapter, &msg, 1); 
		if(ret == 1)
			break; 
		retries++; 
	} 	
	
	return ret; 
}

#else
int cst3xx_i2c_read(struct i2c_client *client, unsigned char *buf, int len) 
{ 
	int ret = -1; 
	int retries = 0; 

	while (retries < 2) { 
		ret = i2c_master_recv(client, buf, len); 
		if(ret<=0) 
		    retries++;
        else
            break; 
	} 
	
	return ret; 
} 

int cst3xx_i2c_write(struct i2c_client *client, unsigned char *buf, int len) 
{ 
	int ret = -1; 
	int retries = 0; 

	while (retries < 2) { 
		ret = i2c_master_send(client, buf, len); 
		if(ret<=0) 
		    retries++;
        else
            break; 
	} 
	
	return ret; 
}

int cst3xx_i2c_read_register(struct i2c_client *client, unsigned char *buf, int len) 
{ 
	int ret = -1; 
    
    ret = cst3xx_i2c_write(client, buf, 2);

    ret = cst3xx_i2c_read(client, buf, len);
	
    return ret; 
} 

#endif

