#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "buttons.h"

void Blink_lEDS(void)
{
    gpio_set_level(BLINK_LED, 1);
    vTaskDelay(1000/ portTICK_PERIOD_MS);
    gpio_set_level(BLINK_LED, 0);
    vTaskDelay(1000/ portTICK_PERIOD_MS);
}