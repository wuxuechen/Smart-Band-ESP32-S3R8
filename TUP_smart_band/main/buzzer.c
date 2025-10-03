#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUZZER_GPIO 42
#define BUZZER_CHANNEL LEDC_CHANNEL_0
#define BUZZER_TIMER   LEDC_TIMER_0
#define BUZZER_FREQ    2000    // 2kHz tone
#define BUZZER_DUTY    4000    // Duty (0-8191 for 13-bit)

void buzzer_init()
{
    ledc_timer_config_t timer_conf = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .timer_num        = BUZZER_TIMER,
        .freq_hz          = BUZZER_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t ch_conf = {
        .gpio_num       = BUZZER_GPIO,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = BUZZER_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = BUZZER_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ch_conf);
}

void buzzer_on()
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL, BUZZER_DUTY);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL);
}

void buzzer_off()
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL);
}

// Example: buzz for 2 seconds
void buzz()
{
    buzzer_init();
    buzzer_on();
    vTaskDelay(pdMS_TO_TICKS(500));
    buzzer_off();
}
