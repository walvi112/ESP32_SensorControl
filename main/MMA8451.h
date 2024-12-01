#ifndef MMA8451_H
#define MMA8451_H

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/i2c_master.h"
#include "esp_err.h"

#define SENSORS_GRAVITY_STANDARD	(9.80665F)

//Device address
#define MMA8451_DEFAULT_ADDRESS		(0x1C) //Default MMA8451 I2C address, if A is GND, its 0x1C
#define MMA8451_WHOAMI_ADDRESS		(0x2A) //Depend of board

//Register address
#define MMA8451_REG_OUT_X_MSB 		0x01 //Read-only device output register
#define MMA8451_REG_SYSMOD 			0x0B    //SYSMOD system mode register
#define MMA8451_REG_WHOAMI 			0x0D    //WHO_AM_I device ID register
#define MMA8451_REG_XYZ_DATA_CFG	0x0E //XYZ_DATA_CFG register, sets dynamic range and high-pass filter for output data

#define MMA8451_REG_PL_STATUS      	0x10 //PL_STATUS portrait/landscape status register

#define MMA8451_REG_PL_CFG 			0x11 //Portrait/landscape configuration register
#define MMA8451_REG_CTRL_REG1 		0x2A //CTRL_REG1 system control 1 register
#define MMA8451_REG_CTRL_REG2 		0x2B //CTRL_REG2 system control 2 register
#define MMA8451_REG_CTRL_REG4 		0x2D //CTRL_REG4 system control 4 register
#define MMA8451_REG_CTRL_REG5 		0x2E //CTRL_REG5 system control 5 register


//Different portrait and landscape settings
#define MMA8451_PL_PUF 0
#define MMA8451_PL_PUB 1
#define MMA8451_PL_PDF 2
#define MMA8451_PL_PDB 3
#define MMA8451_PL_LRF 4
#define MMA8451_PL_LRB 5
#define MMA8451_PL_LLF 6
#define MMA8451_PL_LLB 7

//Different range settings
typedef enum mma8451_range_t{
  MMA8451_RANGE_2_G = 0x00,  // +/- 2g (default value)
  MMA8451_RANGE_4_G = 0x01, // +/- 4g
  MMA8451_RANGE_8_G = 0x02, // +/- 8g
} mma8451_range_t;

//Used with register 0x2A (MMA8451_REG_CTRL_REG1) to set bandwidth
typedef enum mma8451_dataRate_t{
  MMA8451_DATARATE_800_HZ = 0x00,  //  800Hz
  MMA8451_DATARATE_400_HZ = 0x01,  //  400Hz
  MMA8451_DATARATE_200_HZ = 0x02,  //  200Hz
  MMA8451_DATARATE_100_HZ = 0x03,  //  100Hz
  MMA8451_DATARATE_50_HZ = 0x04,   //   50Hz
  MMA8451_DATARATE_12_5_HZ = 0x05, // 12.5Hz
  MMA8451_DATARATE_6_25HZ = 0x06,  // 6.25Hz
  MMA8451_DATARATE_1_56_HZ = 0x07, // 1.56Hz

  MMA8451_DATARATE_MASK = 0x07
} mma8451_dataRate_t;

typedef struct MMA8451
{
	uint8_t _Addr;
	int16_t _x;
	int16_t _y;
	int16_t _z;
	float _xg;
	float _yg;
	float _zg;
	i2c_master_dev_handle_t _hi2c;
}MMA8451;

esp_err_t MMA8451_Init(MMA8451 *sensor, i2c_master_dev_handle_t _hi2c, uint8_t _addr);
esp_err_t MMA8451_setRange(MMA8451 *sensor, mma8451_range_t range);
void MMA8451_read(MMA8451 *sensor);
void MMA8451_setDataRate(MMA8451 *sensor, mma8451_dataRate_t dataRate);
uint8_t MMA8451_getOrientation(MMA8451 *sensor);
mma8451_range_t MMA8451_getRange(MMA8451 *sensor);
mma8451_dataRate_t MMA8451_getDataRate(MMA8451 *sensor);



#ifdef __cplusplus
}
#endif

#endif