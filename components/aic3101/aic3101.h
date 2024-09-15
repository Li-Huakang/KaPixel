#ifndef AIC3101_H
#define AIC3101_H

#include <stdint.h>
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AIC3101_I2C_ADDR 0x18
#define I2C_MASTER_FREQ_HZ 100000

/**
 * @brief Codec I2C configuration
 */
typedef struct {
    uint8_t port;       /*!< I2C port, this port needs to be pre-installed by other modules */ /*not used*/
    uint8_t addr;       /*!< I2C address, default address can be found in codec header files */
    i2c_master_bus_handle_t *i2c_bus_handle; /*!< I2C Master bus handle (for IDF v5.3 or higher version) */
    i2c_master_dev_handle_t *i2c_device_handle; /*!< I2C device handle (for IDF v5.3 or higher version) */
    uint8_t sda_pin;    /*!< I2C SDA pin number */
    uint8_t scl_pin;    /*!< I2C SCL pin number */
    uint32_t clk_speed; /*!< I2C clock speed */
} audio_codec_i2c_cfg_t;

/**
 * @brief Codec I2S configuration
 */
typedef struct {
    uint8_t port;      /*!< I2S port, this port needs to be pre-installed by other modules */ /*not used*/
    void   *rx_handle; /*!< I2S RX handle, must be provided on IDF 5.x */
    void   *tx_handle; /*!< I2S TX handle, must be provided on IDF 5.x */
    uint8_t mclk_pin;  /*!< I2S MCLK pin number */
    uint8_t bclk_pin;  /*!< I2S BCLK pin number */
    uint8_t lrclk_pin; /*!< I2S LRCLK pin number */
    uint8_t din_pin;   /*!< I2S DIN pin number */
    uint8_t dout_pin;  /*!< I2S DOUT pin number */
} audio_codec_i2s_cfg_t;

/**
 * @brief Customized codec configuration
 */
typedef struct {
    const audio_codec_i2c_cfg_t *i2c_cfg; /*!< Codec I2C configuration */
    const audio_codec_i2s_cfg_t *i2s_cfg; /*!< Codec I2S configuration */
    uint8_t codec_reset_pin;              /*!< Codec reset pin number */
    uint8_t pa_shutdown_pin;              /*!< PA shutdown pin number */
} audio_codec_cfg_t;

/**
 * @brief Initialize the codec
 *
 * @param[in] codec Pointer to the codec configuration
 * @return 0 on success, negative error code on failure
 */
esp_err_t audio_codec_init(audio_codec_cfg_t *codec_config);

/**
 * @brief Reset the codec to default settings
 *
 * @param[in] codec Pointer to the codec configuration
 * @return 0 on success, negative error code on failure
 */
int audio_codec_reset(audio_codec_cfg_t *codec);

/**
 * @brief Read a register from the codec
 *
 * @param[in] codec Pointer to the codec configuration
 * @param[in] reg Register address to read from
 * @param[out] value Pointer to store the read value
 * @return 0 on success, negative error code on failure
 */
esp_err_t audio_codec_read(const audio_codec_cfg_t *codec_config, const uint8_t *data);

/**
 * @brief Write a value to a register of the codec
 *
 * @param[in] codec Pointer to the codec configuration
 * @param[in] reg Register address to write to
 * @param[in] value Value to write to the register
 * @return 0 on success, negative error code on failure
 */
esp_err_t audio_codec_write(const audio_codec_cfg_t *codec_config, const uint8_t *data);

esp_err_t enable_pa(const audio_codec_cfg_t *codec_config);

void set_line_to_pa_mode(const audio_codec_cfg_t *codec_config);

void codec_sw_reset(const audio_codec_cfg_t *codec_config);

#ifdef __cplusplus
}
#endif

#endif /* AIC3101_H */
