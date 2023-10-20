#ifndef SOIL_HUMIDITY_MEASUREMENT
#define SOIL_HUMIDITY_MEASUREMENT

#define INPUT_LINE_OCCUPIED 1
#define INPUT_LINE_FREE 2

#define MUX_ADDR_A GPIO_NUM_32
#define MUX_ADDR_B GPIO_NUM_33
#define MUX_ADDR_C GPIO_NUM_5

#include "esp_check.h"

extern uint32_t _sh_sensors;

esp_err_t sh_measurement_init(void);

uint32_t sh_measurement_add_sensors(uint32_t sensors);

uint32_t sh_measurement_remove_sensors(uint32_t input_line);

uint32_t sh_measurement_check_sensors(uint32_t input_line);

esp_err_t sh_measure(uint32_t *results);

#endif