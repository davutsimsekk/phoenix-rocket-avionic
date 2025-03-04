/*
 * bmp280.c
 *
 *  Created on: Feb 13, 2025
 *      Author: Davut
 */


#include "bmp280.h"
#include "math.h"


HAL_StatusTypeDef BMP280_Init(I2C_HandleTypeDef *hi2c,BMP280_Data *bmp280_data){
	uint8_t chip_id;


    // device id kontrol et
	HAL_StatusTypeDef status=HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR, BMP280_REG_ID, I2C_MEMADD_SIZE_8BIT, &chip_id, 1, 100);
    if (status!=HAL_OK) return status;
    if (chip_id!=BMP280_REG_ID_VAL) return HAL_ERROR;
    // soft reset yap
    uint8_t reset_cmd=BMP280_REG_SOFTRESET_VAL;
    status=HAL_I2C_Mem_Write(hi2c, BMP280_I2C_ADDR, BMP280_REG_SOFTRESET, I2C_MEMADD_SIZE_8BIT, &reset_cmd, 1, 100);
    if (status!=HAL_OK) return status;
    HAL_Delay(10); //reset icin bekle (min 2ms)
    
    //calibration data oku
    status=BMP280_Read_Calibration_Data(hi2c,bmp280_data);
    if (status!=HAL_OK) return status;

    //default oversampling ve power mode ayarla 
    status=BMP280_Config(hi2c,BMP280_OVERSAMPLING_16,BMP280_OVERSAMPLING_1,BMP280_MODE_NORMAL);
    if (status!=HAL_OK) return status;
    //0.5 ms standby
    status=BMP280_Set_Standby_Time(hi2c, BMP280_STANDBY_0_5MS);
    if (status!=HAL_OK) return status;

    //16x filtre degistirilebilir
    status=BMP280_Set_Filter(hi2c, BMP280_FILTER_16X);

    return status;

}

HAL_StatusTypeDef BMP280_Reset(I2C_HandleTypeDef *hi2c) {
    uint8_t reset_cmd = BMP280_REG_SOFTRESET_VAL;
    return HAL_I2C_Mem_Write(hi2c, BMP280_I2C_ADDR, BMP280_REG_SOFTRESET, I2C_MEMADD_SIZE_8BIT, &reset_cmd, 1, 100);
}

HAL_StatusTypeDef BMP280_Read_Calibration_Data(I2C_HandleTypeDef *hi2c,BMP280_Data *dev){
    uint8_t calib_data_buffer[24];
    if (HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR, BMP280_REG_CALIB_START, I2C_MEMADD_SIZE_8BIT, calib_data_buffer, 24, 100)!=HAL_OK)
    {
        return HAL_ERROR;
    }
    
    dev->calib_data.dig_T1 = (uint16_t)calib_data_buffer[0] | ((uint16_t)calib_data_buffer[1] << 8);
    dev->calib_data.dig_T2 = (int16_t)((uint16_t)calib_data_buffer[2] | ((uint16_t)calib_data_buffer[3] << 8));
    dev->calib_data.dig_T3 = (int16_t)((uint16_t)calib_data_buffer[4] | ((uint16_t)calib_data_buffer[5] << 8));

    // Pressure calibration parameters
    dev->calib_data.dig_P1 = (uint16_t)calib_data_buffer[6] | ((uint16_t)calib_data_buffer[7] << 8);
    dev->calib_data.dig_P2 = (int16_t)((uint16_t)calib_data_buffer[8] | ((uint16_t)calib_data_buffer[9] << 8));
    dev->calib_data.dig_P3 = (int16_t)((uint16_t)calib_data_buffer[10] | ((uint16_t)calib_data_buffer[11] << 8));
    dev->calib_data.dig_P4 = (int16_t)((uint16_t)calib_data_buffer[12] | ((uint16_t)calib_data_buffer[13] << 8));
    dev->calib_data.dig_P5 = (int16_t)((uint16_t)calib_data_buffer[14] | ((uint16_t)calib_data_buffer[15] << 8));
    dev->calib_data.dig_P6 = (int16_t)((uint16_t)calib_data_buffer[16] | ((uint16_t)calib_data_buffer[17] << 8));
    dev->calib_data.dig_P7 = (int16_t)((uint16_t)calib_data_buffer[18] | ((uint16_t)calib_data_buffer[19] << 8));
    dev->calib_data.dig_P8 = (int16_t)((uint16_t)calib_data_buffer[20] | ((uint16_t)calib_data_buffer[21] << 8));
    dev->calib_data.dig_P9 = (int16_t)((uint16_t)calib_data_buffer[22] | ((uint16_t)calib_data_buffer[23] << 8));

    return HAL_OK;
    
}

