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
//#include "../../components/esp32-smbus/include/smbus.h"
//#include "../../components/esp32-i2c-lcd1602/include/i2c-lcd1602.h"

/*Devices*/
#include "../buttons/buttons.h"
#include "../encoder/encoder.h"

// these variables will set rtc start values
int rtc_hour_value, rtc_min_value;
char rtc_hour_value_char, rtc_min_value_char;
char rtc_hour_value_string[], rtc_min_value_string[];

// auxiliary variables for storing periodicty and food quantity values that will be set in screens 2 and 3
int periodicity_hour_value, periodicity_aux = 12;
int food_quantitiy_value, food_aux = 100;
char periodicity_aux_char, periodicity_aux_string[], food_aux_string[];

// auxiliary variables for interfacing lcd and encoder easily
int hour_aux, min_aux = 0;
char hour_aux_string[], min_aux_string[];
char hour_aux_char, min_aux_char;

int display_screen_state = 0; // display auxiliary variable, through it know which screen is currently being displayed
int button_1_is_pressed, button_2_is_pressed, button_3_is_pressed; // button auxiliary variables
int past_state_CLK_encoder, present_state_CLK_encoder, present_state_DT_encoder, sw_encod_is_pressed, sw_encod_is_pressed_feedback ; // rotatory encoder auxiliary variables 

void i2c_master_init(void)
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void reset_buttons_and_encoder_value(void)
{
    button_1_is_pressed = 0; button_2_is_pressed = 0; button_3_is_pressed = 0; sw_encod_is_pressed = 0; sw_encod_is_pressed_feedback = 0;
}

