/*
 * rtc.h
 *
 *  Created on: 18 de mai de 2022
 *
 */

#ifndef RTC_H_
#define RTC_H_

void rtc_update_time(void);

int get_time_sec(void);
int get_time_min(void);
int get_time_hour(void);

void update_time_hour(int hours);
void update_time_min(int minutes);

void reset_rtc(void);

void rtc_task(void * pvParameter);

#endif /* RTC_H_ */
