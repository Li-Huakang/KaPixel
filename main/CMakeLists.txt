idf_component_register(SRCS "wifi.c" "ws2812b.c" "main.c"
                    INCLUDE_DIRS ""
                    REQUIRES aic3101 esp_wifi nvs_flash wifi_provisioning)