int encoder_variation_display(i2c_lcd1602_info_t * lcd, int aux, int top_limit, int bottom_limit, int column, char aux_string[], char aux_char)
{
    while(!sw_encod_is_pressed || !sw_encod_is_pressed_feedback) 
    {
        sw_encod_is_pressed = sw_encoder_is_pressed(SW_encoder);
        present_state_CLK_encoder = verify_clk_encoder_level(CLK_encoder);
        if(past_state_CLK_encoder != present_state_CLK_encoder)
        {
            present_state_DT_encoder = verify_clk_encoder_level(DT_encoder);
            if (present_state_CLK_encoder != present_state_DT_encoder) 
            {   
                // all these if/else were needed cause we are using the same function for screens 0, 2 and 3
                if(bottom_limit == 0) {if(aux < top_limit) aux++;}
                else if(aux<100) {if(aux >= bottom_limit && aux <= top_limit) aux++;}
                else {if(aux >= bottom_limit && aux < top_limit) aux += 100;}

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
                // all these if/else were needed cause we are using the same function for screens 0, 2 and 3
                if(bottom_limit == 0) {if(aux > bottom_limit) aux--;}
                else if(aux<100) {if(aux > bottom_limit && aux <= top_limit + 1) aux--;}     
                else {if(aux > bottom_limit && aux <= top_limit) aux -= 100;}   

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
            past_state_CLK_encoder = verify_clk_encoder_level(CLK_encoder);
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // adding this delay in pratice helped with encoder precision, the delay can not be much bigger, otherwise encoder gets to slow
        sw_encod_is_pressed_feedback = sw_encoder_is_pressed(SW_encoder); // adding this second variable for confirming sw_encoder was pressed helped a lot with precision
    }
    reset_buttons_and_encoder_value();
    i2c_lcd1602_clear(lcd);
    vTaskDelay(pdMS_TO_TICKS(1000));
    return aux;
}

void update_time_values(i2c_lcd1602_info_t * lcd, int rtc_hour_min_update, char rtc_hour_min_string[], char rtc_hour_min_char, int column)
{
    if(rtc_hour_min_update >= 10) // displays the hour or minute that was stored from rtc value (in this example it's value is not updating yet)
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
    if(column == 5) i2c_lcd1602_write_char(lcd, ':'); // column == 5 means the value being displayed it's hour, so we add ":" on the right
}

void turn_off_display(i2c_lcd1602_info_t * lcd)
{   
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
}

int display_go_screen_0_hour(i2c_lcd1602_info_t * lcd)
{
    display_screen_state = 0;
    i2c_lcd1602_set_backlight(lcd, true); // turn on backlight
    i2c_lcd1602_clear(lcd);// clear display
    i2c_lcd1602_move_cursor(lcd, 1, 0); // move cursor to column 1 row 0
    i2c_lcd1602_write_string(lcd, "Insira a hora:"); 
    i2c_lcd1602_move_cursor(lcd, 7, 1); 
    i2c_lcd1602_write_string(lcd, "00");
    hour_aux = encoder_variation_display(lcd, hour_aux, 23, 0, 7, hour_aux_string, hour_aux_char);
    return hour_aux;
}

int display_go_screen_0_minutes(i2c_lcd1602_info_t * lcd)
{
    display_screen_state = 0;
    i2c_lcd1602_clear(lcd);
    i2c_lcd1602_move_cursor(lcd, 4, 0);
    i2c_lcd1602_write_string(lcd, "Minutos:");
    i2c_lcd1602_move_cursor(lcd, 7, 1); 
    i2c_lcd1602_write_string(lcd, "00"); 
    min_aux = encoder_variation_display(lcd, min_aux, 59, 0, 7, min_aux_string, min_aux_char);
    return min_aux;
}

void display_go_screen_1(i2c_lcd1602_info_t * lcd, int hour, int min)
{
    // will freeze screen updating rtc values until either button_1, button_2 or button_3 is pressed **updating rtc values not implemented yet**
    display_screen_state = 1;
    i2c_lcd1602_clear(lcd);
    i2c_lcd1602_move_cursor(lcd, 3, 0);
    i2c_lcd1602_write_string(lcd, "Bem vindo!");
    while(!button_1_is_pressed && !button_3_is_pressed && !button_2_is_pressed)
    {
        button_1_is_pressed = button_is_pressed(BUTTON_1); 
        button_2_is_pressed = button_is_pressed(BUTTON_2); 
        button_3_is_pressed = button_is_pressed(BUTTON_3);
        update_time_values(lcd, hour, rtc_hour_value_string, rtc_hour_value_char, 5);
        update_time_values(lcd, min, rtc_min_value_string, rtc_min_value_char, 8);
    } 
    i2c_lcd1602_clear(lcd);
}

int display_go_screen_2(i2c_lcd1602_info_t * lcd)
{   
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
    reset_buttons_and_encoder_value();
    i2c_lcd1602_clear(lcd);
    vTaskDelay(pdMS_TO_TICKS(1000));
    return periodicity_aux;
}

int display_go_screen_3(i2c_lcd1602_info_t * lcd)
{
    display_screen_state = 3;
    i2c_lcd1602_move_cursor(lcd, 2, 0);
    i2c_lcd1602_write_string(lcd, "Quantidade:");
    i2c_lcd1602_move_cursor(lcd, 7, 1);
    i2c_lcd1602_write_string(lcd, "gramas");
    sprintf(food_aux_string, "%d", food_aux); 
    i2c_lcd1602_move_cursor(lcd, 3, 1);
    i2c_lcd1602_write_string(lcd, food_aux_string);
    reset_buttons_and_encoder_value();
    past_state_CLK_encoder = verify_clk_encoder_level(CLK_encoder);
    food_aux = encoder_variation_display(lcd, food_aux, 1000, 100, 3, food_aux_string, '$'); // since food_aux is always higher than 100g we don't need to use single char in this function
    i2c_lcd1602_clear(lcd);
    i2c_lcd1602_move_cursor(lcd, 0, 0);
    i2c_lcd1602_write_string(lcd, "Alteracao feita!");    
    i2c_lcd1602_move_cursor(lcd, 4, 1);
    i2c_lcd1602_write_string(lcd, "Obrigado");    
    vTaskDelay(pdMS_TO_TICKS(1000));
    turn_off_display(lcd);
    reset_buttons_and_encoder_value();
    return food_aux;
}

void lcd1602_task(void * pvParameter)
{
    // i2c and devices initialization
    i2c_master_init();
    clk_encoder_init(CLK_encoder); 
	dt_encoder_init(DT_encoder);
    sw_encoder_init(SW_encoder);
    button_init(BUTTON_1);
    button_init(BUTTON_2);
    button_init(BUTTON_3);
    
    // setting up lcd address and I2C_MASTER_NUM
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t lcd_address = 0x27; // configuring lcd i2c address

    // setting up smbus
    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, lcd_address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_PERIOD_MS);

    // set up the LCD1602 device with backlight off, (2,32,16) = (rows, all columns, visible columns)  
    i2c_lcd1602_info_t * lcd_info = i2c_lcd1602_malloc(); 
    i2c_lcd1602_init(lcd_info, smbus_info, true, 2, 32, 16); 

    while(1)
    {
        reset_buttons_and_encoder_value();
        rtc_hour_value = display_go_screen_0_hour(lcd_info);
        rtc_min_value = display_go_screen_0_minutes(lcd_info);
        while(1) // after RTC values are set in screen 0 this task will run indefinitely
        {
            if(display_screen_state == 5) // if while(1) was "restarted" button_1 must be pressed to advance
            {
                while(!button_1_is_pressed) button_1_is_pressed = button_is_pressed(BUTTON_1);
                i2c_lcd1602_set_backlight(lcd_info, true); 
                reset_buttons_and_encoder_value();
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            
            display_go_screen_1(lcd_info, rtc_hour_value, rtc_min_value);
            if (button_3_is_pressed) turn_off_display(lcd_info); // when in screen 1 button 3 will turn off backlight, going to stand by mode
            else if(button_1_is_pressed) periodicity_hour_value = display_go_screen_2(lcd_info); // when in screen 1 button 1 will advance, moving to screen 2
            else break; // when in screen 1 if button 2 is pressed you can configure RTC values again
            if(display_screen_state == 2) food_quantitiy_value = display_go_screen_3(lcd_info);  
        }
    }
    vTaskDelete(NULL);
}
