#include "six-axis-calculation.h"
#include <math.h>

void compute_angles(mpu6050_t *input) {
    float accelerometer_x_g = input->accelerometer[0] * ACCELEROMETER_SENSITIVITY;
    float accelerometer_y_g = input->accelerometer[1] * ACCELEROMETER_SENSITIVITY;
    float accelerometer_z_g = input->accelerometer[2] * ACCELEROMETER_SENSITIVITY;

}