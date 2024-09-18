#ifndef WS2812B_H
#define WS2812B_H

#include "driver/gpio.h"
#include "led_strip.h"
#include "time.h"


// GPIO assignment
#define LED_STRIP_BLINK_GPIO  6

// Numbers of the LED in the strip
#define PIXEL_WIDTH 32
#define PIXEL_HIGHT 8
#define LED_STRIP_LED_NUMBERS (PIXEL_WIDTH*PIXEL_HIGHT)

// 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

#define BRIGHTNESS_SCALE 0.01


esp_err_t led_init();

esp_err_t led_clear_all();

bool led_is_reverse(int start_index);

void led_set_num_pixel(int num,int *start_index, uint32_t red, uint32_t green, uint32_t blue, uint8_t brightness, bool reverse);

void led_set_colon(int *start_index, uint32_t red, uint32_t green, uint32_t blue, uint8_t brightness, bool reverse);

void led_set_space(int *start_index);

void led_display_time(const struct tm *timeinfo);


#endif // WS2812B_H
