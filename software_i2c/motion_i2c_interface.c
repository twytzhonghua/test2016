#if defined(MOTION_SENSOR_SUPPORT) && defined(__MTK_TARGET__)

#include <dcl_i2c.h>
#include "motion_i2c_interface.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"

void ms_i2c_init(I2C_CLIENT *client)
{
    GPIO_ModeSetup(client->scl_gpio, 0);
    GPIO_ModeSetup(client->sda_gpio, 0);
#if defined(__CUSTOM_XUN_MOTION_SENSOR_FEATURE__) 
	kal_prompt_trace(MOD_GS, "ms_i2c_init sck=%d, sda=%d\n", 
		client->scl_gpio, client->sda_gpio);
#endif

	SET_MS_CLK_HIGH;
	SET_MS_DATA_HIGH;
	SET_MS_CLK_OUTPUT;
	SET_MS_DATA_OUTPUT;
}

static void MS_Delay(kal_uint16 time)
{
	kal_uint16 i;
	for(i=0;i<time;i++);
}

static void I2C_Start(I2C_CLIENT *client)
{
	SET_MS_DATA_OUTPUT;
	SET_MS_DATA_HIGH;
	SET_MS_CLK_HIGH;
	SET_MS_DATA_LOW;
	MS_Delay(MS_DELAY);
	SET_MS_CLK_LOW;
	MS_Delay(MS_DELAY);
}


static void I2C_Restart(I2C_CLIENT *client)
{
	SET_MS_DATA_OUTPUT;
	SET_MS_DATA_HIGH;
	SET_MS_CLK_HIGH;
	SET_MS_DATA_LOW;
	MS_Delay(MS_DELAY);
	SET_MS_CLK_LOW;
	MS_Delay(MS_DELAY);
}


static void I2C_Stop(I2C_CLIENT *client)
{
	SET_MS_DATA_OUTPUT;
	SET_MS_DATA_LOW;
	MS_Delay(MS_DELAY);
	SET_MS_CLK_HIGH;
	MS_Delay(MS_DELAY);
	SET_MS_DATA_HIGH;
	MS_Delay(MS_DELAY);
}


/*************************************************************************
* FUNCTION
*	motion_sensor_send_byte
*
* DESCRIPTION
*	This function send one byte to through software i2c
*
* PARAMETERS
*	send_byte : the data that writes out through software i2c
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void motion_sensor_send_byte(I2C_CLIENT *client, kal_uint8 send_byte)
{
	signed char i;

/*MSB  data bit 7~0 */
	for (i=7;i>=0;i--)
	{
		if (send_byte & (1<<i))
		{
			SET_MS_DATA_HIGH;
		}
		else
		{
			SET_MS_DATA_LOW;
		}
		SET_MS_CLK_HIGH;

		MS_Delay(MS_DELAY);
		MS_Delay(MS_DELAY);
		SET_MS_CLK_LOW;

//		MS_Delay(MS_DELAY);
	}
/*receive ACK from SLAVE*/
	SET_MS_DATA_INPUT;
	SET_MS_CLK_HIGH;
	if(GET_MS_DATA==0)
		;//kal_prompt_trace(MOD_GS,"revecie ACK(LOW) FORM SLAVE");
	else
		#ifdef ACC_DEBUG
			kal_prompt_trace(MOD_SYSTEM,"can not revecie ACK(LOW) FORM SLAVE");
		#else
			;
		#endif
	SET_MS_CLK_LOW;
	SET_MS_DATA_OUTPUT;
}

/*************************************************************************
* FUNCTION
*	motion_sensor_get_byte
*
* DESCRIPTION
*	This function read one byte through software IIC interface
*
* PARAMETERS
*	None
*
* RETURNS
*	the data that read from SCCB interface
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint8 motion_sensor_get_byte(I2C_CLIENT *client)
{
	kal_uint8 get_byte=0;
	signed char i;

/*MSB  data bit 7~0 */
	SET_MS_DATA_INPUT;
	for (i=7;i>=0;i--)
	{
		SET_MS_CLK_HIGH;

		MS_Delay(MS_DELAY);
//		MS_Delay(MS_DELAY);
		if (GET_MS_DATA)
			get_byte |= (1<<i);
		SET_MS_CLK_LOW;

//		MS_Delay(MS_DELAY);
	}
/* 9th NACK bit */
	SET_MS_DATA_OUTPUT;
	SET_MS_DATA_HIGH;
	SET_MS_CLK_HIGH;
	SET_MS_CLK_LOW;
	SET_MS_DATA_LOW;

	return get_byte;
}


