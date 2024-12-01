#include "MMA8451.h"
#include <string.h>

#define MAX_TX_BUFFER	50

static esp_err_t writeReg8(MMA8451 *sensor, uint8_t _regAddr, uint8_t _data);
static esp_err_t writeReg(MMA8451 *sensor, uint8_t _regAddr, uint8_t *_data, uint8_t _len);
static esp_err_t readReg8(MMA8451 *sensor, uint8_t _regAddr, uint8_t *_data);
static esp_err_t readReg(MMA8451 *sensor, uint8_t _regAddr, uint8_t *_data, uint8_t _len);

esp_err_t MMA8451_Init(MMA8451 *sensor, i2c_master_dev_handle_t _hi2c, uint8_t _addr)
{
	sensor->_Addr = _addr << 1;
	sensor->_hi2c = _hi2c;
	uint8_t deviceID = 0;
	if (readReg8(sensor, MMA8451_REG_WHOAMI, &deviceID) != ESP_OK || deviceID != MMA8451_WHOAMI_ADDRESS)
		return ESP_FAIL;


	writeReg8(sensor, MMA8451_REG_CTRL_REG2, 0x40); //reset

	uint8_t reg_ctrl_reg2;
	do
	{
		readReg8(sensor, MMA8451_REG_CTRL_REG2, &reg_ctrl_reg2);
	} while (reg_ctrl_reg2 & 0x40);


	 // enable 4G range
	writeReg8(sensor, MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_4_G);
	  // High res
	writeReg8(sensor, MMA8451_REG_CTRL_REG2, 0x02);
	  // DRDY on INT1
	writeReg8(sensor, MMA8451_REG_CTRL_REG4, 0x01);
	writeReg8(sensor, MMA8451_REG_CTRL_REG5, 0x01);

	  // Turn on orientation config
	writeReg8(sensor,MMA8451_REG_PL_CFG, 0x40);

	  // Activate at max rate, low noise mode
	writeReg8(sensor, MMA8451_REG_CTRL_REG1, 0x01 | 0x04);

	return ESP_OK;
}

esp_err_t MMA8451_setRange(MMA8451 *sensor, mma8451_range_t range)
{
	uint8_t reg1;
	if(readReg8(sensor, MMA8451_REG_CTRL_REG1, &reg1) != ESP_OK)
		return ESP_FAIL;
	writeReg8(sensor, MMA8451_REG_CTRL_REG1, 0x00); // deactivate
	writeReg8(sensor, MMA8451_REG_XYZ_DATA_CFG, range & 0x03);
	writeReg8(sensor, MMA8451_REG_CTRL_REG1, reg1 | 0x01); // activate
	return ESP_OK;
}

mma8451_range_t MMA8451_getRange(MMA8451 *sensor)
{
	mma8451_range_t range = 0;
	readReg8(sensor, MMA8451_REG_XYZ_DATA_CFG, (uint8_t*) &range);
	return (range & 0x03);
}

void MMA8451_read(MMA8451 *sensor)
{
	 // read x y z at once
	  uint8_t buffer[6] = {0, 0, 0, 0, 0, 0};
	  readReg(sensor, MMA8451_REG_OUT_X_MSB, buffer, 6);

	  sensor->_x = buffer[0];
	  sensor->_x <<= 8;
	  sensor->_x |= buffer[1];
	  sensor->_x >>= 2;
	  sensor->_y = buffer[2];
	  sensor->_y <<= 8;
	  sensor->_y |= buffer[3];
	  sensor->_y >>= 2;
	  sensor->_z = buffer[4];
	  sensor->_z <<= 8;
	  sensor->_z |= buffer[5];
	  sensor->_z >>= 2;

	  uint8_t range = MMA8451_getRange(sensor);
	  uint16_t divider = 1;
	  if (range == MMA8451_RANGE_8_G)
	    divider = 1024;
	  if (range == MMA8451_RANGE_4_G)
	    divider = 2048;
	  if (range == MMA8451_RANGE_2_G)
	    divider = 4096;

	  sensor->_xg = (float) (sensor->_x) / divider;
	  sensor->_yg = (float) (sensor->_y) / divider;
	  sensor->_zg = (float) (sensor->_z) / divider;
}

uint8_t MMA8451_getOrientation(MMA8451 *sensor)
{
	uint8_t orientation;
	readReg8(sensor, MMA8451_REG_PL_STATUS, &orientation);
	return (orientation & 0x07);
}

void MMA8451_setDataRate(MMA8451 *sensor, mma8451_dataRate_t dataRate)
{
	uint8_t ctl1;
	readReg8(sensor, MMA8451_REG_CTRL_REG1, &ctl1);
	writeReg8(sensor, MMA8451_REG_CTRL_REG1, 0x00); // deactivate
	ctl1 &= ~(MMA8451_DATARATE_MASK << 3);       // mask off bits
	ctl1 |= (dataRate << 3);
	writeReg8(sensor, MMA8451_REG_CTRL_REG1, ctl1 | 0x01); // activate
}

mma8451_dataRate_t MMA8451_getDataRate(MMA8451 *sensor)
{
	mma8451_dataRate_t dataRate;
	readReg8(sensor, MMA8451_REG_CTRL_REG1, (uint8_t*) &dataRate);
	return ((dataRate >> 3) & MMA8451_DATARATE_MASK);
}


/************ low level data pushing commands **********/
static esp_err_t writeReg8(MMA8451 *sensor, uint8_t _regAddr, uint8_t _data)
{
	uint8_t tx_buff[2];
	tx_buff[0] = _regAddr;
	tx_buff[1] = _data;
	return i2c_master_transmit(sensor->_hi2c, tx_buff, 2, 1000);
}

static esp_err_t writeReg(MMA8451 *sensor, uint8_t _regAddr, uint8_t *_data, uint8_t _len)
{
	if (_len > MAX_TX_BUFFER)
		return ESP_FAIL;
	uint8_t tx_buff[MAX_TX_BUFFER];
	tx_buff[0] = _regAddr;
	memcpy(tx_buff + 1, _data, _len);
	return i2c_master_transmit(sensor->_hi2c, tx_buff, _len + 1, 1000);
}

static esp_err_t readReg8(MMA8451 *sensor, uint8_t _regAddr, uint8_t *_data)
{
	return i2c_master_transmit_receive(sensor->_hi2c,  &_regAddr, 1, _data, 1, 1000);
}

static esp_err_t readReg(MMA8451 *sensor, uint8_t _regAddr, uint8_t *_data, uint8_t _len)
{
	return i2c_master_transmit_receive(sensor->_hi2c,  &_regAddr, 1, _data, _len, 1000);
}