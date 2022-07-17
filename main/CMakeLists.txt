idf_component_register(SRCS "main.c" "devices/buttons/buttons.c" "devices/display_LCD/display_LCD.c" 
                            "devices/encoder/encoder.c" "devices/load_sensor/load_sensor.c" 
                            "devices/proximity_sensor/proximity_sensor.c" "devices/rtc/rtc.c"
                            "devices/ac_check/ac_check.c" "devices/buzzer/buzzer.c"
                            "devices/servo_motor/servo_motor.c" "components/esp32-smbus/smbus.c"
                            "components/esp32-i2c-lcd1602/i2c-lcd1602.c"                            
                    INCLUDE_DIRS ".")