kal_uint8 ms_i2c_read(I2C_CLIENT *client, kal_uint8 addr)
{
	kal_uint8 data=0;

	I2C_Start(client);
	motion_sensor_send_byte(client, client->slave_addr | MOTION_SENSOR_WRITE);
	motion_sensor_send_byte(client, addr);/**/
	I2C_Restart(client);/*sr*/
	motion_sensor_send_byte(client, client->slave_addr | MOTION_SENSOR_READ);
	data=motion_sensor_get_byte(client);
	I2C_Stop(client);

	return data;
}

kal_uint8 ms_i2c_write(I2C_CLIENT *client, kal_uint8 addr, kal_uint8 data)
{
   	I2C_Start(client);
	motion_sensor_send_byte(client, client->slave_addr | MOTION_SENSOR_WRITE);
	motion_sensor_send_byte(client, addr);
	motion_sensor_send_byte(client, data);
	I2C_Stop(client);

}


int sys_i2c_init(I2C_CLIENT *client)
{
	I2C_CONFIG_T i2c_config;
	DCL_STATUS status;

	if(client->i2c_handle <= 0) {
		client->i2c_handle = DclSI2C_Open(DCL_I2C, client->i2c_owner);
		if(client->i2c_handle <= 0) {
            #if defined(__CUSTOM_XUN_MOTION_SENSOR_FEATURE__) 

			kal_prompt_trace(MOD_GS, "open i2c failed\n");
            #endif
			return -1;
		}
        #if defined(__CUSTOM_XUN_MOTION_SENSOR_FEATURE__) 

		kal_prompt_trace(MOD_GS,"i2c handle = %x\n", client->i2c_handle);
        #endif

		i2c_config.eOwner = client->i2c_owner;
		i2c_config.fgGetHandleWait = 1;
		i2c_config.u1SlaveAddress = client->slave_addr;
		i2c_config.u1DelayLen = 0;
		i2c_config.eTransactionMode = DCL_I2C_TRANSACTION_FAST_MODE;
		i2c_config.u4FastModeSpeed = 300;
		i2c_config.u4HSModeSpeed = 0;
		i2c_config.fgEnableDMA = 0;
		status = DclSI2C_Configure(client->i2c_handle, (DCL_CONFIGURE_T *)&i2c_config);
		if(status != STATUS_OK) {
            #if defined(__CUSTOM_XUN_MOTION_SENSOR_FEATURE__) 

			kal_prompt_trace(MOD_GS, "configure i2c failed, status = %d\n", status);
            #endif
			DclSI2C_Close(client->i2c_handle);
			client->i2c_handle = 0;
			return -1;
		}
	}

	return 0;
}


int sys_i2c_write(I2C_CLIENT *client, unsigned char reg_addr, unsigned char reg_data)
{
	I2C_CTRL_SINGLE_WRITE_T single_write;
	DCL_UINT8 databuf[2];
	DCL_STATUS status;

	if(client->i2c_handle <= 0) {
        #if defined(__CUSTOM_XUN_MOTION_SENSOR_FEATURE__) 

		kal_prompt_trace(MOD_GS, "I2C NOT OPEN!\n");
        #endif
		return -1;
	}

	databuf[0] = reg_addr;
	databuf[1] = reg_data;
	single_write.pu1Data = &databuf[0];
	single_write.u4DataLen = 2;
	status = DclSI2C_Control(client->i2c_handle, I2C_CMD_SINGLE_WRITE, (DCL_CTRL_DATA_T *)&single_write);
	if(status != STATUS_OK) {
		if(STATUS_ACKERR == status){	// reset reg has no ack
		#if defined(__CUSTOM_XUN_MOTION_SENSOR_FEATURE__) 

			kal_prompt_trace(MOD_GS, "write data failed, status = %d\n", status);
               #endif
			return -1;
		}
	}

	return 0;
}

int sys_i2c_read(I2C_CLIENT *client, unsigned char reg_addr, unsigned char *buff)
{
	I2C_CTRL_WRITE_AND_READE_T I2cData;
	int ret;
	
	I2cData.pu1OutData = &reg_addr;
	I2cData.u4OutDataLen = 1;
	I2cData.pu1InData = buff;
	I2cData.u4InDataLen = 1;
	ret = DclSI2C_Control(client->i2c_handle, I2C_CMD_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&I2cData);
	if(ret != STATUS_OK)
	{
	#if defined(__CUSTOM_XUN_MOTION_SENSOR_FEATURE__) 

		kal_prompt_trace(MOD_GS, "sys_i2c_read failed, reg_add = %x\n", reg_addr);
    #endif
		return -1;
	}

	return 0;
}
#endif

