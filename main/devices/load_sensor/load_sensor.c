/*
 * load_sensor.c
 *
 *  Created on: 16/06/2022
 *
 */

#include "load_sensor.h"

#include "hx711.h"
#include "esp_log.h"

int32_t weight = 0;
hx711_t  dev;

static const char* TAG = "Load sensor Feedback";

void load_sensor_init(void)
{
    dev.dout = GPIO_NUM_12;
    dev.pd_sck = GPIO_NUM_27;
    dev.gain = HX711_GAIN_A_64;

    if(hx711_init(&dev) == ESP_OK) ESP_LOGI(TAG, "Successfully configured HX711 ADC!\n");
    else ESP_LOGE(TAG, "HX711 ADC was not configured sucessfully!\n"); 
}

int load_sensor_get_weight(void)
{
    hx711_wait(&dev, 1215752190); /* Wait hx711 to be ready */

    if(hx711_read_average(&dev, 5, &weight) == ESP_OK) /* Get average from 5 measurements */
    {
        ESP_LOGI(TAG,"\nMeasured Weight: %d \n", weight);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to measure weight!");
        weight = INTMAX_MIN;
    }

    return weight;
}
