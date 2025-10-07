#include <cstring>
#include <cstdio>

#include <Adafruit_NeoPixel.h>

#include "draw.hpp"



Adafruit_NeoPixel& matrix() {
  static Adafruit_NeoPixel m(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
  return m;
}



bool display_init() {
    return matrix().begin();
}

void display_clear() {
    matrix().clear();
}

void display_show() {
    matrix().show();
}


void display_set_brightness(std::uint8_t b) {
    matrix().setBrightness(b);
}



int xy_to_pixel_index(int x, int y) {
  if (x % 2) {
    return (x * LED_MATRIX_HEIGHT) + LED_MATRIX_HEIGHT - 1 - y;
  } else {
    return (x * LED_MATRIX_HEIGHT) + y;
  }
}

uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
  return matrix().Color(r, g, b);
}

uint32_t color_hsv(uint16_t h, uint8_t s, uint8_t v) {
  return matrix().ColorHSV(h, s, v);
}



void set_pixel(int x, int y, uint32_t c) {
  matrix().setPixelColor(xy_to_pixel_index(x, y), c);
}

uint32_t get_pixel(int x, int y) {
  return matrix().getPixelColor(xy_to_pixel_index(x, y));
}



void fill_rect(int x, int y, int w, int h, uint32_t c) {
  for (int ix = x; ix < x + w; ix++) {
    for (int iy = y; iy < y + h; iy++) {
      set_pixel(ix, iy, c);
    }
  }
}

void fill_rect_shaded(int x, int y, int w, int h, shader_func_t shader) {
  for (int ix = x; ix < x + w; ix++) {
    for (int iy = y; iy < y + h; iy++) {
      set_pixel(ix, iy, shader(ix, iy));
    }
  }
}



// On success 0, if index = -1 then returns number of glyphs, on error -1
int draw_glyph(int index, int x, int y, shader_func_t shader) {
  static const std::array<uint8_t, 8 * (10 + 26 + 26)> GLYPHS = {
    // 0
    0b01110,
    0b10001,
    0b10001,
    0b10101,
    0b10101,
    0b10001,
    0b10001,
    0b01110,
    // 1
    0b00100,
    0b01100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b01110,
    // 2
    0b01110,
    0b10001,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b10000,
    0b11111,
    // 3
    0b01110,
    0b10001,
    0b00001,
    0b00110,
    0b00001,
    0b00001,
    0b10001,
    0b01110,
    // 4
    0b10000,
    0b10000,
    0b10010,
    0b11111,
    0b00010,
    0b00010,
    0b00010,
    0b00010,
    // 5
    0b11111,
    0b10000,
    0b11110,
    0b00001,
    0b00001,
    0b00001,
    0b10001,
    0b01110,
    // 6
    0b01110,
    0b10001,
    0b10000,
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    // 7
    0b11111,
    0b00001,
    0b00010,
    0b00100,
    0b00100,
    0b01000,
    0b01000,
    0b01000,
    // 8
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    // 9
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b01111,
    0b00001,
    0b10001,
    0b01110,
    // A (10)
    0b00100,
    0b01010,
    0b10001,
    0b10001,
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    // B
    0b11110,
    0b10001,
    0b10001,
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b11110,
    // C
    0b01110,
    0b10001,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10001,
    0b01110,
    // D
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11110,
    // E
    0b11111,
    0b10000,
    0b10000,
    0b11110,
    0b10000,
    0b10000,
    0b10000,
    0b11111,
    // F
    0b11111,
    0b10000,
    0b10000,
    0b11110,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    // G
    0b01110,
    0b10001,
    0b10000,
    0b10111,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    // H
    0b10001,
    0b10001,
    0b10001,
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    // I
    0b01110,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b01110,
    // J
    0b11111,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b10001,
    0b01110,
    // K
    0b10001,
    0b10010,
    0b10100,
    0b11000,
    0b10100,
    0b10010,
    0b10001,
    0b10001,
    // L
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b11111,
    // M
    0b10001,
    0b11011,
    0b10101,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    // N
    0b10001,
    0b11001,
    0b10101,
    0b10011,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    // O
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    // P
    0b11110,
    0b10001,
    0b10001,
    0b11110,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    // Q
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10101,
    0b10010,
    0b01101,
    // R
    0b11110,
    0b10001,
    0b10001,
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    // S
    0b01110,
    0b10001,
    0b10000,
    0b01110,
    0b00001,
    0b00001,
    0b10001,
    0b01110,
    // T
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    // U
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    // V
    0b10001,
    0b10001,
    0b10001,
    0b01010,
    0b01010,
    0b01010,
    0b00100,
    0b00100,
    // W
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10101,
    0b10101,
    0b10101,
    0b01010,
    // X
    0b10001,
    0b01010,
    0b01010,
    0b00100,
    0b01010,
    0b01010,
    0b10001,
    0b10001,
    // Y
    0b10001,
    0b10001,
    0b01010,
    0b01010,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    // Z (10 + 26, 36)
    0b11111,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b10000,
    0b10000,
    0b11111,
    // a
    0b00000,
    0b00000,
    0b01110,
    0b00001,
    0b01111,
    0b10001,
    0b10001,
    0b01111,
    // b
    0b10000,
    0b10000,
    0b10000,
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b11110,
    // c
    0b00000,
    0b00000,
    0b01110,
    0b10001,
    0b10000,
    0b10000,
    0b10001,
    0b01110,
    // d
    0b00001,
    0b00001,
    0b00001,
    0b01111,
    0b10001,
    0b10001,
    0b10001,
    0b01111,
    // e
    0b00000,
    0b00000,
    0b01110,
    0b10001,
    0b11111,
    0b10000,
    0b10001,
    0b01110,
    // f
    0b00011,
    0b00100,
    0b00100,
    0b01110,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    // g
    0b00000,
    0b00000,
    0b01111,
    0b10001,
    0b10001,
    0b01111,
    0b00001,
    0b01110,
    // h
    0b10000,
    0b10000,
    0b10000,
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    // i
    0b00000,
    0b00100,
    0b00000,
    0b01100,
    0b00100,
    0b00100,
    0b00100,
    0b01110,
    // j
    0b00010,
    0b00000,
    0b00110,
    0b00010,
    0b00010,
    0b00010,
    0b10010,
    0b01100,
    // k
    0b10000,
    0b10000,
    0b10001,
    0b10010,
    0b11100,
    0b10100,
    0b10010,
    0b10001,
    // l
    0b11000,
    0b01000,
    0b01000,
    0b01000,
    0b01000,
    0b01000,
    0b01000,
    0b00111,
    // m
    0b00000,
    0b00000,
    0b11110,
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    // n
    0b00000,
    0b00000,
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    // o
    0b00000,
    0b00000,
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    // p
    0b00000,
    0b00000,
    0b11110,
    0b10001,
    0b10001,
    0b11110,
    0b10000,
    0b10000,
    // q
    0b00000,
    0b00000,
    0b01111,
    0b10001,
    0b10001,
    0b01111,
    0b00001,
    0b00001,
    // r
    0b00000,
    0b00000,
    0b10110,
    0b11000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    // s
    0b00000,
    0b00000,
    0b01110,
    0b10001,
    0b01000,
    0b00110,
    0b10001,
    0b01110,
    // t
    0b00000,
    0b00100,
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00011,
    // u
    0b00000,
    0b00000,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b01111,
    // v
    0b00000,
    0b00000,
    0b10001,
    0b10001,
    0b01010,
    0b01010,
    0b00100,
    0b00100,
    // w
    0b00000,
    0b00000,
    0b10001,
    0b10001,
    0b10001,
    0b10101,
    0b10101,
    0b01010,
    // x
    0b00000,
    0b00000,
    0b00000,
    0b10001,
    0b01010,
    0b00100,
    0b01010,
    0b10001,
    // y
    0b00000,
    0b00000,
    0b10001,
    0b10001,
    0b01010,
    0b00100,
    0b01000,
    0b10000,
    // z (36 + 26)
    0b00000,
    0b00000,
    0b11111,
    0b00010,
    0b00100,
    0b01000,
    0b10000,
    0b11111,
  };

  int char_w = 5;
  int char_h = 8;

  if (index < 0) {
    return std::floor(GLYPHS.size() / 8);
  }
  else if (index >= GLYPHS.size()) {
    return -1;
  }

  for (int j = 0; j < char_h; j++) {
    for (int i = 0; i < char_w; i++) {
      if (GLYPHS[8 * index + j] & (1 << char_w - i - 1)) {
        set_pixel(i + x, j + y, shader(i + x, j + y));
      }
    }
  }

  return 0;
}

