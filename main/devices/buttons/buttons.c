/*
 * buttons.c
 *
 *  Created on: 18/05/2022
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "driver/gpio.h"
#include "buttons.h"

static const char* TAG = "Buttons Feedback";

int button_1_is_pressed, button_2_is_pressed, button_3_is_pressed; 

void button_init(button_e button)
{
    gpio_config_t button_init_gpio = {};  
    button_init_gpio.intr_type = GPIO_INTR_DISABLE; 
    button_init_gpio.mode = GPIO_MODE_INPUT; 
    button_init_gpio.pull_up_en = GPIO_PULLUP_ENABLE; 
    button_init_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    button_init_gpio.pin_bit_mask = (1ULL << button); 
    if (gpio_config(&button_init_gpio) == ESP_OK) ESP_LOGI(TAG, "Successfully configured button!\n");
    else ESP_LOGI(TAG, "Button was not configured sucessfully!\n"); 
}

void reset_buttons_state(void) {button_1_is_pressed = 0; button_2_is_pressed = 0; button_3_is_pressed = 0;}

void update_button_1_state(void) {button_1_is_pressed = button_is_pressed(BUTTON_1);}
void update_button_2_state(void) {button_2_is_pressed = button_is_pressed(BUTTON_2);}
void update_button_3_state(void) {button_3_is_pressed = button_is_pressed(BUTTON_3);}

int button_is_pressed(button_e button) {return !gpio_get_level(button);}

int get_button_1_state(void) {return button_1_is_pressed;}
int get_button_2_state(void) {return button_2_is_pressed;}
int get_button_3_state(void) {return button_3_is_pressed;}

