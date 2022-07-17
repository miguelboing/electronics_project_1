/*
 * buzzer.c
 *
 *  Created on: 16/06/2022
 *
 */

#include "buzzer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUZZER GPIO_NUM_18 /* D18 */

#define BUZZER_ON  1
#define BUZZER_OFF 0

static const char* TAG = "Buzzer Feedback";

void buzzer_init(void)
{
    gpio_config_t buzzer_gpio = {};
    buzzer_gpio.intr_type = GPIO_INTR_DISABLE;
    buzzer_gpio.mode = GPIO_MODE_OUTPUT;
    buzzer_gpio.pull_up_en = GPIO_PULLUP_DISABLE;
    buzzer_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;
    buzzer_gpio.pin_bit_mask = (1ULL << BUZZER);
    if (gpio_config(&buzzer_gpio) == ESP_OK) ESP_LOGI(TAG, "Successfully configured buzzer!\n");
    else ESP_LOGE(TAG, "Buzzer was not configured sucessfully!\n"); 
}

void buzzer_beep(void)
{
    gpio_set_level(BUZZER, BUZZER_ON);
    vTaskDelay(2000 / portTICK_PERIOD_MS); 
    gpio_set_level(BUZZER, BUZZER_OFF);
    vTaskDelay(500 / portTICK_PERIOD_MS); 
}