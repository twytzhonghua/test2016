#ifndef _MOTION_I2C_INTERFACE_H_
#define _MOTION_I2C_INTERFACE_H_
#include <dcl.h>
#include "kal_release.h"
#include "drv_comm.h"
#include "gpio_hw.h"
#include "gpio_sw.h"
#include "kal_general_types.h"

/* for lis2ds12 i2c */
#define MS_I2C_CLK_PIN	41		//define GPIO41 for i2c CLK Singal
#define MS_I2C_DATA_PIN  42		//define GPIO42 for i2c DATA Singal

#define SET_MS_CLK_OUTPUT		GPIO_InitIO(OUTPUT,client->scl_gpio);
#define SET_MS_DATA_OUTPUT		GPIO_InitIO(OUTPUT,client->sda_gpio);
#define SET_MS_DATA_INPUT		GPIO_InitIO(INPUT,client->sda_gpio);
#define SET_MS_CLK_HIGH			GPIO_WriteIO(1,client->scl_gpio);
#define SET_MS_CLK_LOW			GPIO_WriteIO(0,client->scl_gpio);
#define SET_MS_DATA_HIGH		GPIO_WriteIO(1,client->sda_gpio);
#define SET_MS_DATA_LOW			GPIO_WriteIO(0,client->sda_gpio);
#define GET_MS_DATA				GPIO_ReadIO(client->sda_gpio)

#define MOTION_SENSOR_READ       	0x1
#define MOTION_SENSOR_WRITE      	0x0


#define MS_DELAY				1


typedef struct i2c_client{
	unsigned char slave_addr;
	DCL_HANDLE i2c_handle;
	int i2c_owner;
	char name[10];
	int scl_gpio;
	int sda_gpio;
	int int1_gpio;
	int int2_gpio;
	int int1_num;
	int int2_num;		
}I2C_CLIENT;


kal_uint8 ms_i2c_read(I2C_CLIENT *client, kal_uint8 addr);
kal_uint8 ms_i2c_write(I2C_CLIENT *client, kal_uint8 addr, kal_uint8 data);
void ms_i2c_init(I2C_CLIENT *client);
kal_uint8 ms_i2c_read(I2C_CLIENT *client, kal_uint8 addr);
kal_uint8 ms_i2c_write(I2C_CLIENT *client, kal_uint8 addr, kal_uint8 data);

#endif

