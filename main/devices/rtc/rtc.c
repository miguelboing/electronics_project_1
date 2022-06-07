#include <math.h>

#include "rtc.h"

static time_t time;
static time_hour_t time_hour;
static time_min_t time_min;

void rtc_update_time(void)
{
    time += 1;
    if (time >= 1440)
    {
        time = 0;
    }
}

time_hour_t rtc_time_hour(void)
{
    time_hour = floor(time_hour/60);
    return time_hour;
}

time_min_t rtc_time_min(void)
{
    time_min = 60 * (time % 60);
    return time_min;
}