idf_component_register(SRCS "button.c" "event.c" "ic74hc595.c" "main.c" "matrix.c" "metronome.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    REQUIRES freertos)
