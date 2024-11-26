idf_component_register(SRCS "blinker.c" "button.c" "event.c" "ic74hc595.c" "main.c" "matrix.c" "object.c" "ticker.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    REQUIRES freertos)
