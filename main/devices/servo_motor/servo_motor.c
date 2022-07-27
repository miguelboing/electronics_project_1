/*
 * servo_motor.c
 *
 *  Created on: 18 de mai de 2022
 *
 */

#include "servo_motor.h"
#include "iot_servo.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char* TAG_SERVO = "Servo motor Feedback";

#define sMOTOR GPIO_NUM_19 

void servo_motor_init(void) {

    servo_config_t servo_cfg = {
            .max_angle = 180,
            .min_width_us = 500,
            .max_width_us = 2500,
            .freq = 50,
            .timer_number = LEDC_TIMER_0,
            .channels = {
                    .servo_pin = {
                            sMOTOR,
                    },
                    .ch = {
                            LEDC_CHANNEL_5,
                    },
            },
            .channel_number = 1,
    };

    if(iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg) == ESP_OK) ESP_LOGI(TAG_SERVO, "Successfully configured servo motor!\n");
    else ESP_LOGE(TAG_SERVO, "Servo motor was not configured sucessfully!");
}

/* Since our servo was internally modified to rotate more than 360 degrees, we need to
   terminate it's GPIO terminal in order to stop it, otherwise the component would keep
   vibrating, compromising our feeder structure  
*/
void servo_motor_terminate(void)
{
    if(gpio_reset_pin(sMOTOR) == ESP_OK) ESP_LOGI(TAG_SERVO, "Servo motor's GPIO was terminated!\n");
    else ESP_LOGE(TAG_SERVO, "Servo motor's GPIO  was not terminated sucessfully!");
}

void servo_motor_start_spin(void)
{
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 5, 180.0f);
}
