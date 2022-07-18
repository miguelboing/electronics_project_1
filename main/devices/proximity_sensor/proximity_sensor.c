/*
 * proximity_sensor.c
 *
 *  Created on: 18/05/2022
 *
 */

#include "proximity_sensor.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define IR_out GPIO_NUM_4 /* D4 */

static const char* TAG = "Proximity Sensor Feedback";

void proximity_sensor_init(void)
{
    gpio_config_t proximity_sensor_gpio = {};
    proximity_sensor_gpio.intr_type = GPIO_INTR_DISABLE;
    proximity_sensor_gpio.mode = GPIO_MODE_INPUT;
    proximity_sensor_gpio.pull_up_en = GPIO_PULLUP_ENABLE;
    proximity_sensor_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;
    proximity_sensor_gpio.pin_bit_mask = (1ULL << IR_out);
    
    if (gpio_config(&proximity_sensor_gpio) == ESP_OK) ESP_LOGI(TAG, "Successfully configured proximity sensor!\n");
    else ESP_LOGE(TAG, "Proximity sensor was not configured sucessfully!\n"); 
}

int proximity_sensor_get_presence(void)
{
    return !gpio_get_level(IR_out);
}
