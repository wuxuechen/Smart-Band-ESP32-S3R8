#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUZZER_GPIO 42

void buzzer_init();

void buzzer_on();

void buzzer_off();

void buzz(void);