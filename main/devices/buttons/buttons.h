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

void reset_buttons_state(void);

void update_button_1_state(void);
void update_button_2_state(void);
void update_button_3_state(void);

int button_is_pressed(button_e button);

int get_button_1_state(void);
int get_button_2_state(void);
int get_button_3_state(void);

#endif /* BUTTONS_H_ */
