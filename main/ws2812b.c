#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"
#include "esp_err.h"
#include "ws2812b.h"

static const char *TAG = "WS2812B";

static led_strip_handle_t led_strip_handle;

esp_err_t led_init()
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_BLINK_GPIO,   // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_NUMBERS,        // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };
    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
#else
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
        .flags.with_dma = true,               // DMA feature is available on ESP target like ESP32-S3
#endif
    };

    // LED Strip object handle
    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WS2812B initialize failed: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");

    ESP_ERROR_CHECK(led_strip_clear(led_strip_handle)); // Clear all the LEDs
    return ESP_OK;
    
}

esp_err_t led_clear_all() {
    return led_strip_clear(led_strip_handle);
}

const uint8_t font_num[10][3] = {
    {0x3E, 0x44, 0x3E}, // 0
    {0x12, 0x7C, 0x02}, // 1
    {0x26, 0x54, 0x32}, // 2
    {0x2A, 0x54, 0x3E}, // 3
    {0x38, 0x10, 0x3E}, // 4
    {0x3A, 0x54, 0x2E}, // 5
    {0x3E, 0x54, 0x2E}, // 6
    {0x20, 0x74, 0x30}, // 7
    {0x3E, 0x54, 0x3E}, // 8
    {0x3A, 0x54, 0x3E}  // 9
};

const uint8_t font_space[1][1] = {
    {0x00}
};

const uint8_t font_colon[1][1] = {
    {0x14}
};

bool led_is_reverse(int start_index) {
    return (start_index/8%2) != 0;
}

void led_set_num_pixel(int num,int *start_index, uint32_t red, uint32_t green, uint32_t blue, uint8_t brightness, bool reverse) {
    int end_index = *start_index;
    for (int i = 0; i < sizeof(font_num[0]) / sizeof(font_num[0][0]) ; i++) {
        for (int j = 0; j < 8; j++) {
            int bit_pos = reverse ? j : (7 - j);  // 根据 reverse 计算当前位位置
            int re = (font_num[num][i] >> bit_pos) & 1;
            if (re == 1) {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip_handle, end_index, red*brightness/100, green*brightness/100, blue*brightness/100));
            }else {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip_handle, end_index, 0, 0, 0));
            }
            end_index +=1;
        }
    }
    ESP_ERROR_CHECK(led_strip_refresh(led_strip_handle));
    *start_index = end_index;
}

void led_set_colon(int *start_index, uint32_t red, uint32_t green, uint32_t blue, uint8_t brightness, bool reverse) {
    int end_index = *start_index;
        for (int j = 0; j < 8; j++) {
            int bit_pos = reverse ? j : (7 - j);  // 根据 reverse 计算当前位位置
            int re = (font_colon[0][j] >> bit_pos) & 1;
            if (re == 1) {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip_handle, end_index, red*brightness/100, green*brightness/100, blue*brightness/100));
            }else {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip_handle, end_index, 0, 0, 0));
            }
            end_index +=1;
        }
    ESP_ERROR_CHECK(led_strip_refresh(led_strip_handle));
    *start_index = end_index;
}

void led_set_space(int *start_index) {
    *start_index += 8;
}

void led_display_time(const char *time) {
    ESP_ERROR_CHECK(led_clear_all());
    int hour1, hour2, minute1, minute2, second1, second2;
    hour1 = time[0] - '0';
    hour2 = time[1] - '0';
    minute1 = time[2] - '0';
    minute2 = time[3] - '0';
    second1 = time[4] - '0';
    second2 = time[5] - '0';
    int index = 0;
    led_set_space(&index);
    led_set_space(&index);
    led_set_num_pixel(hour1, &index, 255, 0, 0, 1, led_is_reverse(index));
    led_set_space(&index);
    led_set_num_pixel(hour2, &index, 255, 0, 0, 1, led_is_reverse(index));
    led_set_space(&index);
    led_set_colon(&index, 255, 0, 0, 1, led_is_reverse(index));
    led_set_space(&index);

    led_set_num_pixel(minute1, &index, 255, 0, 0, 1, led_is_reverse(index));
    led_set_space(&index);
    led_set_num_pixel(minute2, &index, 255, 0, 0, 1, led_is_reverse(index));
    led_set_space(&index);
    led_set_colon(&index, 255, 0, 0, 1, led_is_reverse(index));
    led_set_space(&index);

    led_set_num_pixel(second1, &index, 255, 0, 0, 1, led_is_reverse(index));
    led_set_space(&index);
    led_set_num_pixel(second2, &index, 255, 0, 0, 1, led_is_reverse(index));
    led_set_space(&index);
}

