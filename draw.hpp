#pragma once

#include "config.hpp"
#include "shaders.hpp"



bool display_init();
void display_clear();
void display_show();

void display_set_brightness(std::uint8_t b);

int xy_to_pixel_index(int x, int y);

uint32_t color(uint8_t r, uint8_t g, uint8_t b);
uint32_t color_hsv(uint16_t h, uint8_t s = 255, uint8_t v = 255);

void set_pixel(int x, int y, uint32_t c);
uint32_t get_pixel(int x, int y);

void fill_rect(int x, int y, int w, int h, uint32_t c);
void fill_rect_shaded(int x, int y, int w, int h, shader_func_t shader);

int draw_glyph(int index, int x, int y, shader_func_t shader);
void draw_char(char c, int x, int y, shader_func_t shader);
void draw_digit(int d, int x, int y, shader_func_t shader);
void draw_text(const char *str, int x, int y, shader_func_t shader);
void draw_text_scrolling(const char *str, int x, int y, int w, shader_func_t shader);
void draw_glyph_test(float delta);

void blit_img(int target_x, int target_y, int w, int h, const unsigned char img[]);