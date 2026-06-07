#ifndef MPU6050_H
#define MPU6050_H
#include "main.h"
#include <stdbool.h>

typedef struct {
    int16_t accelerometer[3];
    int16_t gyroscope[3];
} mpu6050_t;

typedef enum {
    I2C_STATE_IDLE,
    I2C_STATE_READING_MPU6050_DATA,
    I2C_STATE_READ_MPU6050_DATA
} I2C_STATE_t;

extern volatile I2C_STATE_t current_i2c_state;
extern mpu6050_t mpu6050_data;
#define MPU6050_RX_BUFFER_SIZE 14

uint8_t who_am_i(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef mpu6050_wake_device(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef mpu6050_read_data(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef mpu6050_save_data(void);

#endif