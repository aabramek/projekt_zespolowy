#ifndef MUX_H
#define MUX_H

#include "esp_check.h"
#include "esp_err.h"

esp_err_t mux_init(void);
esp_err_t mux_address(uint32_t address);

#endif