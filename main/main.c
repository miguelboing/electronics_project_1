#include <stdio.h>

#include "esp_system.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "driver/gpio.h"

/*Drivers */
#include "devices/buttons/buttons.h"

static const char* TAG = "Feedback";

void app_main(void)
{
	char *ourTaskName = pcTaskGetName(NULL);
	ESP_LOGI(ourTaskName, "Hello, starting up!\n");

	button_init(BUTTON_1);
	button_init(BUTTON_2);
	button_init(BUTTON_3);

	int button_1_is_pressed;
	int button_2_is_pressed;
	int button_3_is_pressed;                                                  

	while (1)
	{
		button_1_is_pressed = button_is_pressed(BUTTON_1); 
		button_2_is_pressed = button_is_pressed(BUTTON_2);
		button_3_is_pressed = button_is_pressed(BUTTON_3);

		if (button_1_is_pressed) {ESP_LOGI(TAG, "Button 1 was pressed!\n");}
		else 
		if (button_2_is_pressed) {ESP_LOGI(TAG, "Button 2 was pressed!\n");}
		else
		if (button_3_is_pressed) {ESP_LOGI(TAG, "Button 3 was pressed!\n");}
		else                     
								 {ESP_LOGI(TAG, "Press a button!\n");}

		vTaskDelay(pdMS_TO_TICKS(250));
	}
}
