#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define I2C_SCL_PIN 10
#define I2C_SDA_PIN 11

#define QMI8658_ADDR 0x6B  // Default I2C address

// Registers
#define QMI8658_CTRL1 0x7E
#define QMI8658_CTRL2 0x40
#define QMI8658_CTRL3 0x42
#define QMI8658_CTRL7 0x4E
#define QMI8658_RESET 0x14
#define QMI8658_WHO_AM_I 0x00
#define QMI8658_REVISION 0x01

#define QMI8658_ACC_X_L 0x12
#define QMI8658_ACC_X_H 0x13
#define QMI8658_ACC_Y_L 0x14
#define QMI8658_ACC_Y_H 0x15
#define QMI8658_ACC_Z_L 0x16
#define QMI8658_ACC_Z_H 0x17

#define QMI8658_GYR_X_L 0x18
#define QMI8658_GYR_X_H 0x19
#define QMI8658_GYR_Y_L 0x1A
#define QMI8658_GYR_Y_H 0x1B
#define QMI8658_GYR_Z_L 0x1C
#define QMI8658_GYR_Z_H 0x1D

#define QMI8658_TEMP_L 0x1E
#define QMI8658_TEMP_H 0x1F

#define TEMPERATURE_SENSOR_RESOLUTION 100.0f

// Accelerometer scale
typedef enum {
    ACC_SCALE_2G = 0,
    ACC_SCALE_4G,
    ACC_SCALE_8G,
    ACC_SCALE_16G
} acc_scale_t;

// Gyroscope scale
typedef enum {
    GYRO_SCALE_16DPS = 0,
    GYRO_SCALE_32DPS,
    GYRO_SCALE_64DPS,
    GYRO_SCALE_128DPS,
    GYRO_SCALE_256DPS,
    GYRO_SCALE_512DPS,
    GYRO_SCALE_1024DPS,
    GYRO_SCALE_2048DPS
} gyro_scale_t;

// Output Data Rate (ODR)
typedef enum {
    ACC_ODR_100HZ = 0,
    ACC_ODR_200HZ,
    ACC_ODR_400HZ,
    ACC_ODR_800HZ
} acc_odr_t;

typedef enum {
    GYRO_ODR_100HZ = 0,
    GYRO_ODR_200HZ,
    GYRO_ODR_400HZ,
    GYRO_ODR_800HZ
} gyro_odr_t;

typedef enum {
    QMI_MODE_NORMAL = 0x00,
    QMI_MODE_ACC_ONLY = 0x01,
    QMI_MODE_GYRO_ONLY = 0x02
} qmi8658_mode_t;

typedef struct {
    float x;
    float y;
    float z;
} imu_data_t;

typedef struct {
    imu_data_t acc_xyz;
    imu_data_t gyro_xyz;
    float temperature;
} qmi_data_t;

typedef struct {
    uint8_t device_addr;
    uint32_t i2c_freq;

    uint8_t acc_scale;
    uint16_t acc_sensitivity;
    uint8_t gyro_scale;
    uint16_t gyro_sensitivity;
} qmi_ctx_t;

typedef enum {
    QMI_RESULT_OK,
    QMI_RESULT_ERROR
} qmi_result_t;

// Initialization and I2C
void qmi_init(qmi_ctx_t *ctx, uint8_t device_addr, uint32_t i2c_freq);
qmi_result_t qmi_reset(qmi_ctx_t *ctx);
qmi_result_t qmi_set_mode(qmi_ctx_t *ctx, qmi8658_mode_t mode);
qmi_result_t qmi_read(qmi_ctx_t *ctx, qmi_data_t *data);

// Accelerometer / Gyroscope config
qmi_result_t qmi_acc_set_scale(qmi_ctx_t *ctx, acc_scale_t scale);
qmi_result_t qmi_acc_set_odr(qmi_ctx_t *ctx, acc_odr_t odr);
qmi_result_t qmi_gyro_set_scale(qmi_ctx_t *ctx, gyro_scale_t scale);
qmi_result_t qmi_gyro_set_odr(qmi_ctx_t *ctx, gyro_odr_t odr);

void imu_task(void *arg);


void init_qmi8658(void);