#include "config.hpp"



const int LED_PIN = 18;
const int LED_MATRIX_WIDTH = 32;
const int LED_MATRIX_HEIGHT = 8;
const int LED_COUNT = (LED_MATRIX_WIDTH * LED_MATRIX_HEIGHT);

const std::uint8_t BRIGHTNESS_MAX = 7;
const std::uint8_t BRIGHTNESS_AUTO = BRIGHTNESS_MAX;
const std::uint8_t BRIGHTNESS_USER_MAX = 6;
const std::uint8_t BRIGHTNESS_BASE = BRIGHTNESS_AUTO;

const gpio_num_t BUTTON_MAIN_PIN = GPIO_NUM_0;
const gpio_num_t BUTTON_L_PIN = GPIO_NUM_21;
const gpio_num_t BUTTON_R_PIN = GPIO_NUM_22;

const gpio_num_t PHOTORESISTOR_PIN = GPIO_NUM_1;

const char* WIFI_SSID = "CHANGE";
const char* WIFI_PASS = "ME";

const char* TIMEZONE = "CET-1CEST,M3.5.0,M10.5.0/3";