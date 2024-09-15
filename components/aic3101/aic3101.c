#include "aic3101.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include <string.h>

static const char TAG[] = "i2c-codec";

esp_err_t audio_codec_init(audio_codec_cfg_t *codec_config) {
    i2c_master_bus_handle_t bus_handle = *(codec_config->i2c_cfg->i2c_bus_handle);
    i2c_master_dev_handle_t  *codec_handle = codec_config->i2c_cfg->i2c_device_handle;
    esp_err_t ret = ESP_OK;
    
    // 配置 I2C 设备
    i2c_device_config_t i2c_dev_conf = {
        .scl_speed_hz = codec_config->i2c_cfg->clk_speed,
        .device_address = codec_config->i2c_cfg->addr,
    };

    // 添加 I2C 设备到总线
    ret = i2c_master_bus_add_device(bus_handle, &i2c_dev_conf, codec_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Failed to add I2C device to bus");

    // 打印成功日志
    ESP_LOGI(TAG, "I2C device handle initialized: %p", *codec_handle);
    return ESP_OK;

err:
    if (*codec_handle) {
        i2c_master_bus_rm_device(*codec_handle);
        *codec_handle = NULL;
    }
    return ret;
}


esp_err_t audio_codec_write(const audio_codec_cfg_t *codec_config, const uint8_t *data)
{
    ESP_RETURN_ON_FALSE(*(codec_config->i2c_cfg->i2c_device_handle), ESP_ERR_NO_MEM, TAG, "no mem for buffer");
    uint32_t size = sizeof(data);
    ESP_LOGI(TAG, "dev_handle %p", *(codec_config->i2c_cfg->i2c_device_handle));//i2c-codec: dev_handle 0x3fc99f10
    ESP_LOGI(TAG, "Writing %d bytes to codec, Address %u, DATA 0x%02X", (int)size, data[0], data[1]);
    return i2c_master_transmit(*(codec_config->i2c_cfg->i2c_device_handle), data, size, -1);
}

esp_err_t audio_codec_read(const audio_codec_cfg_t *codec_config, const uint8_t *data) {
    ESP_RETURN_ON_FALSE(codec_config->i2c_cfg->i2c_device_handle, ESP_ERR_INVALID_ARG, TAG, "invalid device handle");
    uint32_t size = sizeof(&data);
    return 0; //todo
}


esp_err_t enable_pa(const audio_codec_cfg_t *codec_config)
{
    // 配置 GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << codec_config->pa_shutdown_pin),  // 定义要配置的引脚
        .mode = GPIO_MODE_OUTPUT,                   // 设置为输出模式
        .pull_up_en = GPIO_PULLUP_DISABLE,          // 禁用上拉
        .pull_down_en = GPIO_PULLDOWN_DISABLE,      // 禁用下拉
        .intr_type = GPIO_INTR_DISABLE              // 禁用中断
    };
    
    // 检查 gpio_config 的返回值
    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) {
        ESP_LOGE("PA_CTRL", "GPIO configuration failed: %s", esp_err_to_name(err));
        return err;  // 如果 GPIO 配置失败，返回错误
    }

    // 设置引脚电平，并检查返回值
    err = gpio_set_level(codec_config->pa_shutdown_pin, 1);
    if (err != ESP_OK) {
        ESP_LOGE("PA_CTRL", "GPIO set level failed: %s", esp_err_to_name(err));
        return err;  // 如果设置引脚电平失败，返回错误
    }

    ESP_LOGI("PA_CTRL", "PA Enabled");
    return ESP_OK;  // 成功时返回 ESP_OK
}


void codec_sw_reset(const audio_codec_cfg_t *codec_config) {
    // Select Page 0
    uint8_t page_select_buffer[] = {0x00, 0x00}; // Page Select to Page 0
    ESP_ERROR_CHECK(audio_codec_write(codec_config, page_select_buffer));

    // Self-clearing software reset
    uint8_t reset_buffer[] = {1, 0x80};
    ESP_ERROR_CHECK(audio_codec_write(codec_config, reset_buffer));
}


void set_line_to_pa_mode(const audio_codec_cfg_t *codec_config) {
    codec_sw_reset(codec_config);

    // Select Page 0
    uint8_t page_select_buffer[] = {0x00, 0x00}; // Page Select to Page 0
    ESP_ERROR_CHECK(audio_codec_write(codec_config, page_select_buffer));

    // Power up the LINE2L and LINE2R inputs and route them to the left and right ADCs
    uint8_t line2l_buffer[] = {17, 0x0F};
    audio_codec_write(codec_config, line2l_buffer); // Route LINE2L to Left ADC, power up Left ADC
    uint8_t line2r_buffer[] = {18, 0x0F};
    audio_codec_write(codec_config, line2r_buffer); // Route LINE2R to Right ADC, power up Right ADC

    // Unmute and set gain for the left and right PGAs
    uint8_t left_pga_buffer[] = {15, 0x00}; // Unmute Left PGA, set gain to 0 dB
    audio_codec_write(codec_config, left_pga_buffer);
    uint8_t right_pga_buffer[] = {16, 0x00}; // Unmute Right PGA, set gain to 0 dB
    audio_codec_write(codec_config, right_pga_buffer);

    // Route PGA to LOP/M
    uint8_t left_dac_buffer[] = {81, 0x80+50}; // Route PGA_L to LEFT_LOP/M Volume Control Register –30.1dB
    audio_codec_write(codec_config, left_dac_buffer);
    uint8_t right_dac_buffer[] = {91, 0x80+50}; // PGA_R to LEFT_LOP/M Volume Control Register –30.1
    audio_codec_write(codec_config, right_dac_buffer);

    uint8_t left_lop_buffer[] = {86, 0x09}; // Power up Left LOP and LOM, set gain to 0 dB
    audio_codec_write(codec_config, left_lop_buffer);
    uint8_t right_lop_buffer[] = {93, 0x09}; // Power up Right LOP and LOM, set gain to 0 dB
    audio_codec_write(codec_config, right_lop_buffer);
}
