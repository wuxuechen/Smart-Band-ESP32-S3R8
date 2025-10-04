#include "keys.h"
#include "soc/gpio_num.h"
#include "constant.h"

#define BOOT_KEY_GPIO  GPIO_NUM_0

static const char *TAG = "KEYS";



#define LCD_GPIO_BL (GPIO_NUM_15)  // backlight pin


static void screen_set_backlight(bool on) {
    gpio_set_level(LCD_GPIO_BL, on ? 1 : 0);
    screen_on = on;
}

// ISR handler
static void IRAM_ATTR key_isr_handler(void *arg)
{
    int gpio_num = (int)arg;
    int level = gpio_get_level(gpio_num);

    if (level == 0) { // pressed
        if (gpio_num == BOOT_KEY_GPIO) {
            ESP_EARLY_LOGI(TAG, "BOOT key pressed");
        }
    } else { // released
        if (gpio_num == BOOT_KEY_GPIO) {
            ESP_EARLY_LOGI(TAG, "BOOT key released");
        }
        if (screen_on){
			screen_set_backlight(false);  // turn off
	        screen_on = false;
		}else{
			screen_set_backlight(true);   // turn on
			screen_on = true;
		}
    }
}

void keys_init(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE, // trigger on press + release
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BOOT_KEY_GPIO),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,   // internal pull-up
    };
    gpio_config(&io_conf);

    // install ISR service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOOT_KEY_GPIO, key_isr_handler, (void *)BOOT_KEY_GPIO);
}
