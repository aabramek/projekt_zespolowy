idf_component_register(SRCS "onewire_bus_api.c"
                    SRCS "onewire_bus_impl_rmt.c"
                    SRCS "onewire_crc.c"
                    SRCS "onewire_device.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "driver")
