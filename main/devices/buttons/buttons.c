/*
 * buttons.c
 *
 *  Created on: 18/05/2022
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "buttons.h"

void button_init(button_e button)
{
    gpio_config_t button_init_gpio = {};  
    button_init_gpio.intr_type = GPIO_INTR_NEGEDGE; 
    button_init_gpio.mode = GPIO_MODE_INPUT; 
    button_init_gpio.pull_up_en = GPIO_PULLUP_ENABLE; 
    button_init_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    button_init_gpio.pin_bit_mask = (1ULL << button); 

    if (gpio_config(&button_init_gpio) == ESP_OK) printf("Successfully configured button!\n");
    else printf("Button was not configured sucessfully\n"); 
}

// ISR + TASK for Button_1;
void IRAM_ATTR button_1_isr_handler(void* param) {xTaskResumeFromISR(task_handle_button_1);}

void task_button_1(void* param)
{   
    button_init(BUTTON_1);
    gpio_isr_handler_add(BUTTON_1, button_1_isr_handler, NULL);

    while (1)
    {
        vTaskSuspend(NULL);
        // screen changing code;
        vTaskDelay(pdMS_TO_TICKS(250));
    }  
}

// ISR + TASK for Button_2;
void IRAM_ATTR button_2_isr_handler(void* param) {xTaskResumeFromISR(task_handle_button_2);}

void task_button_2(void* param)
{    
    button_init(BUTTON_2);
    gpio_isr_handler_add(BUTTON_2, button_2_isr_handler, NULL);

    while (1)
    {
        vTaskSuspend(NULL);
        // screen changing code;
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

// ISR + TASK for Button_3;
void IRAM_ATTR button_3_isr_handler(void* param) {xTaskResumeFromISR(task_handle_button_3);}

void task_button_3(void* param)
{    
    button_init(BUTTON_3);
    gpio_isr_handler_add(BUTTON_3, button_3_isr_handler, NULL);

    while (1)
    {
        vTaskSuspend(NULL);
        // screen changing code;
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
