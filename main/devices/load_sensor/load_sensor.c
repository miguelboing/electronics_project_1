#include "load_sensor.h"
#include "hx711.h"


int32_t weight = 0;

void load_sensor_init(void)
{
    hx711_t dev;
    dev.dout = GPIO_NUM_12;
    dev.pd_sck = GPIO_NUM_27;
    dev.gain = HX711_GAIN_A_128;

  if(hx711_init(&dev) == ESP_OK)
  {
      printf("Successfully configured HX711 ADC!\n");
  }
}

int load_sensor_get_weight(void)
{
    hx711_t dev;
    dev.dout = GPIO_NUM_12;
    dev.pd_sck = GPIO_NUM_27;
    dev.gain = HX711_GAIN_A_64;

    hx711_wait(&dev, 1215752190);
    if(hx711_read_data(&dev, &weight) == ESP_OK)
    {
        printf("Consegui realizar leitura");
    }

    printf("\nValor medido: %d \n", weight);
    return weight;
}