/*
 * buttons.c
 *
 *  Created on: 18/05/2022
 *
 */

#include "esp_log.h"

#include "driver/gpio.h"
#include "buttons.h"

static const char* TAG_BUTTONS = "Buttons Feedback";

int button_1_is_pressed, button_2_is_pressed, 
    button_3_is_pressed, button_state;

void button_init(button_e button)
{
    gpio_config_t button_init_gpio = {};  
    button_init_gpio.intr_type = GPIO_INTR_DISABLE; 
    button_init_gpio.mode = GPIO_MODE_INPUT; 
    button_init_gpio.pull_up_en = GPIO_PULLUP_ENABLE; 
    button_init_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    button_init_gpio.pin_bit_mask = (1ULL << button); 
    
    if (gpio_config(&button_init_gpio) == ESP_OK) ESP_LOGI(TAG_BUTTONS, "Successfully configured button!\n");
    else ESP_LOGE(TAG_BUTTONS, "Button was not configured sucessfully!\n"); 
}

void button_reset_states(void)
{
    button_1_is_pressed = 0;
    button_2_is_pressed = 0;
    button_3_is_pressed = 0;
}

void button_update_state(button_e button)
{
    switch(button)
    {
        case BUTTON_1:
            button_1_is_pressed = button_is_pressed(BUTTON_1);
            break;
        case BUTTON_2:
            button_2_is_pressed = button_is_pressed(BUTTON_2);
            break;
        case BUTTON_3:
            button_3_is_pressed = button_is_pressed(BUTTON_3);
            break;
    }
}

int button_is_pressed(button_e button)
{
    return !gpio_get_level(button);
}

int button_get_state(button_e button)
{
    switch(button)
    {
        case BUTTON_1:
            button_state = button_1_is_pressed;
            break;
        case BUTTON_2:
            button_state = button_2_is_pressed;
            break;
        case BUTTON_3:
            button_state = button_3_is_pressed;
            break;
    }
    return button_state;
}
