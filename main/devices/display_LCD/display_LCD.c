/*
 * diplsay_LCD.c
 *
 *  Created on: 08/07/2022
 *
 */

#include "display_LCD.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

/*Components*/
#include "../../components/esp32-smbus/smbus.h"

/*Devices*/
#include "../buttons/buttons.h"
#include "../encoder/encoder.h"
#include "../rtc/rtc.h"

static const char* TAG = "Display Feedback";

// auxiliary variables for setting rtc reference values
int start_RTC = 0;
char rtc_hour_value_char, rtc_min_value_char, rtc_hour_value_string[20], rtc_min_value_string[20];

// auxiliary variables for storing periodicty and food quantity values that will be set in screens 2 and 3
int periodicity_aux = 12;
int food_aux = 100;
char periodicity_aux_char, periodicity_aux_string[20], food_aux_string[20];

// auxiliary variables for interfacing lcd and encoder easily
int hour_aux, min_aux = 0;
char hour_aux_char, min_aux_char, hour_aux_string[20], min_aux_string[20];

int display_screen_state = 0; // display auxiliary variable, through it we can know which screen is currently being displayed

void i2c_master_init(void)
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    if (gpio_config(&conf) == ESP_OK) ESP_LOGI(TAG, "Successfully configured I2C display!\n");
    else ESP_LOGI(TAG, "I2C display was not configured sucessfully!\n"); 

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

int get_display_screen_state(void) {return display_screen_state;}
int get_start_RTC(void) {return start_RTC;}

void init_RTC(void) {start_RTC = 1;}
void stop_RTC(void) {start_RTC = 0;}

void reset_buttons_and_encoder_value(void)
{
    reset_buttons_state(); reset_sw_encoder_state();
    ESP_LOGI(TAG, "Buttons and encoder reseted.\n");
}

