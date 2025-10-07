#include <ctime>

#include "shaders.hpp"

uint32_t color(uint8_t r, uint8_t g, uint8_t b);
uint32_t color_hsv(uint16_t h, uint8_t s = 255, uint8_t v = 255);



const shader_func_t shader_hue_shift = [](int x, int y) -> uint32_t {
  auto hue = std::time(nullptr);
  return color_hsv((hue + (x << 7)) % UINT16_MAX);
};

const shader_func_t shader_hue_shift_dim = [](int x, int y) -> uint32_t {
  auto hue = std::time(nullptr);
  return color_hsv((hue + (x << 7)) % UINT16_MAX, 255, 127);
};

const shader_func_t shader_white = [](int x, int y) -> uint32_t {
  return color(255, 255, 255);
};

const shader_func_t shader_red = [](int x, int y) -> uint32_t {
  return color(255, 0, 0);
};

const shader_func_t shader_yellow = [](int x, int y) -> uint32_t {
  return color(255, 255, 0);
};

const shader_func_t shader_green = [](int x, int y) -> uint32_t {
  return color(0, 255, 0);
};

const shader_func_t shader_turquoise = [](int x, int y) -> uint32_t {
  return color(0, 255, 255);
};

const shader_func_t shader_blue = [](int x, int y) -> uint32_t {
  return color(0, 0, 255);
};

const shader_func_t shader_purple = [](int x, int y) -> uint32_t {
  return color(255, 0, 255);
};