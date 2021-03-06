#include <stdio.h>

#include "esp_system.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "driver/gpio.h"

/*Drivers */
#include "devices/buttons/buttons.h"
#include "devices/display_LCD/display_LCD.h"
#include "devices/encoder/encoder.h"

void app_main(void)
{
	char *ourTaskName = pcTaskGetName(NULL);
	ESP_LOGI(ourTaskName, "Hello, starting up!\n");

	gpio_reset_pin(BLINK_LED);
	gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);

	while (1)
	{
        Blink_lEDS();
	}
}