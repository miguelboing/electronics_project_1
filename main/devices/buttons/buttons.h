/*
 * buttons.h
 *
 *  Created on: 18 de mai de 2022
 *
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

typedef enum
{
    BUTTON_1 = GPIO_NUM_27,          
    BUTTON_2 = GPIO_NUM_32,            
    BUTTON_3 = GPIO_NUM_33         
} button_e;

void button_init(button_e button);

int button_is_pressed(button_e button);

/* void button_enable(button_e button);
   void button_disable(button_e button); */

#endif /* BUTTONS_H_ */
