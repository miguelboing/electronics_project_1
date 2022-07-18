/*
 * ac_check.c
 *
 *  Created on: 14/07/2022
 *
 */

#include "ac_check.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char* TAG_AC_CHECK = "AC CHECK:";

int ac_check_status;

#define AC_check_pin GPIO_NUM_26 /* D26 */

void ac_check_init(void)
{
    gpio_config_t ac_check_gpio = {};
    ac_check_gpio.intr_type = GPIO_INTR_DISABLE;
    ac_check_gpio.mode = GPIO_MODE_INPUT;   
    ac_check_gpio.pull_up_en = GPIO_PULLUP_ENABLE;
    ac_check_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ac_check_gpio.pin_bit_mask = (1ULL << AC_check_pin); 

    if (gpio_config(&ac_check_gpio) == ESP_OK) ESP_LOGI(TAG_AC_CHECK, "Successfully configured AC check!");
    else ESP_LOGE(TAG_AC_CHECK, "AC check was not configured successfully!");
}

int ac_check_power(void)
{
    ac_check_status = gpio_get_level(AC_check_pin);

    if(!ac_check_status) ESP_LOGW(TAG_AC_CHECK, "Feeder is not plugged, running in battery mode!");

    return ac_check_status;
}
