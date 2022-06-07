/*
 * rtc.h
 *
 *  Created on: 18 de mai de 2022
 *
 */

#ifndef RTC_H_
#define RTC_H_
typedef unsigned int time_t;
typedef int time_hour_t;
typedef int time_min_t;

void rtc_update_time(void);

time_hour_t rtc_time_hour(void);

time_min_t rtc_time_min(void);

#endif /* RTC_H_ */