int encoder_variation_display(i2c_lcd1602_info_t * lcd, int aux, int top_limit, int bottom_limit, int column, char aux_string[], char aux_char)
{
    while(!get_sw_encod_is_pressed() || !get_sw_encod_is_pressed_feedback()) 
    {
        update_sw_encod_is_pressed();
        update_present_state_CLK_encoder();
        if(get_past_state_CLK_encoder() != get_present_state_CLK_encoder())
        {
            update_present_state_DT_encoder();
            if (get_present_state_CLK_encoder() != get_present_state_DT_encoder()) 
            {   
                // all these if/else were needed because we are using the same function for screens 0, 2 and 3
                if(bottom_limit == 0) {if(aux < top_limit) aux++;} // screen 0.0 and 0.1 condition
                else if(aux<100) {if(aux >= bottom_limit && aux <= top_limit) aux++;} // screen 2 condition
                else {if(aux >= bottom_limit && aux < top_limit) aux += 100;} // screen 3 condition

                if(aux >= 10 && aux != 1000)
                {
                    sprintf(aux_string, "%d", aux); // transforming int hour (or minute) to string
                    i2c_lcd1602_move_cursor(lcd, column, 1);
                    i2c_lcd1602_write_string(lcd, aux_string);
                }
                else if(aux == 1000)
                {
                    sprintf(aux_string, "%d", aux); 
                    i2c_lcd1602_move_cursor(lcd, column-1, 1); // 1000 value will occupy 1 more slot, so we shift value displayed to left;
                    i2c_lcd1602_write_string(lcd, aux_string);
                }    
                else 
                {
                    aux_char = aux + '0'; 
                    i2c_lcd1602_move_cursor(lcd, column, 1);
                    i2c_lcd1602_write_char(lcd, '0');
                    i2c_lcd1602_write_char(lcd, aux_char);
                }    
            }
            else 
            {
                // all these if/else were needed because we are using the same function for screens 0, 2 and 3
                if(bottom_limit == 0) {if(aux > bottom_limit) aux--;} // screen 0.0 and 0.1 condition
                else if(aux<100) {if(aux > bottom_limit && aux <= top_limit + 1) aux--;} // screen 2 condition
                else {if(aux > bottom_limit && aux <= top_limit) aux -= 100;} // screen 3 condition

                if(aux >= 10 && aux != 900)
                {
                    sprintf(aux_string, "%d", aux); // transforming int hour (or minute) to string
                    i2c_lcd1602_move_cursor(lcd, column, 1);
                    i2c_lcd1602_write_string(lcd, aux_string);
                }
                else if(aux == 900)
                {
                    sprintf(aux_string, "%d", aux);
                    i2c_lcd1602_move_cursor(lcd, column-1, 1); 
                    i2c_lcd1602_write_char(lcd, ' '); // erases the "1" from "1000" value that was read before.
                    i2c_lcd1602_write_string(lcd, aux_string);
                }    
                else 
                {
                    aux_char = aux + '0'; 
                    i2c_lcd1602_move_cursor(lcd, column, 1);
                    i2c_lcd1602_write_char(lcd, '0');
                    i2c_lcd1602_write_char(lcd, aux_char);
                }                     
            }  
            update_past_state_CLK_encoder();
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // adding this delay in pratice helped with encoder precision, the delay can not be much bigger, otherwise encoder gets too slow
        update_sw_encod_is_pressed_feedback(); // adding this second variable for confirming sw_encoder was pressed helped a lot with precision
    }
    ESP_LOGI(TAG, "SW_encoder was pressed!.\n");
    reset_buttons_and_encoder_value();
    i2c_lcd1602_clear(lcd);
    vTaskDelay(pdMS_TO_TICKS(1000));
    return aux;
}

void update_time_values(i2c_lcd1602_info_t * lcd, int rtc_hour_min_update, char rtc_hour_min_string[], char rtc_hour_min_char, int column)
{
    if(rtc_hour_min_update >= 10) // displays the hour or minute that was stored from rtc value
    {
        sprintf(rtc_hour_min_string, "%d", rtc_hour_min_update); 
        i2c_lcd1602_move_cursor(lcd, column, 1);
        i2c_lcd1602_write_string(lcd, rtc_hour_min_string);
    }
    else 
    {
        rtc_hour_min_char = rtc_hour_min_update + '0'; 
        i2c_lcd1602_move_cursor(lcd, column, 1);
        i2c_lcd1602_write_char(lcd, '0');
        i2c_lcd1602_write_char(lcd, rtc_hour_min_char);
    }
}

void turn_off_display(i2c_lcd1602_info_t * lcd)
{   
    ESP_LOGI(TAG, "Begin Screen 5.\n");
    display_screen_state = 5;
    reset_buttons_and_encoder_value();
    i2c_lcd1602_clear(lcd);
    i2c_lcd1602_move_cursor(lcd, 3, 0);
    i2c_lcd1602_write_string(lcd, "Desligando");
    i2c_lcd1602_move_cursor(lcd, 6, 1);
    i2c_lcd1602_write_string(lcd, "Tela");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_clear(lcd);
    i2c_lcd1602_set_backlight(lcd, false); 
    ESP_LOGI(TAG, "End Screen 5.\n");
    ESP_LOGI(TAG, "Display entered stand by mode.\n");
}

int display_go_screen_0_hour(i2c_lcd1602_info_t * lcd, int first_time)
{
    ESP_LOGI(TAG, "Begin Screen 0.0.\n");
    display_screen_state = 0;
    i2c_lcd1602_set_backlight(lcd, true); // turn on backlight
    i2c_lcd1602_clear(lcd);// clear display
    i2c_lcd1602_move_cursor(lcd, 1, 0); // move cursor to column 1 row 0
    i2c_lcd1602_write_string(lcd, "Insira a hora:"); 
    i2c_lcd1602_move_cursor(lcd, 7, 1); 
    if(first_time) i2c_lcd1602_write_string(lcd, "00"); // will only display 00 if it's the first time configuring the RTC reference values
    else 
    {
        if(hour_aux >= 10)
        {
            sprintf(hour_aux_string, "%d", hour_aux);
            i2c_lcd1602_write_string(lcd, hour_aux_string);
        }  
        else 
        {
            hour_aux_char = hour_aux + '0'; 
            i2c_lcd1602_write_char(lcd, '0');
            i2c_lcd1602_write_char(lcd, hour_aux_char);
        }          
    }
    hour_aux = encoder_variation_display(lcd, hour_aux, 23, 0, 7, hour_aux_string, hour_aux_char);
    ESP_LOGI(TAG, "End of Screen 0.0.\n");
    return hour_aux;
}

int display_go_screen_0_minutes(i2c_lcd1602_info_t * lcd, int first_time)
{
    ESP_LOGI(TAG, "Begin Screen 0.1.\n");
    display_screen_state = 0;
    i2c_lcd1602_clear(lcd);
    i2c_lcd1602_move_cursor(lcd, 4, 0);
    i2c_lcd1602_write_string(lcd, "Minutos:");
    i2c_lcd1602_move_cursor(lcd, 7, 1); 
    if(first_time) i2c_lcd1602_write_string(lcd, "00"); 
    else 
    {
        if(min_aux >= 10)
        {
            sprintf(min_aux_string, "%d", min_aux);
            i2c_lcd1602_write_string(lcd, min_aux_string);
        }  
        else 
        {
            min_aux_char = min_aux + '0'; 
            i2c_lcd1602_write_char(lcd, '0');
            i2c_lcd1602_write_char(lcd, min_aux_char);
        }          
    }
    min_aux = encoder_variation_display(lcd, min_aux, 59, 0, 7, min_aux_string, min_aux_char);
    ESP_LOGI(TAG, "End of Screen 0.1.\n");
    return min_aux;
}

void display_go_screen_1(i2c_lcd1602_info_t * lcd, int hour, int min)
{
    // will freeze screen updating rtc values until either button_1, button_2 or button_3 is pressed
    ESP_LOGI(TAG, "Begin Screen 1.\n");
    display_screen_state = 1;
    i2c_lcd1602_clear(lcd);
    i2c_lcd1602_move_cursor(lcd, 3, 0);
    i2c_lcd1602_write_string(lcd, "Bem vindo!");
    i2c_lcd1602_move_cursor(lcd, 7, 1);
    i2c_lcd1602_write_char(lcd, ':');
    while(!get_button_1_state() && !get_button_2_state() && !get_button_3_state())
    {
        update_button_1_state();
        update_button_2_state();
        update_button_3_state();
        update_time_values(lcd, hour, rtc_hour_value_string, rtc_hour_value_char, 5);
        update_time_values(lcd, min, rtc_min_value_string, rtc_min_value_char, 8);
        hour = get_time_hour();
        min = get_time_min();
        vTaskDelay(pdMS_TO_TICKS(50));
    } 
    i2c_lcd1602_clear(lcd);
    ESP_LOGI(TAG, "End of Screen 1.\n");
}

int display_go_screen_2(i2c_lcd1602_info_t * lcd)
{   
    ESP_LOGI(TAG, "Begin Screen 2.\n");
    display_screen_state = 2;
    reset_buttons_and_encoder_value();
    i2c_lcd1602_move_cursor(lcd, 1, 0);
    i2c_lcd1602_write_string(lcd, "Periodicidade:");
    i2c_lcd1602_move_cursor(lcd, 7, 1);
    i2c_lcd1602_write_string(lcd, "horas");
    sprintf(periodicity_aux_string, "%d", periodicity_aux);
    i2c_lcd1602_move_cursor(lcd, 4, 1);
    i2c_lcd1602_write_string(lcd, periodicity_aux_string);
    periodicity_aux = encoder_variation_display(lcd, periodicity_aux, 23, 4, 4, periodicity_aux_string, periodicity_aux_char);
    i2c_lcd1602_clear(lcd);
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "End of Screen 2.\n");
    return periodicity_aux;
}

