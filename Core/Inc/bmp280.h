/*
 * bmp280.h
 *
 *  Created on: Feb 13, 2025
 *      Author: Davut
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"


#define SEA_LEVEL_PRESSURE          (1013.25f)
//sd0 pini low ise i2c BMP280 addresi sensor 0x77 eger sd0 pini high ise i2c BMP280 addresi  
#define BMP280_I2C_ADDR             0x76

//alttaki adresi okuyunca onun altindaki degeri donmeli
#define BMP280_REG_ID               0xD0
#define BMP280_REG_ID_VAL           0x58


//reset tusu asagida degeri yazarsan power on stateye resetlenir
#define BMP280_REG_SOFTRESET        0xE0
#define BMP280_REG_SOFTRESET_VAL    0xB6
//status conversiondayken 0x08 registere copy olduysa 0x00 gerekli degil gibi
#define BMP280_REG_STATUS           0xF3

//bit 7-5 sicaklik oversampling bit 4-2 basinc oversampling bit 1-0 mode bitleri asagidaki enumlar ile ayarlanir
#define BMP280_REG_CTRL_MEAS        0xF4

//bit 7-6 t_standby bit normal modda bot 4-2 iir filtresi icin time constant bit 0 spi enable bitleri 1 ise spi aciliyor
#define BMP280_REG_CONFIG           0xF5

//basinc raw datasinin msb lerinin saklandigi register bir sonraki register 0xF8 lsb leri iceriyor
#define BMP280_REG_PRESS_MSB        0xF7
//temp raw datasinin msb lerinin saklandigi register bir sonraki register 0xFB lsb leri iceriyor
#define BMP280_REG_TEMP_MSB         0xFA




typedef enum {
    BMP280_MODE_SLEEP = 0x00,
    BMP280_MODE_FORCED = 0x01,
    BMP280_MODE_NORMAL = 0x03
} bmp280_mode_t;

// iki oversampling icin de asagidaki degerler ilgili registere yazilmamli
typedef enum {
    BMP280_OVERSAMPLING_0=0x00,
    BMP280_OVERSAMPLING_1=0x01,
    BMP280_OVERSAMPLING_2=0x02,
    BMP280_OVERSAMPLING_4=0x03,
    BMP280_OVERSAMPLING_8=0x04,
    BMP280_OVERSAMPLING_16=0x05,
} bmp280_oversampling_mode;

// iir filtre icin
typedef enum {
    BMP280_FILTER_OFF      = 0,
    BMP280_FILTER_2X       = 1,
    BMP280_FILTER_4X       = 2,
    BMP280_FILTER_8X       = 3,
    BMP280_FILTER_16X      = 4
} BMP280_Filter;

// Standby Time
typedef enum {
    BMP280_STANDBY_0_5MS   = 0, // 0.5 ms
    BMP280_STANDBY_62_5MS  = 1, // 62.5 ms
    BMP280_STANDBY_125MS   = 2, // 125 ms
    BMP280_STANDBY_250MS   = 3, // 250 ms
    BMP280_STANDBY_500MS   = 4, // 500 ms
    BMP280_STANDBY_1000MS  = 5, // 1000 ms
    BMP280_STANDBY_2000MS  = 6, // 2000 ms
    BMP280_STANDBY_4000MS  = 7  // 4000 ms
} BMP280_StandbyTime;





typedef struct {
    uint16_t dig_T1;  ///< Calibration data for temperature (parameter 1)
    int16_t  dig_T2;  ///< Calibration data for temperature (parameter 2)
    int16_t  dig_T3;  ///< Calibration data for temperature (parameter 3)
    uint16_t dig_P1;  ///< Calibration data for pressure (parameter 1)
    int16_t  dig_P2;  ///< Calibration data for pressure (parameter 2)
    int16_t  dig_P3;  ///< Calibration data for pressure (parameter 3)
    int16_t  dig_P4;  ///< Calibration data for pressure (parameter 4)
    int16_t  dig_P5;  ///< Calibration data for pressure (parameter 5)
    int16_t  dig_P6;  ///< Calibration data for pressure (parameter 6)
    int16_t  dig_P7;  ///< Calibration data for pressure (parameter 7)
    int16_t  dig_P8;  ///< Calibration data for pressure (parameter 8)
    int16_t  dig_P9;  ///< Calibration data for pressure (parameter 9)
    uint8_t  dig_H1;  ///< Calibration data for humidity (parameter 1) - BMP280 doesn't have humidity but struct from common Bosch library might include it
    int16_t  dig_H2;  ///< Calibration data for humidity (parameter 2)
    uint8_t  dig_H3;  ///< Calibration data for humidity (parameter 3)
    int16_t  dig_H4;  ///< Calibration data for humidity (parameter 4)
    int16_t  dig_H5;  ///< Calibration data for humidity (parameter 5)
    int8_t   dig_H6;  ///< Calibration data for humidity (parameter 6)
}BMP280_Calib_Data;


typedef struct 
{
    int16_t temperature;
    int16_t pressure;
    int16_t altitude;

    int16_t temperature_kalman;
    int16_t pressure_kalman;
    int16_t altitude_kalman;

    BMP280_Calib_Data calib_data;
} BMP280_Data;



void BMP280_Reset();
HAL_StatusTypeDef Read_Calibration_Data(I2C_HandleTypeDef *hi2c,BMP280_Data *dev);
HAL_StatusTypeDef BMP280_Config(I2C_HandleTypeDef *hi2c, bmp280_oversampling_mode pressure_os, bmp280_oversampling_mode temp_os,bmp280_mode_t mode);
HAL_StatusTypeDef BMP280_Init(I2C_HandleTypeDef *hi2c,BMP280_Data *bmp280_data);
HAL_StatusTypeDef BMP280_Read_Raw_Data(I2C_HandleTypeDef *hi2c,int32_t *raw_pressure, int32_t *raw_temperature);

HAL_StatusTypeDef BMP280_Read_Data(I2C_HandleTypeDef *hi2c,BMP280_Data *bmp280_data);

float BMP280_Compensate_Temperature(int32_t rawTemperature, int32_t *t_fine,BMP280_Data *dev);

float BMP280_compensate_Pressure(int32_t rawPressure, int32_t t_fine,BMP280_Data *dev);


HAL_StatusTypeDef BMP280_Set_Filter(I2C_HandleTypeDef *hi2c, BMP280_Filter filter);
HAL_StatusTypeDef BMP280_Set_Standby_Time(I2C_HandleTypeDef *hi2c, BMP280_StandbyTime standbyTime);
#endif /* INC_BMP280_H_ */
