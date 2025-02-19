/*
 * mpu9250.h
 *
 *  Created on: Feb 12, 2025
 *      Author: Davut
 */

#ifndef INC_MPU9250_H_
#define INC_MPU9250_H_


#include "stm32f1xx_hal.h"

#define MPU9250_I2C_ADDR	0x68<<1
#define AK8963_I2C_ADDR		0x0C<<1

//MPU9250 Registers




//hangi cihaza eristigimizin adresi 0x68 okursak donmeli(read only)
#define WHO_AM_I			0x75

// sensorlerin clocklarini bolmeye yarayan register( Sample Rate Divider)
#define SMPLRT_DIV			0x19
//disaridan clock ayari var digital low pass filtresi(dlpf) var noise reduction icin SMPLRT_DIV ile beraber calisiyor
#define	MPU9250_CONFIG		0x1A
//self test bitleri var 3-4. bitler scale rangei ayarliyor bu range hizli mi donecegine yoksa yavas mi donecegine gore secilmeli
//0-1 pinleri dlpf'yi aktif edip etmemekle alakali ama reserved gosteriyor
#define GYRO_CONFIG			0x1B
// yine 7-5 bitleri self testler icin ayrilmis prodda bunlar kapatilmali 4-3 pinleri range selection bitleri burada yuksek hizlanma mi var ona gore secilmeli

// bu ayara bakilmali duruma gore 11 (16g) kullanilabilir biz accelerationu apogee noktasinda olceceksek dusuk g daha iyi ama normal olcum icin yuksek g daha islevsel

#define ACCEL_CONFIG		0x1C

//burasi datalarin basladigi yer bundan sonra 14 adreste once accel sonra temp sonra gyro olmak uzere veriler var veriler 16 bit oldugu icin 8-8 ikiye bolunmus birlestirmek lazim
#define ACCEL_XOUT_H		0x3B

//sadece temp okumak istersek adresi ama usttekinden sonra ardisik geliyor zaten
#define TEMP_OUT_H			0x41

//ayni mantik gyro icin
#define GYRO_XOUT_H			0x43

// bit 7 reset biti her seyi sifirliyor 6. bit sleep mode 5. bit cycle sleep ile ilgili bir sey low-power consumption 4. bit reserved 2-0 CLKSEL sensorun internal circuiti icin clock secmeye yariyormus 001 secilebilir arastirmak lazim
#define PWR_MGMT_1			0x6B



//AK8963
#define AK8963_CNTL1    0x0A
#define AK8963_CNTL2    0x0B
#define AK8963_ASAX     0x10
#define AK8963_ST1      0x02
#define AK8963_HXL      0x03


typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temp;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;

    int16_t kalman_accel_x;
    int16_t kalman_accel_y;
    int16_t kalman_accel_z;
    int16_t kalman_gyro_x;
    int16_t kalman_gyro_y;
    int16_t kalman_gyro_z;
}MPU9250_Data;


HAL_StatusTypeDef MPU9250_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef MPU9250_Read_All(I2C_HandleTypeDef *hi2c, MPU9250_Data *data);


#endif /* INC_MPU9250_H_ */
