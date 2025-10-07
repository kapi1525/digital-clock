#pragma once

#include <cstdint>
#include <driver/gpio.h>



extern const int LED_PIN;
extern const int LED_MATRIX_WIDTH;
extern const int LED_MATRIX_HEIGHT;
extern const int LED_COUNT;

extern const std::uint8_t BRIGHTNESS_MAX;
extern const std::uint8_t BRIGHTNESS_AUTO;
extern const std::uint8_t BRIGHTNESS_USER_MAX;
extern const std::uint8_t BRIGHTNESS_BASE;

extern const gpio_num_t BUTTON_MAIN_PIN;
extern const gpio_num_t BUTTON_L_PIN;
extern const gpio_num_t BUTTON_R_PIN;

extern const gpio_num_t PHOTORESISTOR_PIN;

extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

extern const char* TIMEZONE;