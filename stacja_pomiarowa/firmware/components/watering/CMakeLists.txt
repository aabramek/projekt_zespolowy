idf_component_register(SRCS "watering.c"
                    PRIV_REQUIRES "json"
                    PRIV_REQUIRES "freertos"
                    PRIV_REQUIRES "ic_driver"
                    INCLUDE_DIRS "include")
