#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "lcd_touch.h"
#include "ui/screens.h"
#include "widgets/lv_label.h"
#include "qmi8658c.h"

// I2C configuration
#define I2C_MASTER_SCL 10
#define I2C_MASTER_SDA 11
#define I2C_MASTER_NUM I2C_NUM_0
#define QMI8658_ADDRESS 0x6B
#define TEMPERATURE_SENSOR_RESOLUTION 100.0f // adjust per datasheet

// QMI8658 Registers
#define QMI8658_REG_RESET 0x60
#define QMI8658_REG_CTRL2 0x03
#define QMI8658_REG_CTRL3 0x04
#define QMI8658_REG_CTRL7 0x08
#define QMI8658_REG_WHOAMI 0x00
#define QMI8658_REG_AX_L 0x35
#define QMI8658_REG_AX_H 0x36
#define QMI8658_REG_AY_L 0x37
#define QMI8658_REG_AY_H 0x38
#define QMI8658_REG_AZ_L 0x39
#define QMI8658_REG_AZ_H 0x3A
#define QMI8658_REG_GX_L 0x3B
#define QMI8658_REG_GX_H 0x3C
#define QMI8658_REG_GY_L 0x3D
#define QMI8658_REG_GY_H 0x3E
#define QMI8658_REG_GZ_L 0x3F
#define QMI8658_REG_GZ_H 0x40
#define QMI8658_REG_TEMPEARTURE_L 0x33
#define QMI8658_REG_TEMPEARTURE_H 0x34

// Sensitivity tables
static const uint16_t acc_sens_table[4]  = {16384, 8192, 4096, 2048};
static const uint16_t gyro_sens_table[8] = {2048, 1024, 512, 256, 128, 64, 32, 16};

// I2C helpers
static esp_err_t i2c_write_byte(uint8_t device_addr, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    return i2c_master_write_to_device(I2C_MASTER_NUM, device_addr, data, 2, 1000 / portTICK_PERIOD_MS);
}

static esp_err_t i2c_read_byte(uint8_t device_addr, uint8_t reg, uint8_t *value) {
    return i2c_master_write_read_device(I2C_MASTER_NUM, device_addr, &reg, 1, value, 1, 1000 / portTICK_PERIOD_MS);
}

// Initialize sensor context
void qmi_init(qmi_ctx_t *ctx, uint8_t device_addr, uint32_t i2c_freq) {
    ctx->device_addr = device_addr;
    ctx->i2c_freq = i2c_freq;
    ctx->acc_scale = 0; // ACC_RANGE_2G
    ctx->acc_sensitivity = acc_sens_table[0];
    ctx->gyro_scale = 0; // GYR_RANGE_16DPS
    ctx->gyro_sensitivity = gyro_sens_table[0];

/*    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA,
        .scl_io_num = I2C_MASTER_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_freq
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);*/
}

// Reset sensor
qmi_result_t qmi_reset(qmi_ctx_t *ctx) {
    return i2c_write_byte(ctx->device_addr, QMI8658_REG_RESET, 0xB0) == ESP_OK ? QMI_RESULT_OK : QMI_RESULT_ERROR;
}

// Set accelerometer scale
qmi_result_t qmi_acc_set_scale(qmi_ctx_t *ctx, uint8_t scale) {
    uint8_t ctrl2;
    if(i2c_read_byte(ctx->device_addr, QMI8658_REG_CTRL2, &ctrl2) != ESP_OK) return QMI_RESULT_ERROR;
    ctrl2 = (ctrl2 & 0x8F) | (scale << 4);
    ctx->acc_scale = scale;
    ctx->acc_sensitivity = acc_sens_table[scale];
    return i2c_write_byte(ctx->device_addr, QMI8658_REG_CTRL2, ctrl2) == ESP_OK ? QMI_RESULT_OK : QMI_RESULT_ERROR;
}

// Set gyroscope ODR (simple example)
qmi_result_t qmi_gyro_set_odr(qmi_ctx_t *ctx, uint8_t odr) {
    uint8_t ctrl3;
    if(i2c_read_byte(ctx->device_addr, QMI8658_REG_CTRL3, &ctrl3) != ESP_OK) return QMI_RESULT_ERROR;
    ctrl3 = (ctrl3 & 0xF0) | odr;
    return i2c_write_byte(ctx->device_addr, QMI8658_REG_CTRL3, ctrl3) == ESP_OK ? QMI_RESULT_OK : QMI_RESULT_ERROR;
}

