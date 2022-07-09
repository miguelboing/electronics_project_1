/*
 * encoder.h
 *
 *  Created on: 26/05/2022
 *
 */

#ifndef ENCODER_H_
#define ENCODER_H_

typedef enum{CLK_encoder = GPIO_NUM_15} clk_encoder_e;
typedef enum{DT_encoder = GPIO_NUM_14} dt_encoder_e;
typedef enum{SW_encoder = GPIO_NUM_13} sw_encoder_e;

TaskHandle_t task_handle_sw_encoder = NULL;
TaskHandle_t task_handle_rotation_encoder = NULL;

void clk_encoder_init(clk_encoder_e encod);
void dt_encoder_init(dt_encoder_e encod);
void sw_encoder_init(sw_encoder_e encod);

int verify_clk_encoder_level(clk_encoder_e encod);
int verify_dt_encoder_level(dt_encoder_e encod);
int sw_encoder_is_pressed(sw_encoder_e encod);

#endif /* ENCODER_H_ */