HAL_StatusTypeDef BMP280_Config(I2C_HandleTypeDef *hi2c, bmp280_oversampling_mode pressure_os, bmp280_oversampling_mode temp_os,bmp280_mode_t mode){
    uint8_t ctrl_meas_reg;
    HAL_StatusTypeDef status= HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR, BMP280_REG_CTRL_MEAS, I2C_MEMADD_SIZE_8BIT, &ctrl_meas_reg, 1, 100);
    if (status!=HAL_OK) {
		return status;
	}
    //sifirla ve yeni degeri gir 0x3F=0011 1111 (63)
    ctrl_meas_reg &= ~0x3F;
    ctrl_meas_reg |= (temp_os << 5) | (pressure_os<< 2)|mode;
    status=HAL_I2C_Mem_Write(hi2c, BMP280_I2C_ADDR, BMP280_REG_CTRL_MEAS, I2C_MEMADD_SIZE_8BIT, &ctrl_meas_reg, 1, 100);
    return status;
}

HAL_StatusTypeDef BMP280_Set_Filter(I2C_HandleTypeDef *hi2c, BMP280_Filter filter){
    uint8_t config_reg;
    HAL_StatusTypeDef status= HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR, BMP280_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, &config_reg, 1, 100);
    if (status!=HAL_OK) return status;
    config_reg &= ~0x1C; // 0001 1100(2-4) bitleri sifirla
    config_reg |= (filter << 2);
    status=HAL_I2C_Mem_Write(hi2c, BMP280_I2C_ADDR, BMP280_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, &config_reg, 1, 100);
    return status;
}

HAL_StatusTypeDef BMP280_Set_Standby_Time(I2C_HandleTypeDef *hi2c, BMP280_StandbyTime standbyTime){
    uint8_t config_reg;
    HAL_StatusTypeDef status= HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR, BMP280_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, &config_reg, 1, 100);
    if (status!=HAL_OK) return status;
    config_reg &= ~0xE0; // 1110 0000(5-7) bitleri sifirla
    config_reg |= (standbyTime << 5);
    status=HAL_I2C_Mem_Write(hi2c, BMP280_I2C_ADDR, BMP280_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, &config_reg, 1, 100);
    return status;
}



HAL_StatusTypeDef BMP280_Read_Raw_Data(I2C_HandleTypeDef *hi2c,int32_t *raw_pressure, int32_t *raw_temperature){
    uint8_t data_buffer[6];
    
    HAL_StatusTypeDef status= HAL_I2C_Mem_Read(hi2c, BMP280_I2C_ADDR, BMP280_REG_PRESS_MSB, I2C_MEMADD_SIZE_8BIT, data_buffer, 6, 100);
    if (status!=HAL_OK) return status;
    *raw_pressure = (int32_t)data_buffer[0] << 12 | (int32_t)data_buffer[1] << 4 | (int32_t)data_buffer[2] >> 4;
    *raw_temperature = (int32_t)data_buffer[3] << 12 | (int32_t)data_buffer[4] << 4 | (int32_t)data_buffer[5] >> 4;
    return status;
}

// asagidaki iki formul datasheeten

// --- Compensate Temperature ---
float BMP280_Compensate_Temperature(int32_t rawTemperature, int32_t *t_fine,BMP280_Data *dev) {
    int32_t var1, var2;
    var1 = ((((rawTemperature >> 3) - ((int32_t)dev->calib_data.dig_T1 << 1))) * ((int32_t)dev->calib_data.dig_T2)) >> 11;
    var2 = (((((rawTemperature >> 4) - ((int32_t)dev->calib_data.dig_T1)) * ((rawTemperature >> 4) - ((int32_t)dev->calib_data.dig_T1))) >> 12) * ((int32_t)dev->calib_data.dig_T3)) >> 14;
    *t_fine = var1 + var2;
    return (float)((( *t_fine) * 5 + 128) >> 8) / 100.0f;
}

// --- Compensate Pressure ---
float BMP280_Compensate_Pressure(int32_t rawPressure, int32_t t_fine,BMP280_Data *dev) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dev->calib_data.dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->calib_data.dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->calib_data.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->calib_data.dig_P3) >> 8) + ((var1 * (int64_t)dev->calib_data.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dev->calib_data.dig_P1) >> 33;

    if (var1 == 0) {
        return 0; // Avoid division by zero
    }
    p = 1048576 - rawPressure;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dev->calib_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->calib_data.dig_P8) * p * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calib_data.dig_P7) << 4);
    return (float)p / 256.0f; // Pressure in Pa
}

HAL_StatusTypeDef BMP280_Read_Data(I2C_HandleTypeDef *hi2c,BMP280_Data *bmp280_data){
    HAL_StatusTypeDef status;
    //t fine ara deger gibi bir seymis
    int32_t raw_pressure, raw_temperature, t_fine;
    float compensated_pressure, compensated_temperature;

    status = BMP280_Read_Raw_Data(hi2c, &raw_pressure, &raw_temperature);
    if (status != HAL_OK)
    {
        return status;
    }

    //formullerden gecir
    compensated_temperature = BMP280_Compensate_Temperature(raw_temperature, &t_fine,bmp280_data);
    compensated_pressure = BMP280_Compensate_Pressure(raw_pressure, t_fine,bmp280_data);
    //basincdan yukseklik hesabi
    float altitude = 44330 * (1.0 - pow(((compensated_pressure / 100.0f) / SEA_LEVEL_PRESSURE), 0.190295)); 
    bmp280_data->temperature = compensated_temperature;
    bmp280_data->pressure = compensated_pressure;
    bmp280_data->altitude = altitude;
    return HAL_OK;
}
