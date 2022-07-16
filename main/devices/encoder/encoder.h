/*
 * encoder.h
 *
 *  Created on: 26/05/2022
 *
 */

#ifndef ENCODER_H_
#define ENCODER_H_

typedef enum
{
    CLK_encoder = GPIO_NUM_15,         
    DT_encoder = GPIO_NUM_14,
    SW_encoder = GPIO_NUM_13,
    CLK_past_state = 1,      
    SW_encoder_feedback = 0,
} encoder_e;

void encoder_init(encoder_e encoder);
void encoder_update_state(encoder_e encoder);

int encoder_verify_level(encoder_e encoder);
int encoder_get_state(encoder_e encoder);

void reset_sw_encoder_state(void);

#endif /* ENCODER_H_ */