// Set mode
qmi_result_t qmi_set_mode(qmi_ctx_t *ctx, uint8_t mode) {
    uint8_t ctrl7;
    if(i2c_read_byte(ctx->device_addr, QMI8658_REG_CTRL7, &ctrl7) != ESP_OK) return QMI_RESULT_ERROR;
    ctrl7 = (ctrl7 & 0xFC) | mode;
    return i2c_write_byte(ctx->device_addr, QMI8658_REG_CTRL7, ctrl7) == ESP_OK ? QMI_RESULT_OK : QMI_RESULT_ERROR;
}

// Read accelerometer, gyro, temperature
qmi_result_t qmi_read(qmi_ctx_t *ctx, qmi_data_t *data) {
    uint8_t buf[2];
    int16_t temp16;

    // Accelerometer
    uint8_t acc_regs[3][2] = {{QMI8658_REG_AX_L,QMI8658_REG_AX_H},
                              {QMI8658_REG_AY_L,QMI8658_REG_AY_H},
                              {QMI8658_REG_AZ_L,QMI8658_REG_AZ_H}};
    for(int i=0;i<3;i++){
        if(i2c_read_byte(ctx->device_addr, acc_regs[i][0], &buf[0]) != ESP_OK) return QMI_RESULT_ERROR;
        if(i2c_read_byte(ctx->device_addr, acc_regs[i][1], &buf[1]) != ESP_OK) return QMI_RESULT_ERROR;
        temp16 = ((int16_t)buf[1]<<8)|buf[0];
        if(i==0) data->acc_xyz.x = (float)temp16/ctx->acc_sensitivity;
        if(i==1) data->acc_xyz.y = (float)temp16/ctx->acc_sensitivity;
        if(i==2) data->acc_xyz.z = (float)temp16/ctx->acc_sensitivity;
    }

    // Gyroscope
    uint8_t gyro_regs[3][2] = {{QMI8658_REG_GX_L,QMI8658_REG_GX_H},
                               {QMI8658_REG_GY_L,QMI8658_REG_GY_H},
                               {QMI8658_REG_GZ_L,QMI8658_REG_GZ_H}};
    for(int i=0;i<3;i++){
        if(i2c_read_byte(ctx->device_addr, gyro_regs[i][0], &buf[0]) != ESP_OK) return QMI_RESULT_ERROR;
        if(i2c_read_byte(ctx->device_addr, gyro_regs[i][1], &buf[1]) != ESP_OK) return QMI_RESULT_ERROR;
        temp16 = ((int16_t)buf[1]<<8)|buf[0];
        if(i==0) data->gyro_xyz.x = (float)temp16/ctx->gyro_sensitivity;
        if(i==1) data->gyro_xyz.y = (float)temp16/ctx->gyro_sensitivity;
        if(i==2) data->gyro_xyz.z = (float)temp16/ctx->gyro_sensitivity;
    }

    // Temperature
    if(i2c_read_byte(ctx->device_addr, QMI8658_REG_TEMPEARTURE_L, &buf[0]) != ESP_OK) return QMI_RESULT_ERROR;
    if(i2c_read_byte(ctx->device_addr, QMI8658_REG_TEMPEARTURE_H, &buf[1]) != ESP_OK) return QMI_RESULT_ERROR;
    temp16 = ((int16_t)buf[1]<<8)|buf[0];
    data->temperature = ((float)temp16/TEMPERATURE_SENSOR_RESOLUTION-32)*5/9;

    return QMI_RESULT_OK;
}
 
void gait_add_step(gait_metrics_t *g) {
    // Get current time
    time_t t = time(NULL);
    struct tm tm_now;
    localtime_r(&t, &tm_now);
 
    // Reset steps at midnight
    if (tm_now.tm_hour == 0 && tm_now.tm_min == 0) {
        g->steps = 0;
    } else {
        g->steps++;
    }
}
 
