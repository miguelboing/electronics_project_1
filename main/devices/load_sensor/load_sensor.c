/*
 * load_sensor.c
 *
 *  Created on: 16/06/2022
 *
 */

#include "load_sensor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hx711.h"
#include "esp_log.h"

/* 
    both values below were measured in practice using a 300 grams
    weight for reference, the formula used to calculate the conversion
    factor was LOAD_CELL_CONVERSION_FACTOR = (F1-F0)/(R1-R0), where:
    F1 -> 300 grams weight;
    F0 ->   0 grams weight;
    R1 -> 300 grams load sensor reading;
    R0 -> 300 grams load sensor reading;
*/
#define LOAD_CELL_300_GRAMS_VALUE   85575040
#define LOAD_CELL_CONVERSION_FACTOR 0.001175

int32_t weight = 0;
float weight_g = 0;
hx711_t  dev;

static const char* TAG_LOAD_SENSOR = "Load sensor Feedback";

void load_sensor_init(void)
{
    dev.dout = GPIO_NUM_12;
    dev.pd_sck = GPIO_NUM_27;
    dev.gain = HX711_GAIN_A_128;

    if(hx711_init(&dev) == ESP_OK) ESP_LOGI(TAG_LOAD_SENSOR, "Successfully configured HX711 ADC!\n");
    else ESP_LOGE(TAG_LOAD_SENSOR, "HX711 ADC was not configured sucessfully!\n"); 
}

int load_sensor_get_weight(void)
{
    hx711_wait(&dev, 1215752190); /* Wait hx711 to be ready */

    if(hx711_read_average(&dev, 50, &weight) == ESP_OK) /* Get average from 50 measurements */
    {
        weight_g = (((float) weight) - LOAD_CELL_300_GRAMS_VALUE) * (LOAD_CELL_CONVERSION_FACTOR);
        ESP_LOGI(TAG_LOAD_SENSOR,"Measured Weight: %f \n", weight_g);
    }
    else
    {
        ESP_LOGE(TAG_LOAD_SENSOR, "Failed to measure weight!\n");
        vTaskDelay(500 / portTICK_PERIOD_MS); 
    }
    return weight_g;
}