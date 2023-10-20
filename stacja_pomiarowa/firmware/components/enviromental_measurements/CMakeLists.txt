idf_component_register(SRCS "enviromental_measurements.c"
                    SRCS "sh_measurement.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES "driver"
                    PRIV_REQUIRES "esp_adc"
                    PRIV_REQUIRES "ic_driver")
