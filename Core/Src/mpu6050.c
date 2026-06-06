#include "mpu6050.h"

uint8_t who_am_i(I2C_HandleTypeDef *hi2c){
    assert_param(hi2c);

    uint8_t output = 0;
    uint16_t dev_addr = 0x68 << 1; 
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, dev_addr, 0x75, I2C_MEMADD_SIZE_8BIT, &output, 1, 100);
    if (status == HAL_OK) {
        return output;
    }
    else {
        return 0;
    }
}