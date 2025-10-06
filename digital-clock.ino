#include <stdint.h>
#include <time.h>
#include <array>
#include <cmath>

#include <esp_netif.h>
#include <esp_netif_sntp.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

#include "res/imgs.hpp"



static const int LED_PIN = 18;
static const int LED_MATRIX_WIDTH = 32;
static const int LED_MATRIX_HEIGHT = 8;
static const int LED_COUNT = (LED_MATRIX_WIDTH * LED_MATRIX_HEIGHT);

static const uint8_t BRIGHTNESS_MAX = 7;
static const uint8_t BRIGHTNESS_AUTO = BRIGHTNESS_MAX;
static const uint8_t BRIGHTNESS_USER_MAX = 6;
static const uint8_t BRIGHTNESS_BASE = BRIGHTNESS_AUTO;

static const gpio_num_t BUTTON_MAIN_PIN = GPIO_NUM_0;
static const gpio_num_t BUTTON_L_PIN = GPIO_NUM_21;
static const gpio_num_t BUTTON_R_PIN = GPIO_NUM_22;

static const gpio_num_t PHOTORESISTOR_PIN = GPIO_NUM_1;

static const char* WIFI_SSID = "CHANGE";
static const char* WIFI_PASS = "ME";

static const char* TIMEZONE = "CET-1CEST,M3.5.0,M10.5.0/3";



enum class ClockStyle : uint32_t {
  lgbt_digits,
  lgbt_background_white_digits,
  white,
  red,
  yellow,
  green,
  turquoise,
  blue,
  purple,
  style_max,
};


static struct State {
  ClockStyle clock_style = ClockStyle::lgbt_digits;
  bool syncing_time = true;
  uint8_t brightness = BRIGHTNESS_BASE;   // = 1 << brightness, 0 = auto
} state;



Adafruit_NeoPixel& matrix() {
  static Adafruit_NeoPixel m(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
  return m;
}



int xy_to_pixel_index(int x, int y) {
  if (x % 2) {
    return (x * LED_MATRIX_HEIGHT) + LED_MATRIX_HEIGHT - 1 - y;
  } else {
    return (x * LED_MATRIX_HEIGHT) + y;
  }
}



typedef uint32_t (*shader_func)(int x, int y);


uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
  return matrix().Color(r, g, b);
}

