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

TaskHandle_t task_handle_button_1 = NULL;
TaskHandle_t task_handle_button_2 = NULL;
TaskHandle_t task_handle_button_3 = NULL;

void button_init(button_e button);

void task_button_1(void* param);
void task_button_2(void* param);
void task_button_3(void* param);

#endif /* BUTTONS_H_ */
