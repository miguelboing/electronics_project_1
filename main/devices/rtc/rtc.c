#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rtc.h"

static unsigned int time_sec;
static unsigned int time_min;
static unsigned int time_hour;
static unsigned int abs_time_sec;

void rtc_update_time(void)
{
    time_sec++;
    abs_time_sec++;
    if (time_sec > 59)
    {
        time_sec = 0;
        time_min++;

        if(time_min > 59)
        {
            time_min = 0;
            time_hour++;
            if(time_hour > 23)
            {
                time_hour = 0;
                abs_time_sec = 0;
            }
        }
    }

    vTaskDelay(990 / portTICK_PERIOD_MS); /* xTicksToDelay = 990 to compensate variation */
}

int rtc_get_time_abs(void)
{
    return time_sec;
}

int rtc_get_time_sec(void)
{
    return time_sec;
}

int rtc_get_time_min(void)
{
    return time_min;
}

int rtc_get_time_hour(void)
{
    return time_hour;
}

void rtc_update_time_min(unsigned int minutes)
{
    time_min = minutes;
}

void rtc_update_time_hour(unsigned int hours)
{
    time_hour = hours;
}

void rtc_reset(void)
{
    time_sec = 0U;
    time_hour = 0U;
    time_min = 0U;
}

void rtc_reset_abs(void)
{
    abs_time_sec = 0;
}