void handle_acc_gyr(const qmi_data_t *data, gait_metrics_t *gait_out) {
    const float step_threshold = 1.5f;      // m/s², tune this for real walking
    const uint32_t min_step_interval = 300; // ms, minimum time between steps

    static uint32_t last_step_time = 0;
    static bool step_ready = true;
    static float acc_peak = 0.0f;

    // --- Convert accelerometer from g to m/s² ---
    float ax = data->acc_xyz.x * 9.81f;
    float ay = data->acc_xyz.y * 9.81f;
    float az = data->acc_xyz.z * 9.81f;

    // --- Compute acceleration magnitude ---
    float acc_sq = ax*ax + ay*ay + az*az;
    float acc_mag = pow(acc_sq, 0.5);       // manual sqrt
    float acc_dynamic = acc_mag - 9.81f;    // remove gravity

    // --- Track peak acceleration ---
    if (acc_dynamic > acc_peak) {
        acc_peak = acc_dynamic;
    }

    // --- Step detection at peak ---
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    if (acc_peak > step_threshold && step_ready) {
        if (last_step_time == 0 || (now - last_step_time) > min_step_interval) {
            // calculate dt BEFORE updating last_step_time
            float dt = (last_step_time == 0) ? 0.5f : (now - last_step_time) / 1000.0f; // fallback 0.5s
            last_step_time = now;

            gait_add_step(gait_out);

            // Update cadence, stride length, pace
            if (dt > 0.01f) {
                gait_out->cadence = 60.0f / dt;  // steps/min
                float acc_abs = acc_peak > 0 ? acc_peak : -acc_peak;
                gait_out->stride_length = 0.25f * pow(acc_abs, 0.5f); // meters
                float speed = gait_out->stride_length / dt;            // m/s
                if (speed > 0.01f)
                    gait_out->pace = (1000.0f / speed) / 60.0f;      // min/km
            }
        }
        step_ready = false;
        acc_peak = 0.0f; // reset peak after counting step
    }

    // --- Reset step_ready when acceleration is low ---
    if (acc_dynamic < 0.3f) { // slightly higher than noise
        step_ready = true;
        acc_peak = 0.0f;
    }

    // --- Ground contact (placeholder) ---
    float gz = data->gyro_xyz.z; // assume already in °/s
    if (gz > -5.0f && gz < 5.0f) {
        gait_out->ground_contact = 200; // ms
    }


    // --- Debug print ---
/*    printf("Gait metrics:\n");
    printf("  Stride length: %.2f m\n", gait_out->stride_length);
    printf("  Cadence: %.2f steps/min\n", gait_out->cadence);
    printf("  Pace: %.2f min/km\n", gait_out->pace);
    printf("  Ground contact: %.1f ms\n", gait_out->ground_contact);
    printf("  Steps: %d\n", gait_out->steps);*/
}

lvgl_msg_t msg = {0};

// FreeRTOS task
void imu_task(void *arg) {
    qmi_ctx_t imu;
    qmi_data_t data;

    qmi_init(&imu, QMI8658_ADDRESS, 400000);
    vTaskDelay(pdMS_TO_TICKS(100));
    qmi_reset(&imu);
    vTaskDelay(pdMS_TO_TICKS(100));
    qmi_set_mode(&imu, 0x03); // normal mode
    qmi_acc_set_scale(&imu, 1); // 4G
    qmi_gyro_set_odr(&imu, 0x03); // example ODR
    while(1) {
        if(qmi_read(&imu, &data) == QMI_RESULT_OK) {
            //printf("Acc: %.2f %.2f %.2f\n", data.acc_xyz.x, data.acc_xyz.y, data.acc_xyz.z);
            //printf("Gyro: %.2f %.2f %.2f\n", data.gyro_xyz.x, data.gyro_xyz.y, data.gyro_xyz.z);
            //printf("Temp: %.2f\n", data.temperature);
            snprintf(msg.gait.info, sizeof(msg.gait.info),
		        "   acc    gyr\n"
			    "x %+.2f %+.2f\n"
			    "y %+.2f %+.2f\n"
			    "z %+.2f %+.2f\n",
		        data.acc_xyz.x, data.gyro_xyz.x,
		        data.acc_xyz.y, data.gyro_xyz.y,
		        data.acc_xyz.z, data.gyro_xyz.z
		    );
		    handle_acc_gyr(&data, &msg.gait);
			msg.gait.temperature = data.temperature;
	        // Send gait data to GUI task (non-blocking)
	        if(lvgl_update_queue){
	        	xQueueSend(lvgl_update_queue, &msg, 0);
	        }
	        
        } else {
            printf("IMU read error!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void init_qmi8658(void){
    xTaskCreate(imu_task, "imu_task", 3072, NULL, 5, NULL);
}
