#include <stdio.h>
#include "aic3101.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"

#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 18
#define PORT_NUMBER -1

#define I2S_MCLK_PIN 2
#define I2S_BCLK_PIN 17
#define I2S_LRCLK_PIN 45
#define I2S_DIN_PIN 15
#define I2S_DOUT_PIN 16

#define CODEC_RESET_PIN 19
#define PA_SHUTDOWN_PIN 46