int display_go_screen_3(i2c_lcd1602_info_t * lcd)
{
    ESP_LOGI(TAG, "Begin Screen 3.\n");
    display_screen_state = 3;
    i2c_lcd1602_move_cursor(lcd, 2, 0);
    i2c_lcd1602_write_string(lcd, "Quantidade:");
    i2c_lcd1602_move_cursor(lcd, 7, 1);
    i2c_lcd1602_write_string(lcd, "gramas");
    sprintf(food_aux_string, "%d", food_aux);
    i2c_lcd1602_move_cursor(lcd, 3, 1);
    i2c_lcd1602_write_string(lcd, food_aux_string);
    reset_buttons_and_encoder_value();
    food_aux = encoder_variation_display(lcd, food_aux, 1000, 100, 3, food_aux_string, '$'); // since food_aux is always higher than 100g we don't need to use single char in this function
    i2c_lcd1602_clear(lcd);
    ESP_LOGI(TAG, "End of Screen 3.\n");
    i2c_lcd1602_move_cursor(lcd, 0, 0);
    i2c_lcd1602_write_string(lcd, "Alteracao feita!");    
    i2c_lcd1602_move_cursor(lcd, 4, 1);
    i2c_lcd1602_write_string(lcd, "Obrigado");    
    vTaskDelay(pdMS_TO_TICKS(1000));
    turn_off_display(lcd);
    reset_buttons_and_encoder_value();
    return food_aux;
}
