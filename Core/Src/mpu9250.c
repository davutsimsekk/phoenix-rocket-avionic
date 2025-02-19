/*
 * mpu9250.c
 *
 *  Created on: Feb 12, 2025
 *      Author: Davut
 */


#include "mpu9250.h"

//magnometer baslatmak icin bizim projede lazim degil gibi gpt den alindi
static HAL_StatusTypeDef AK8963_Init(I2C_HandleTypeDef *hi2c);

// burada ileride diger ayarlari da yapmamiz lazim
HAL_StatusTypeDef MPU9250_Init(I2C_HandleTypeDef *hi2c){
	//cihazi kaldirmak icin wake up
	uint8_t data=0x00;
	HAL_StatusTypeDef status=HAL_I2C_Mem_Write(hi2c,MPU9250_I2C_ADDR ,PWR_MGMT_1 , 1 , &data, 1, 100);
	if (status!=HAL_OK){
		return status;
	}
	//accelerometerin rangesi +16g olarak ayarlandi degistirlebilir
	data=0x18;
	//decimal 24 e denk geliyor
	status = HAL_I2C_Mem_Write(hi2c, MPU9250_I2C_ADDR, ACCEL_CONFIG, 1, &data, 1, 100);
	if(status!=HAL_OK){
		return status;
	}

	//jiroskop rangesi 2000dps olarak ayarlandi bu ayara tekrar bakmali

	data = 0x18;
	//(24)
	status = HAL_I2C_Mem_Write(hi2c, MPU9250_I2C_ADDR, GYRO_CONFIG, 1, &data, 1, 100);
	if(status!=HAL_OK){
		return status;
	}

	return AK8963_Init(hi2c);


}

HAL_StatusTypeDef MPU9250_Read_All(I2C_HandleTypeDef *hi2c,MPU9250_Data *data){
	uint8_t buffer[14];
	HAL_StatusTypeDef status;

	//accelerometer sicaklik ve jiroskopu okuma
	status = HAL_I2C_Mem_Read(hi2c, MPU9250_I2C_ADDR, ACCEL_XOUT_H, 1, buffer, 14, 100);

	if(status!=HAL_OK) {
		return status;
	}

	data->accel_x = (buffer[0]<<8) | buffer[1];
	data->accel_y = (buffer[2]<<8) | buffer[3];
	data->accel_z = (buffer[4]<<8) | buffer[5];
    data->temp = (buffer[6] << 8) | buffer[7];
    data->gyro_x = (buffer[8] << 8) | buffer[9];
    data->gyro_y = (buffer[10] << 8) | buffer[11];
    data->gyro_z = (buffer[12] << 8) | buffer[13];

    //magnometeri oku sonra yapilabilir
    return HAL_OK;
}

//gpten alindi
static HAL_StatusTypeDef AK8963_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t data;

    // Check WHO_AM_I
    HAL_I2C_Mem_Read(hi2c, AK8963_I2C_ADDR, 0x00, 1, &data, 1, 100);
    if (data != 0x48) return HAL_ERROR;

    // Set power down mode
    data = 0x00;
    HAL_I2C_Mem_Write(hi2c, AK8963_I2C_ADDR, AK8963_CNTL1, 1, &data, 1, 100);
    HAL_Delay(100);

    // Set Fuse ROM access
    data = 0x0F;
    HAL_I2C_Mem_Write(hi2c, AK8963_I2C_ADDR, AK8963_CNTL1, 1, &data, 1, 100);
    HAL_Delay(100);

    // Read sensitivity adjustments
    uint8_t asa[3];
    HAL_I2C_Mem_Read(hi2c, AK8963_I2C_ADDR, AK8963_ASAX, 1, asa, 3, 100);

    // Set 16-bit output and continuous measurement mode 2
    data = 0x16;
    HAL_I2C_Mem_Write(hi2c, AK8963_I2C_ADDR, AK8963_CNTL1, 1, &data, 1, 100);

    return HAL_OK;
}


