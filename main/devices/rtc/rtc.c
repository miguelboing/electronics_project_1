#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include <math.h>

#include "rtc.h"

unsigned int time;
unsigned int time_min;
unsigned int time_hour;

void rtc_update_time(void)
{
    time++;
    if (time > 59) 
    {
        time = 0; time_min++;
        if(time_min > 59)
        {
            time_min = 0; time_hour++;
            if(time_hour > 23) time_hour = 0;
        }
    }
    // in practice 10 seconds of variation were measured in 16 minutes (960 sec) of test, so i anticipated vTaskDelay in 10/960 seconds, that is approximately 10ms.
    vTaskDelay(990 / portTICK_PERIOD_MS); 
}

int get_time_sec(void) {return time;}
int get_time_min(void) {return time_min;}
int get_time_hour(void) {return time_hour;}

void update_time_min(int minutes) {time_min = minutes;}
void update_time_hour(int hours) {time_hour = hours;}

void reset_rtc(void)
{
    time = 0;
    time_hour = 0;
    time_min = 0;
}
