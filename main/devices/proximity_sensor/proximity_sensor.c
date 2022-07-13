#include "proximity_sensor.h"
#include "driver/gpio.h"

#define IR_out GPIO_NUM_4 /* D4 */

void proximity_sensor_init(void)
{
    gpio_config_t proximity_sensor_gpio = {};
    /* Disable Interrupt */
    proximity_sensor_gpio.intr_type = GPIO_INTR_DISABLE;
    /* Set as Input Mode*/
    proximity_sensor_gpio.mode = GPIO_MODE_INPUT;
    /* Enable pullup and disable pulldown*/
    proximity_sensor_gpio.pull_up_en = GPIO_PULLUP_ENABLE;
    proximity_sensor_gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;
    /* Select pin */
    proximity_sensor_gpio.pin_bit_mask = (1ULL << IR_out);
    if (gpio_config(&proximity_sensor_gpio) == ESP_OK)
    {
        printf("Successfully configured proximity sensor!\n");
    }
}
int proximity_sensor_get_presence(void)
{
    return gpio_get_level(IR_out);
}