uint32_t color_hsv(uint16_t h, uint8_t s = 255, uint8_t v = 255) {
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

void fill_rect_shaded(int x, int y, int w, int h, uint32_t (*shader)(int x, int y)) {
  for (int ix = x; ix < x + w; ix++) {
    for (int iy = y; iy < y + h; iy++) {
      set_pixel(ix, iy, shader(ix, iy));
    }
  }
}



// On success 0, if index = -1 then returns number of glyphs, on error -1
int draw_glyph(int index, int x, int y, uint32_t (*shader)(int x, int y)) {
  static const std::array<uint8_t, 8 * (10 + 26)> GLYPHS = {
    // 0
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
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
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
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
    0b10011,
    0b01111,
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
    // Z (10 + 26)
    0b11111,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b10000,
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

void draw_char(char c, int x, int y, uint32_t (*shader)(int x, int y)) {
  size_t char_index = INT_MAX;

  if (c >= '0' && c <= '9') {
    char_index = c - '0';
  }
  else if (c >= 'A' && c <= 'Z') {
    char_index = c - 'A' + 10;
  }

  auto ret = draw_glyph(char_index, x, y, shader);

  if (ret == -1) {
    Serial.printf("Unsupported char: '%c'!\n", c);
  }
}

void draw_digit(int d, int x, int y, uint32_t (*shader)(int x, int y)) {
  draw_char('0' + d, x, y, shader);
}



void blit_img(int target_x, int target_y, int w, int h, const unsigned char img[]) {
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      set_pixel(target_x + x, target_y + y, color(img[(x + (y * w)) * 3], img[((x + (y * w)) * 3) + 1], img[((x + (y * w)) * 3) + 2]));
    }
  }
}


void draw_clock(float delta, ClockStyle style) {
  time_t t = time(nullptr);
  tm *time_ptr = localtime(&t);

  int hour = time_ptr->tm_hour;
  int min = time_ptr->tm_min;
  bool tick = time_ptr->tm_sec % 2;

  static time_t hue;
  hue = t;
  shader_func shader_hue_shift = [](int x, int y) -> uint32_t {
    return color_hsv((hue + (x << 7)) % UINT16_MAX);
  };

  shader_func shader_hue_shift_dim = [](int x, int y) -> uint32_t {
    return color_hsv((hue + (x << 7)) % UINT16_MAX, 255, 127);
  };

  shader_func shader_white = [](int x, int y) -> uint32_t {
    return color(255, 255, 255);
  };

  shader_func shader_red = [](int x, int y) -> uint32_t {
    return color(255, 0, 0);
  };

  shader_func shader_yellow = [](int x, int y) -> uint32_t {
    return color(255, 255, 0);
  };

  shader_func shader_green = [](int x, int y) -> uint32_t {
    return color(0, 255, 0);
  };

  shader_func shader_turquoise = [](int x, int y) -> uint32_t {
    return color(0, 255, 255);
  };

  shader_func shader_blue = [](int x, int y) -> uint32_t {
    return color(0, 0, 255);
  };

  shader_func shader_purple = [](int x, int y) -> uint32_t {
    return color(255, 0, 255);
  };

  shader_func shader;

  switch (style) {
    case ClockStyle::lgbt_digits:
      shader = shader_hue_shift;
      break;
    case ClockStyle::lgbt_background_white_digits:
      shader = shader_white;
      break;
    case ClockStyle::white:
      shader = shader_white;
      break;
    case ClockStyle::red:
      shader = shader_red;
      break;
    case ClockStyle::yellow:
      shader = shader_yellow;
      break;
    case ClockStyle::green:
      shader = shader_green;
      break;
    case ClockStyle::turquoise:
      shader = shader_turquoise;
      break;
    case ClockStyle::blue:
      shader = shader_blue;
      break;
    case ClockStyle::purple:
      shader = shader_purple;
      break;
    default:
      shader = shader_white;
      break;
  }

  if (style == ClockStyle::lgbt_background_white_digits) {
    fill_rect_shaded(0, 0, LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT, shader_hue_shift_dim);
  }

  if (tick) {
    fill_rect_shaded(15, 1, 2, 2, shader);
    fill_rect_shaded(15, 5, 2, 2, shader);
  }

  draw_digit(hour / 10, 2, 0, shader);
  draw_digit(hour % 10, 8, 0, shader);
  draw_digit(min / 10, 19, 0, shader);
  draw_digit(min % 10, 25, 0, shader);

  // if (tick) {
  //   blit_img(0, 0, sun1_width, sun1_height, sun1_img);
  // } else {
  //   blit_img(0, 0, sun2_width, sun2_height, sun2_img);
  // }
}



void draw_char_test(float delta) {
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



void update_brightness(State &state) {
  if (state.brightness != BRIGHTNESS_AUTO) {
    matrix().setBrightness(1 << state.brightness);
    return;
  }
  // Auto brightness

  uint16_t high_treshhold = 3000;
  uint16_t low_treshhold = 200;

  // gpio_set_direction(PHOTORESISTOR_PIN, GPIO_MODE_INPUT);
  pinMode(PHOTORESISTOR_PIN, INPUT);
  uint16_t value = std::min(std::max(analogRead(PHOTORESISTOR_PIN), low_treshhold), high_treshhold);

  uint8_t new_brightness = 1 << (int)(((value - low_treshhold) / (float)high_treshhold) * BRIGHTNESS_USER_MAX);
  matrix().setBrightness(new_brightness);
}



void draw(float delta, State &state) {
  matrix().clear();
  draw_clock(delta, state.clock_style);
//   draw_char_test(delta);

  if (state.syncing_time) {
    set_pixel(31, 7, color(0, 255, 0));
  }

  matrix().show();
}



// every now and then this will try to update time.
void periodic_sync_time(bool &allow_sleep, State &state) {
  static unsigned long last_time_update = millis();
  static bool waiting_for_ntp = false;
  const unsigned long time_sync_interval = 1000 * 60 * 60 * 24; // 24h

  if (!state.syncing_time && last_time_update + time_sync_interval < millis()) {
    Serial.println("Will try to sync time...");
    state.syncing_time = true;
  }

  if (state.syncing_time) {
    allow_sleep = false;
  }

  if (!state.syncing_time) {
    return;
  }

  if (WiFi.status() == WL_STOPPED) {
    Serial.println("Trying to connect to WIFI...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }

  if (WiFi.status() == WL_CONNECTED && !waiting_for_ntp) {
    Serial.println("Syncing time...");
    const esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
    waiting_for_ntp = true;
    // esp_netif_sntp_sync_wait(portMAX_DELAY);
  }

  if (waiting_for_ntp) {
    auto err = esp_netif_sntp_sync_wait(0);
    if (err == ESP_OK) {
      Serial.println("Time synced!");
      waiting_for_ntp = false;
      state.syncing_time = false;
      last_time_update = millis();

      esp_netif_sntp_deinit();
      WiFi.disconnect(true);
      WiFi.mode(WIFI_MODE_NULL);
    }
  }
}



void button_main_itr(void *data) {
}

void button_l_itr(void *data) {
  static int last_debounce = 0;
  static int debounce_limit = 200;
  if (millis() <= last_debounce + debounce_limit) {
    return;
  }

  State* state = (State*)data;

  last_debounce = millis();
  (*(uint32_t*)&state->clock_style)++;
  if (state->clock_style >= ClockStyle::style_max) {
    state->clock_style = (ClockStyle)0;
  }
}

void button_r_itr(void *data) {
  static int last_debounce = 0;
  static int debounce_limit = 200;
  if (millis() <= last_debounce + debounce_limit) {
    return;
  }
  last_debounce = millis();

  State* state = (State*)data;

  if (state->brightness == 0) {
    state->brightness = BRIGHTNESS_MAX;
  } else {
    state->brightness--;
  }
}


void setup() {
  Serial.begin(115200);
  delay(500);

  if (setenv("TZ", TIMEZONE, 1) != 0) {
    Serial.println("Failed to set timzezone");
  }

  if (matrix().begin() != true) {
    Serial.println("Led matrix begin() failed");
  }
  // matrix().setBrightness(1 << BASE_BRIGHTNESS);

  // LED config, low aka ON when not sleeping.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // GPIO inputs
  gpio_set_direction(BUTTON_MAIN_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(BUTTON_L_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(BUTTON_R_PIN, GPIO_MODE_INPUT);


  // Interrupts
  if (auto res = gpio_set_intr_type(BUTTON_MAIN_PIN, GPIO_INTR_POSEDGE); res != ESP_OK) {
    Serial.printf("gpio_set_intr_type() for main button failed: %d\n", res);
  }
  if (auto res = gpio_set_intr_type(BUTTON_L_PIN, GPIO_INTR_POSEDGE); res != ESP_OK) {
    Serial.printf("gpio_set_intr_type() for left button failed: %d\n", res);
  }
  if (auto res = gpio_set_intr_type(BUTTON_R_PIN, GPIO_INTR_POSEDGE); res != ESP_OK) {
    Serial.printf("gpio_set_intr_type() for right button failed: %d\n", res);
  }

  if (auto res = gpio_intr_enable(BUTTON_MAIN_PIN); res != ESP_OK) {
    Serial.printf("Failed to enable interrupts for main button: %d\n", res);
  }
  if (auto res = gpio_intr_enable(BUTTON_L_PIN); res != ESP_OK) {
    Serial.printf("Failed to enable interrupts for left button: %d\n", res);
  }
  if (auto res = gpio_intr_enable(BUTTON_R_PIN); res != ESP_OK) {
    Serial.printf("Failed to enable interrupts for right button: %d\n", res);
  }

  if (auto res = gpio_install_isr_service(ESP_INTR_FLAG_LOWMED); res != ESP_OK) {
    Serial.printf("gpio_install_isr_service() failed: %d\n", res);
  }

  // Interrupt handlers
  if (auto res = gpio_isr_handler_add(BUTTON_MAIN_PIN, button_main_itr, &state); res != ESP_OK) {
    Serial.printf("gpio_isr_handler_add() for main button failed:%d\n", res);
  }
  if (auto res = gpio_isr_handler_add(BUTTON_L_PIN, button_l_itr, &state); res != ESP_OK) {
    Serial.printf("gpio_isr_handler_add() for left button failed:%d\n", res);
  }
  if (auto res = gpio_isr_handler_add(BUTTON_R_PIN, button_r_itr, &state); res != ESP_OK) {
    Serial.printf("gpio_isr_handler_add() for right button failed:%d\n", res);
  }

  // Sleep config
  // Main button wakeup
  if (auto res = esp_sleep_enable_ext1_wakeup_io(1 << BUTTON_MAIN_PIN, ESP_EXT1_WAKEUP_ANY_HIGH); res != ESP_OK) {
    Serial.printf("esp_sleep_enable_ext1_wakeup_io() failed: %d\n", res);
  }

  // This must be after interrupts were setup, otherwise they do not work.
  // Other buttons
  if (auto res = gpio_wakeup_enable(BUTTON_L_PIN, GPIO_INTR_HIGH_LEVEL); res != ESP_OK) {
    Serial.printf("gpio_wakeup_enable() for left button failed: %d\n", res);
  }
  if (auto res = gpio_wakeup_enable(BUTTON_R_PIN, GPIO_INTR_HIGH_LEVEL); res != ESP_OK) {
    Serial.printf("gpio_wakeup_enable() for right button failed: %d\n", res);
  }
  if (auto res = esp_sleep_enable_gpio_wakeup(); res != ESP_OK) {
    Serial.printf("esp_sleep_enable_gpio_wakeup() failed: %d\n", res);
  }

  // Timer, 1s
  if (auto res = esp_sleep_enable_timer_wakeup(1000000); res != ESP_OK) {
    Serial.printf("esp_sleep_enable_timer_wakeup() failed: %d\n", res);
  }
}



void loop() {
  static float delta = 0;
  {
    // Delta time
    static auto last_time = micros();

    auto cur_time = micros();
    auto delta_time = cur_time - last_time;
    last_time = cur_time;
    delta = delta_time / 1000000.0;
  }

  bool allow_sleep = true;

  if (Serial.isConnected()) {
    allow_sleep = false;
  }

  periodic_sync_time(allow_sleep, state);
  update_brightness(state);
  draw(delta, state);
  // digitalWrite(LED_BUILTIN, LOW);

  if (allow_sleep) {
    esp_light_sleep_start();
    auto cause = esp_sleep_get_wakeup_cause();
    // switch (cause) {
    //   case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    //   case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    //   case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    //   case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    //   case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    //   default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", cause); break;
    // }
  } else {
    delay(50);
  }
}
