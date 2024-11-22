idf_component_register(SRCS "main.c" "button.c" "matrix.c" "event.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    REQUIRES freertos)
