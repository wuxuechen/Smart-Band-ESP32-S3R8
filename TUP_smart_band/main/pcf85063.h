#ifndef RTC_HEADER_H
#define RTC_HEADER_H
#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include "extra/widgets/calendar/lv_calendar.h"
#include "ui/screens.h"
#include "lcd_touch.h"

// I2C configuration
#define I2C_MASTER_SCL_IO    10        // Set the SCL pin
#define I2C_MASTER_SDA_IO    11        // Set the SDA pin
#define I2C_MASTER_NUM       I2C_NUM_0 // I2C port number
#define I2C_MASTER_FREQ_HZ   100000    // I2C frequency
#define I2C_MASTER_TX_BUF_DISABLE 0    // Disable tx buffer
#define I2C_MASTER_RX_BUF_DISABLE 0    // Disable rx buffer
#define I2C_TIMEOUT_MS       1000

#define PCF85063_ADDR        0x51      // PCF85063 I2C address

#define TAGRTC "PCF85063"

extern const char *weekdays[7];
extern const char *months[12];

/*// Helper function to initiate I2C
esp_err_t i2c_master_init(void);*/

// Read RTC register function
esp_err_t rtc_read_reg(uint8_t reg_addr, uint8_t *data, size_t len);

// Write RTC register function
esp_err_t rtc_write_reg(uint8_t reg_addr, uint8_t *data, size_t len);

// BCD to decimal conversion
uint8_t bcd_to_dec(uint8_t val);

// Decimal to BCD conversion
uint8_t dec_to_bcd(uint8_t val);

// Function to get current time from the RTC
esp_err_t rtc_get_time();

// Function to set time on the RTC
esp_err_t rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day,uint8_t weekday, uint8_t month, uint8_t year);

void rtc_task(void *pvParameters);

void init_rtc(void);
#endif // RTC_HEADER_H

