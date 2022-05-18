#include <stdio.h>

#include "esp_system.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "driver/gpio.h"

#define BLINK_LED 2

void app_main(void)
{
	char *ourTaskName = pcTaskGetName(NULL);
	ESP_LOGI(ourTaskName, "Hello, starting up!\n");

	gpio_reset_pin(BLINK_LED);
	gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);

	while (1)
	{
		gpio_set_level(BLINK_LED, 1);
		vTaskDelay(1000/ portTICK_PERIOD_MS);
		gpio_set_level(BLINK_LED, 0);
		vTaskDelay(1000/ portTICK_PERIOD_MS);
	}
}
