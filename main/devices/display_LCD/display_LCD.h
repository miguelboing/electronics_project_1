/*
 * display_LCD.h
 *
 *  Created on: 18 de mai de 2022
 *
 */

#ifndef DISPLAY_LCS_H_
#define DISPLAY_LCS_H_

/*Components*/
#include "../../components/esp32-i2c-lcd1602/i2c-lcd1602.h"

#define I2C_MASTER_SCL_IO           GPIO_NUM_22      /* GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           GPIO_NUM_21      /* GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                /* I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000           /* I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                /* I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                /* I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

void i2c_master_init(void);

int get_display_screen_state(void);
int get_start_RTC(void);

void init_RTC(void);
void stop_RTC(void);

void reset_buttons_and_encoder_value(void);
void update_time_values(i2c_lcd1602_info_t * lcd, int rtc_hour_min_update, char rtc_hour_min_string[], char rtc_hour_min_char, int column);
void turn_off_display(i2c_lcd1602_info_t * lcd);
void display_go_screen_1(i2c_lcd1602_info_t * lcd, int hour, int min);

int encoder_variation_display(i2c_lcd1602_info_t * lcd, int aux, int top_limit, int bottom_limit, int column, char aux_string[], char aux_char);
int display_go_screen_0_hour(i2c_lcd1602_info_t * lcd, int first_time);
int display_go_screen_0_minutes(i2c_lcd1602_info_t * lcd, int first_time);
int display_go_screen_2(i2c_lcd1602_info_t * lcd);
int display_go_screen_3(i2c_lcd1602_info_t * lcd);

void lcd1602_task(void * pvParameter);

#endif /* DISPLAY_LCS_H_ */
