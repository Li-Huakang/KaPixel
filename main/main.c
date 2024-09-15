#include <stdio.h>
#include "main.h"
#include "aic3101.h"
#include "ws2812b.h"
#include "wifi.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"


void app_main(void) {
    wifi_prov();
    esp_err_t ret;
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = PORT_NUMBER,
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t codec_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

    // 配置 Codec I2C
    audio_codec_i2c_cfg_t codec_i2c_cfg = {
        .addr = AIC3101_I2C_ADDR,
        .i2c_bus_handle = &bus_handle,
        .i2c_device_handle = &codec_handle,
        .clk_speed = I2C_MASTER_FREQ_HZ,
    };

    // 配置 Codec I2S
    audio_codec_i2s_cfg_t codec_i2s_cfg = {
        .mclk_pin = I2S_MCLK_PIN,
        .bclk_pin = I2S_BCLK_PIN,
        .lrclk_pin = I2S_LRCLK_PIN,
        .din_pin = I2S_DIN_PIN,
        .dout_pin = I2S_DOUT_PIN,
    };

    // 完整的 Codec 配置
    audio_codec_cfg_t codec_cfg = {
        .i2c_cfg = &codec_i2c_cfg,
        .i2s_cfg = &codec_i2s_cfg,
        .codec_reset_pin = CODEC_RESET_PIN,
        .pa_shutdown_pin = PA_SHUTDOWN_PIN,
    };

    // 初始化 Codec
    ret = audio_codec_init(&codec_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE("main", "Failed to initialize audio codec: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI("main", "Audio codec initialized successfully!");

    //Set CODEC to passthrough mode
    set_line_to_pa_mode(&codec_cfg);
    // enable_pa(&codec_cfg);

    if (led_init() != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize LED strip");
        return;
    }

    // while (true) {
    //     int a = 0;
    //     int *index = &a;
    //     for (int i=0; i<10; i++) {
    //         ESP_LOGI("main", "index: %d", *index);
    //         ESP_LOGI("main", "Display: %d", i);
    //         led_set_num_pixel(i, index, 200, 220, 230, 1, led_is_reverse(*index));
    //         led_set_space(index);
    //         led_set_colon(index, 200, 220, 230, 1, led_is_reverse(*index));
    //         led_set_space(index);
    //         vTaskDelay(pdMS_TO_TICKS(1000));
    //         if ((*index+8*3) >= LED_STRIP_LED_NUMBERS){
    //             led_clear_all();
    //             *index = 0;
    //         }
    //     }
    // }

    led_display_time("123456");
}
