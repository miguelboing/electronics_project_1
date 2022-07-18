/*
 * encoder.c
 *
 *  Created on: 26/05/2022
 *
 */

#include "esp_log.h"

#include "driver/gpio.h"
#include "encoder.h"

static const char* TAG_ENCODER = "Encoder Feedback";

int past_state_CLK_encoder, present_state_CLK_encoder, 
    present_state_DT_encoder, sw_encod_is_pressed, 
    sw_encod_is_pressed_feedback;

int encoder_state, encoder_level;

void encoder_init(encoder_e encoder)
{
    switch(encoder)
    {
        case CLK_encoder:
            gpio_config_t clk_encoder_gpio = {};  
            clk_encoder_gpio.intr_type = GPIO_INTR_DISABLE; 
            clk_encoder_gpio.mode = GPIO_MODE_INPUT; 
            clk_encoder_gpio.pull_up_en = GPIO_PULLUP_DISABLE; 
            clk_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;  
            clk_encoder_gpio.pin_bit_mask = (1ULL << encoder); 
            if (gpio_config(&clk_encoder_gpio) == ESP_OK) ESP_LOGI(TAG_ENCODER, "Successfully configured CLK_encoder!\n");  
            else ESP_LOGE(TAG_ENCODER, "CLK_encoder was not configured sucessfully\n");
            break;

        case DT_encoder:
            gpio_config_t dt_encoder_gpio = {};  
            dt_encoder_gpio.intr_type = GPIO_INTR_DISABLE; 
            dt_encoder_gpio.mode = GPIO_MODE_INPUT; 
            dt_encoder_gpio.pull_up_en = GPIO_PULLUP_DISABLE; 
            dt_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;  
            dt_encoder_gpio.pin_bit_mask = (1ULL << encoder); 
            if (gpio_config(&dt_encoder_gpio) == ESP_OK) ESP_LOGI(TAG_ENCODER, "Successfully configured DT_encoder!\n");  
            else ESP_LOGE(TAG_ENCODER, "DT_encoder was not configured sucessfully\n");
            break;

        case SW_encoder:
            gpio_config_t sw_encoder_gpio = {};  
            sw_encoder_gpio.intr_type = GPIO_INTR_DISABLE; 
            sw_encoder_gpio.mode = GPIO_MODE_INPUT; 
            sw_encoder_gpio.pull_up_en = GPIO_PULLUP_ENABLE; 
            sw_encoder_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE; 
            sw_encoder_gpio.pin_bit_mask = (1ULL << encoder); 
            if (gpio_config(&sw_encoder_gpio) == ESP_OK) ESP_LOGI(TAG_ENCODER, "Successfully configured SW_encoder!\n");  
            else ESP_LOGE(TAG_ENCODER, "SW_encoder was not configured sucessfully\n"); 
            break;

        case CLK_past_state:
            break;

        case SW_encoder_feedback:
            break;
    }
}

int encoder_verify_level(encoder_e encoder)
{
    switch(encoder)
    {
        case CLK_encoder:
            encoder_level = !gpio_get_level(CLK_encoder);
            break;
        case DT_encoder:
            encoder_level = !gpio_get_level(DT_encoder);
            break;
        case SW_encoder:
            encoder_level = !gpio_get_level(SW_encoder);
            break;
        case CLK_past_state:
            break;
        case SW_encoder_feedback:
            break;
    }
    return encoder_level;
}

int encoder_get_state(encoder_e encoder)
{
    switch(encoder)
    {
        case CLK_encoder:
            encoder_state = present_state_CLK_encoder;
            break;
        case CLK_past_state:
            encoder_state = past_state_CLK_encoder;
            break;
        case DT_encoder:
            encoder_state = present_state_DT_encoder;
            break;
        case SW_encoder:
            encoder_state = sw_encod_is_pressed;
            break;
        case SW_encoder_feedback:
            encoder_state = sw_encod_is_pressed_feedback;
            break;            
    }
    return encoder_state;
}

void encoder_update_state(encoder_e encoder)
{
    switch(encoder)
    {
        case CLK_encoder:
            present_state_CLK_encoder = encoder_verify_level(CLK_encoder);
            break;
        case CLK_past_state:
            past_state_CLK_encoder = encoder_verify_level(CLK_encoder);
            break;
        case DT_encoder:
            present_state_DT_encoder = encoder_verify_level(DT_encoder);
            break;
        case SW_encoder:
            sw_encod_is_pressed = encoder_verify_level(SW_encoder);
            break;
        case SW_encoder_feedback:
            sw_encod_is_pressed_feedback = encoder_verify_level(SW_encoder);
            break;            
    }
}

void reset_sw_encoder_state(void) 
{
    sw_encod_is_pressed = 0; 
    sw_encod_is_pressed_feedback = 0;
}
