idf_component_register(SRCS "station.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "mqtt"
                    REQUIRES "freertos"
                    PRIV_REQUIRES "esp_wifi"
                    PRIV_REQUIRES "esp_netif")
