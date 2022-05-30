/*
 * encoder.c
 *
 *  Created on: 26 de mai de 2022
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "encoder.h"

// CLK pin inicialization;
void clk_encoder_init(clk_encoder_e encod)
{
    gpio_config_t clk_encoder_gpio = {};  
    clk_encoder_gpio.intr_type = GPIO_INTR_ANYEDGE; // Interruption occurs for both sides;
    clk_encoder_gpio.mode = GPIO_MODE_INPUT; 
    clk_encoder_gpio.pull_up_en = GPIO_PULLUP_DISABLE; 
    clk_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    clk_encoder_gpio.pin_bit_mask = (1ULL << encod); 

    if (gpio_config(&clk_encoder_gpio) == ESP_OK)
    {
        printf("Successfully configured CLK encoder!\n");  
    }
    else 
        printf("CLK encoder was not configured sucessfully\n"); 
}

// DT pin inicialization;
void dt_encoder_init(dt_encoder_e encod)
{
    gpio_config_t dt_encoder_gpio = {};  
    dt_encoder_gpio.intr_type = GPIO_INTR_DISABLE; // Interruption disabled for DT pin;
    dt_encoder_gpio.mode = GPIO_MODE_INPUT; 
    dt_encoder_gpio.pull_up_en = GPIO_PULLUP_DISABLE; 
    dt_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;  
    dt_encoder_gpio.pin_bit_mask = (1ULL << encod); 

    if (gpio_config(&dt_encoder_gpio) == ESP_OK)
    {
        printf("Successfully configured DT encoder!\n");  
    }
    else 
        printf("DT encoder was not configured sucessfully\n"); 
}

// Switch pin inicialization;
void sw_encoder_init(sw_encoder_e encod)
{
    gpio_config_t sw_encoder_gpio = {};  
    sw_encoder_gpio.intr_type = GPIO_INTR_NEGEDGE; // Interruption occurs when level goes down (pull up resistors are being used);
    sw_encoder_gpio.mode = GPIO_MODE_INPUT; 
    sw_encoder_gpio.pull_up_en = GPIO_PULLUP_ENABLE; 
    sw_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    sw_encoder_gpio.pin_bit_mask = (1ULL << encod); 
    
    if (gpio_config(&sw_encoder_gpio) == ESP_OK)
    {
        printf("Successfully configured SW encoder!\n");  
    }
    else 
        printf("SW encoder was not configured sucessfully\n"); 
}


// Verify CLK level;
int verify_clk_encoder_level(clk_encoder_e encod)
{
    return gpio_get_level(encod);
}

// Verify DT level;
int verify_dt_encoder_level(dt_encoder_e encod)
{
    return gpio_get_level(encod);
}

// ISR + TASK for SW_encoder;
void IRAM_ATTR sw_encoder_isr_handler(void* param) {xTaskResumeFromISR(task_handle_sw_encoder);}

void task_sw_encoder(void* param)
{
	sw_encoder_init(SW_encoder);
    gpio_isr_handler_add(SW_encoder, sw_encoder_isr_handler, NULL);

	while (1)
	{
        vTaskSuspend(NULL);

        printf("SW_encoder was pressed!\n");
/*        
        if(current_screen == food_quantity_screen) 
        {
            go_to_end_screen();
            current_screen = end_screen;
        }    

        if(current_screen == periodicity_screen) 
        {
            go_to_food_quantity_screen();
            current_screen = food_quantity_screen;
        }    
*/        
        vTaskDelay(pdMS_TO_TICKS(400));	
	}
}

// ISR + TASK for CLK_encoder and DT_encoder;
void IRAM_ATTR rot_encoder_isr_handler(void* param) {xTaskResumeFromISR(task_handle_rot_encoder);}

void task_rot_encoder(void* param)
{	
	// Pins inicialization;
	clk_encoder_init(CLK_encoder);
    gpio_isr_handler_add(CLK_encoder, rot_encoder_isr_handler, NULL);
	dt_encoder_init(DT_encoder);

    // Encoder variables;
	int present_state_CLK_encoder;
	int past_state_CLK_encoder = verify_clk_encoder_level(CLK_encoder);
	int counter_encoder = 0;

	while (1)
	{
        vTaskSuspend(NULL);

        present_state_CLK_encoder = verify_clk_encoder_level(CLK_encoder);

        // Verifies the phase between CLK and DT signals, with this information we can know the encoder's direction of rotation;
        // If after CLK level transition CLK and DT levels are not the same, then encoder is clockwise;
        // If after CLK level transition CLK and DT levels are both the same, then encoder is anti-clockwise;
        if (present_state_CLK_encoder != verify_dt_encoder_level(DT_encoder)) 
        {
            counter_encoder++;
            printf("Encoder is clockwise! %d\n", counter_encoder);
        } 
        else
        {
            counter_encoder--;
            printf("Encoder is anti-clockwise! %d\n", counter_encoder);
        } 
        past_state_CLK_encoder = present_state_CLK_encoder;
	}
}
