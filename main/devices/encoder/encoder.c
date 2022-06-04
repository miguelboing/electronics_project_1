/*
 * encoder.c
 *
 *  Created on: 26/05/2022
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "encoder.h"

void clk_encoder_init(clk_encoder_e encod)
{
    gpio_config_t clk_encoder_gpio = {};  
    clk_encoder_gpio.intr_type = GPIO_INTR_ANYEDGE; 
    clk_encoder_gpio.mode = GPIO_MODE_INPUT; 
    clk_encoder_gpio.pull_up_en = GPIO_PULLUP_DISABLE; 
    clk_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    clk_encoder_gpio.pin_bit_mask = (1ULL << encod); 

    if (gpio_config(&clk_encoder_gpio) == ESP_OK) printf("Successfully configured CLK_encoder!\n");  
    else printf("CLK_encoder was not configured sucessfully\n"); 
}

void dt_encoder_init(dt_encoder_e encod)
{
    gpio_config_t dt_encoder_gpio = {};  
    dt_encoder_gpio.intr_type = GPIO_INTR_DISABLE; 
    dt_encoder_gpio.mode = GPIO_MODE_INPUT; 
    dt_encoder_gpio.pull_up_en = GPIO_PULLUP_DISABLE; 
    dt_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    dt_encoder_gpio.pin_bit_mask = (1ULL << encod); 

    if (gpio_config(&dt_encoder_gpio) == ESP_OK) printf("Successfully configured DT_encoder!\n");  
    else printf("DT_encoder was not configured sucessfully\n"); 
}

void sw_encoder_init(sw_encoder_e encod)
{
    gpio_config_t sw_encoder_gpio = {};  
    sw_encoder_gpio.intr_type = GPIO_INTR_NEGEDGE; 
    sw_encoder_gpio.mode = GPIO_MODE_INPUT; 
    sw_encoder_gpio.pull_up_en = GPIO_PULLUP_ENABLE; 
    sw_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    sw_encoder_gpio.pin_bit_mask = (1ULL << encod); 
    
    if (gpio_config(&sw_encoder_gpio) == ESP_OK) printf("Successfully configured SW_encoder!\n");  
    else printf("SW_encoder was not configured sucessfully\n"); 
}

int verify_clk_encoder_level(clk_encoder_e encod) {return gpio_get_level(encod);}
int verify_dt_encoder_level(dt_encoder_e encod) {return gpio_get_level(encod);}

// ISR + TASK for SW_encoder;
void IRAM_ATTR sw_encoder_isr_handler(void* param) {xTaskResumeFromISR(task_handle_sw_encoder);}

void task_sw_encoder(void* param)
{
	sw_encoder_init(SW_encoder);

   	gpio_isr_handler_add(SW_encoder, sw_encoder_isr_handler, NULL);

	while (1)
	{
		vTaskSuspend(NULL);
		// confirm food quantity counter value;
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

// ISR + TASK for CLK_encoder and DT_encoder;
void IRAM_ATTR rotation_encoder_isr_handler(void* param) {xTaskResumeFromISR(task_handle_rotation_encoder);}

void task_rotation_encoder(void* param)
{	
	clk_encoder_init(CLK_encoder); 
	dt_encoder_init(DT_encoder);

   	gpio_isr_handler_add(CLK_encoder, rotation_encoder_isr_handler, NULL);

	int present_state_CLK_encoder;
    	int present_state_DT_encoder;
    	int food_quantity_counter = 0;

	while (1)
	{
		vTaskSuspend(NULL);

		present_state_CLK_encoder = verify_clk_encoder_level(CLK_encoder);
		present_state_DT_encoder = verify_clk_encoder_level(DT_encoder);

		if (present_state_CLK_encoder != present_state_DT_encoder) 
		{
		    food_quantity_counter++;
		    vTaskDelay(pdMS_TO_TICKS(10));
		}
		else 
		{
		    food_quantity_counter--;
		    vTaskDelay(pdMS_TO_TICKS(10));
		}    
	}
}
