/*
 * buttons.c
 *
 *  Created on: 18 de mai de 2022
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "buttons.h"

// Button pins inicialization;
void button_init(button_e button)
{
    gpio_config_t button_init_gpio = {};  
    button_init_gpio.intr_type = GPIO_INTR_NEGEDGE; // Interruption occurs when level goes down (pull up resistors are being used);
    button_init_gpio.mode = GPIO_MODE_INPUT; 
    button_init_gpio.pull_up_en = GPIO_PULLUP_ENABLE; 
    button_init_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    button_init_gpio.pin_bit_mask = (1ULL << button); 

    if (gpio_config(&button_init_gpio) == ESP_OK)
    {
        printf("Successfully configured button!\n");  
    }
    else printf("Button was not configured sucessfully\n"); 
}

// ISR + TASK for Button_1;
void IRAM_ATTR button_1_isr_handler(void* param) {xTaskResumeFromISR(task_handle_button_1);}

void task_button1(void* param)
{   
    button_init(BUTTON_1);
    gpio_isr_handler_add(BUTTON_1, button_1_isr_handler, NULL);

    while (1)
    {
        vTaskSuspend(NULL);

        printf("Button 1 was pressed!\n");
/*        
        if(current_screen == home_screen) 
        {
            go_to_periodicity_screen();
            current_screen = periodicity_screen;
        }    
*/        
        vTaskDelay(pdMS_TO_TICKS(400));	
    }  
}

// ISR + TASK for Button_2;
void IRAM_ATTR button_2_isr_handler(void* param) {xTaskResumeFromISR(task_handle_button_2);}

void task_button2(void* param)
{    
    button_init(BUTTON_2);
    gpio_isr_handler_add(BUTTON_2, button_2_isr_handler, NULL);

    while (1)
    {
        vTaskSuspend(NULL);

        printf("Button 2 was pressed!\n");
/*
        if (current_screen == periodicity_screen) 
        {
            go_to_end_screen();
            current_screen = end_screen;
        }    
*/    
        vTaskDelay(pdMS_TO_TICKS(400));	
    }
}

// ISR + TASK for Button_3;
void IRAM_ATTR button_3_isr_handler(void* param) {xTaskResumeFromISR(task_handle_button_3);}

void task_button3(void* param)
{    
    button_init(BUTTON_3);
    gpio_isr_handler_add(BUTTON_3, button_3_isr_handler, NULL);

    while (1)
    {
        vTaskSuspend(NULL);

        printf("Button 3 was pressed!\n");
/*
        if (current_screen == periodicity_screen) 
        {
            go_to_home_screen();
            current_screen = home_screen;
        }

        if (current_screen == food_quantity_screen) 
        {
            go_to_periodicity_screen();
            current_screen = periodicity_screen;
        }
*/        
        vTaskDelay(pdMS_TO_TICKS(400));	
    }
}
