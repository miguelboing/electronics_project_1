#include <stdio.h>

#include "esp_system.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "driver/gpio.h"

/*Devices */
#include "devices/buttons/buttons.h"
#include "devices/display_LCD/display_LCD.h"
#include "devices/encoder/encoder.h"
#include "devices/load_sensor/load_sensor.h"
#include "devices/proximity_sensor/proximity_sensor.h"
#include "devices/rtc/rtc.h"
#include "devices/servo_motor/servo_motor.h"
#include "devices/ac_check/ac_check.h"

/*Components*/
#include "components/esp32-smbus/smbus.h"

static const char* TAG = "Main Feedback";

int rtc_hour_value, rtc_min_value;
float periodicity_hour_value = 12;
int food_quantity_value = 100;
bool rtc_started = false;

void feeding_time_task(void * pvParameter);
TaskHandle_t feeding_time_task_handle = NULL;
void app_main(void)
{
    xTaskCreatePinnedToCore(&lcd1602_task, "lcd1602_task", 4096,
                            NULL, 1, NULL, 0);

    xTaskCreatePinnedToCore(&rtc_task, "rtc_task", 4096,
                            NULL, 1, NULL, 1);

    xTaskCreatePinnedToCore(&feeding_time_task, "feeding_time_task", 4096,
                            NULL, 2, &feeding_time_task_handle, 0);

    //vTaskStartScheduler();
}

void lcd1602_task(void * pvParameter)
{
    // i2c and devices initialization
    button_init(BUTTON_1); button_init(BUTTON_2); button_init(BUTTON_3);
    display_i2c_master_init();
    encoder_init(CLK_encoder); encoder_init(DT_encoder); encoder_init(SW_encoder);
    load_sensor_init();
    proximity_sensor_init();
    servo_motor_init();
    ac_check_init();


    int first_time_config = 1; // auxiliary variable used in screens 0.0 and 0.1. Will be set to 0 after RTC values are configured for the first time.

    // setting up lcd address and I2C_MASTER_NUM
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t lcd_address = 0x27; 

    // setting up smbus
    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, lcd_address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_PERIOD_MS);

    // set up the LCD1602 device with backlight off, (2,32,16) = (rows, all columns, visible columns)  
    i2c_lcd1602_info_t * lcd_info = i2c_lcd1602_malloc(); 
    i2c_lcd1602_init(lcd_info, smbus_info, true, 2, 32, 16); 

    while(1)
    { 
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Begin RTC reference settings.\n");
        reset_buttons_and_encoder_value();

        rtc_reset();

        rtc_hour_value = display_go_screen_0_hour(lcd_info , first_time_config);
        rtc_update_time_hour(rtc_hour_value);
        ESP_LOGI(TAG, "RTC_hour reference set to %d hours.\n", rtc_hour_value);

        rtc_min_value = display_go_screen_0_minutes(lcd_info, first_time_config);
        rtc_update_time_min(rtc_min_value);
        ESP_LOGI(TAG, "RTC_minutes reference set to %d minutes.\n", rtc_min_value);
        rtc_started = true;

        if(first_time_config != 0) first_time_config = 0;

        while(1) // after RTC values are set in screen 0 this task will run indefinitely, unless button_2 is pressed in screen 1
        {
            if(display_get_screen_state() == 5) // if while(1) was "restarted" button_1 must be pressed to advance
            {
                while(!button_get_state(BUTTON_1))
                {
                    button_update_state(BUTTON_1);
                    vTaskDelay(pdMS_TO_TICKS(50));
                    /* Check for debug mode */
                    button_update_state(BUTTON_3);
                    if(button_get_state(BUTTON_3))
                    {
                        /* Entering debug mode */
                        periodicity_hour_value = 0.0083333; /* Periodicity is 30 seconds */
                        food_quantity_value = 200; /* Food quantity is 200 g */
                        display_go_screen_4_debug_mode(lcd_info); /* Get stuck in debug mode */

                    }
                }
                ESP_LOGI(TAG, "Button_1 was pressed!\n");
                i2c_lcd1602_set_backlight(lcd_info, true); 
                reset_buttons_and_encoder_value();
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            
            display_go_screen_1(lcd_info, rtc_hour_value, rtc_min_value);
            if (button_get_state(BUTTON_3)) // when in screen 1 button 3 will turn off backlight, going to stand by mode
            {
                ESP_LOGI(TAG, "Button_3 was pressed!\n");
                turn_off_display(lcd_info);
            } 
            else if(button_get_state(BUTTON_1)) // when in screen 1 button 1 will advance, moving to screen 2
            {
                ESP_LOGI(TAG, "Button_1 was pressed!\n");
                periodicity_hour_value =  (float) display_go_screen_2(lcd_info);
                ESP_LOGI(TAG, "Periodicity value set to %f hours.\n", periodicity_hour_value);

                rtc_reset();
            } 
            else 
            {
                ESP_LOGI(TAG, "Button_2 was pressed!\n");
                break; // when in screen 1 if button 2 is pressed you can configure RTC values again
            }
            
            if(display_get_screen_state() == 2)
            {
                food_quantity_value = display_go_screen_3(lcd_info);
                ESP_LOGI(TAG, "Food_quantity value set to %d grams.\n", food_quantity_value);
            }
        }
    }
    vTaskDelete(NULL);
}

void rtc_task(void * pvParameter)
{

    while(1)
    {
        if(rtc_started)
        {
            rtc_update_time();

            ESP_LOGI(TAG, "Time %d:%d:%d", rtc_get_time_hour(), rtc_get_time_min(), rtc_get_time_sec());
            ESP_LOGI(TAG, "| Absolute Time %d\n", rtc_get_time_abs());

            if ((rtc_get_time_abs() > (periodicity_hour_value * 60 * 60)) && (feeding_time_task_handle != NULL)) {
                vTaskResume(feeding_time_task_handle);
                rtc_reset_abs();
            }
        }
        else 
        {
            ESP_LOGI(TAG, "RTC is frozen \n");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    vTaskDelete(NULL);
}

void feeding_time_task(void * pvParameter)
{
    vTaskSuspend(NULL);

    while(1)
    {
        ESP_LOGI(TAG, "Starting feeding task! \n");

        while (load_sensor_get_weight() > 10000) //After load sensor calibration change to food_quantity_value
        {
            while(!proximity_sensor_get_presence())  /* Check if dog is nearby */
            {
                if (ac_check_power())
                {
                    servo_motor_open();
                }
                else
                {
                    // Buzzer apita
                }
            }
            if (ac_check_power())
            {
                servo_motor_close();
            }
        }

        ESP_LOGI(TAG, "Ending feeding task! \n");
        rtc_reset_abs();
        vTaskSuspend(NULL);

    }
    vTaskDelete(NULL);
}