void draw_char(char c, int x, int y, shader_func_t shader) {
  size_t char_index = INT_MAX;

  if (c >= '0' && c <= '9') {
    char_index = c - '0';
  }
  else if (c >= 'A' && c <= 'Z') {
    char_index = c - 'A' + 10;
  }
  else if (c >= 'a' && c <= 'z') {
    char_index = c - 'a' + 36;
  }

  auto ret = draw_glyph(char_index, x, y, shader);

  if (ret == -1) {
    Serial.printf("Unsupported char: '%c'!\n", c);
  }
}

void draw_digit(int d, int x, int y, shader_func_t shader) {
  draw_char('0' + d, x, y, shader);
}

void draw_text(const char *str, int x, int y, shader_func_t shader) {
  const size_t len = std::strlen(str);

  for (int i = 0; i < len; i++) {
    draw_char(str[i], i * 6 + x, y, shader);
  }
}

void draw_text_scrolling(const char *str, int x, int y, int w, shader_func_t shader) {
  const size_t len = std::strlen(str);

  for (int i = 0; i < len; i++) {
    draw_char(str[i], i * 6 + x, y, shader);
  }
}

void draw_glyph_test(float delta) {
  static const int glyphs = draw_glyph(-1, 0, 0, nullptr);
  static float offset = -LED_MATRIX_WIDTH;

  if (offset > (6 * glyphs)) {
    offset = -LED_MATRIX_WIDTH;
  }


  for (int i = 0; i < glyphs; i++) {
    draw_glyph(i, i * 6 - floor(offset), 0, [](int x, int y) { return color(255, 255, 255); });
  }

  offset += delta * 16;
}



void blit_img(int target_x, int target_y, int w, int h, const unsigned char img[]) {
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      set_pixel(target_x + x, target_y + y, color(img[(x + (y * w)) * 3], img[((x + (y * w)) * 3) + 1], img[((x + (y * w)) * 3) + 2]));
    }
  }
}