#include "mpu6050.h"
#include "stm32f1xx_hal_def.h"

#define MPU6050_I2C_ADDRESS (0x68 << 1)
// MPU6050_REG_ACCEL is located at the accelerometer register, but I'll read the next 14 bytes in one go.
// 0x3B-0x40 = accelerometers, 
// 0x41-0x42 = temp (this'll be ignored)
// 0x43-0x48 = gyroscope
#define MPU6050_REG_ACCEL 0x3B
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_PWR_MGMT_2 0x6C
#define MPU6050_REG_WHO_AM_I 0x75

static uint8_t rx_buffer[MPU6050_RX_BUFFER_SIZE] = {0};
mpu6050_t mpu6050_data = {0};

volatile I2C_STATE_t current_i2c_state = I2C_STATE_IDLE;

uint8_t who_am_i(I2C_HandleTypeDef *hi2c){
    assert_param(hi2c);

    if (hi2c == NULL) return 0;

    uint8_t output = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, MPU6050_I2C_ADDRESS, MPU6050_REG_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &output, 1, 100);
    if (status == HAL_OK) {
        return output;
    }
    else {
        return 0;
    }
}

HAL_StatusTypeDef mpu6050_wake_device(I2C_HandleTypeDef *hi2c) {
    assert_param(hi2c);

    if (hi2c == NULL) return HAL_ERROR;

    uint8_t reset_bit = 0x80;
    uint8_t wake_bit = 0x0;
    if (HAL_I2C_Mem_Write(hi2c, MPU6050_I2C_ADDRESS, MPU6050_REG_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &reset_bit, 1, 100) != HAL_OK) return HAL_ERROR;
    HAL_Delay(100); // give time for the sensor to restart
    if (HAL_I2C_Mem_Write(hi2c, MPU6050_I2C_ADDRESS, MPU6050_REG_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &wake_bit, 1, 100) != HAL_OK) return HAL_ERROR;
    HAL_Delay(100); // give time for the accelerometer to read

    return HAL_OK;
}

HAL_StatusTypeDef mpu6050_read_data(I2C_HandleTypeDef *hi2c) {
    assert_param(hi2c);

    if (hi2c == NULL) return HAL_ERROR;

    if (current_i2c_state == I2C_STATE_IDLE) {
        if (HAL_I2C_Mem_Read_IT(hi2c, MPU6050_I2C_ADDRESS, MPU6050_REG_ACCEL, I2C_MEMADD_SIZE_8BIT, rx_buffer, MPU6050_RX_BUFFER_SIZE) == HAL_OK){
            current_i2c_state = I2C_STATE_READING_MPU6050_DATA;
        }
    }
    else {
        return HAL_BUSY;
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef mpu6050_save_data(void) {
    // extremely important that the sensor data does not get jumbled from HAL_I2C_MemRxCpltCallback
    __disable_irq();
    mpu6050_data.accelerometer[0] = (int16_t) ((rx_buffer[0] << 8) | rx_buffer[1]);
    mpu6050_data.accelerometer[1] = (int16_t) ((rx_buffer[2] << 8) | rx_buffer[3]);
    mpu6050_data.accelerometer[2] = (int16_t) ((rx_buffer[4] << 8) | rx_buffer[5]);

    mpu6050_data.gyroscope[0] = (int16_t) ((rx_buffer[8] << 8) | rx_buffer[9]);
    mpu6050_data.gyroscope[1] = (int16_t) ((rx_buffer[10] << 8) | rx_buffer[11]);
    mpu6050_data.gyroscope[2] = (int16_t) ((rx_buffer[12] << 8) | rx_buffer[13]);
    __enable_irq();

    current_i2c_state = I2C_STATE_IDLE;

    return HAL_OK;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (current_i2c_state == I2C_STATE_READING_MPU6050_DATA) {
        current_i2c_state = I2C_STATE_READ_MPU6050_DATA;
    }
}