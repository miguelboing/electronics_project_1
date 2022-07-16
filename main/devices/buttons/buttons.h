/*
 * buttons.h
 *
 *  Created on: 18/05/2022
 *
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

typedef enum
{
    BUTTON_1 = GPIO_NUM_32,          
    BUTTON_2 = GPIO_NUM_33,            
    BUTTON_3 = GPIO_NUM_25         
} button_e;

void button_init(button_e button);

void button_reset_states(void);

void button_update_state(button_e button);

int button_is_pressed(button_e button);

int button_get_state(button_e button);

#endif /* BUTTONS_H_ */
