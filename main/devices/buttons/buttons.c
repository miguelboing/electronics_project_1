#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "buttons.h"

void button_init(button_e button){

    gpio_config_t button_gpio = {};  
    button_gpio.intr_type = GPIO_INTR_DISABLE; // Botao 1
    button_gpio.mode = GPIO_MODE_INPUT; 
    button_gpio.pull_up_en = GPIO_PULLUP_ENABLE; 
    button_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    button_gpio.pin_bit_mask = (1ULL << button); 
    if (gpio_config(&button_gpio) == ESP_OK)
    {
        printf("Successfully configured button!\n");  
    }
    else printf("Button was not configured sucessfully\n"); 
}

int button_is_pressed(button_e button)
{
    return !gpio_get_level(button);
}

/* Habilita a interrupcao do botao
void button_enable(button_e button){
}*/

/* Desabilita a interrupcao do botao
void button_disable(button_e button){
}*/




