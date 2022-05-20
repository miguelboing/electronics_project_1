/*
 * proximity_sensor.h
 *
 *  Created on: 18 de mai de 2022
 *
 */

#ifndef PROXIMITY_SENSOR_H_
#define PROXIMITY_SENSOR_H_

#define IR_out 22 /* GPIO 22*/

void proximity_sensor_init(void);

bool proximity_sensor_get_presence(void);

#endif /* PROXIMITY_SENSOR_H_ */
