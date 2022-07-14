/*
 * rtc.h
 *
 *  Created on: 18 de mai de 2022
 *
 */

#ifndef RTC_H_
#define RTC_H_

void rtc_update_time(void);

int rtc_get_time_sec(void);

int rtc_get_time_min(void);

int rtc_get_time_hour(void);

int rtc_get_time_abs(void);

void rtc_update_time_hour(unsigned int hours);

void rtc_update_time_min(unsigned int minutes);

void rtc_reset(void);

void rtc_reset_abs(void);

void rtc_task(void * pvParameter);

#endif /* RTC_H_ */
