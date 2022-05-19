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
    BUTTON_0=0,          /**< Button 0. */
    BUTTON_1,            /**< Button 1. */
    BUTTON_2             /**< BUtton 2. */
} button_e;


void button_init(button_e button);

void button_enable(button_e button);

void button_disable(button_e button);

#endif /* BUTTONS_H_ */
