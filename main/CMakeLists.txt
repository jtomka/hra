idf_component_register(SRCS "blinker.c" "button.c" "event.c" "ic74hc595.c" "main.c" "matrix.c" "object.c" "pwm.c" "ticker.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_system
                    REQUIRES driver
                    REQUIRES freertos)
