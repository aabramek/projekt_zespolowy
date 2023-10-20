#ifndef MCP23017_H
#define MCP23017_H

#include <inttypes.h>

#include "esp_err.h"

#define IODIRA	0x00
#define IODIRB	0x01
#define GPIOA	0x12
#define GPIOB	0x13

esp_err_t MCP23017_Write(uint8_t device_addr, uint8_t register_addr, uint8_t value);
esp_err_t MCP23017_Init(uint8_t device_addr, uint32_t sda_pin, uint32_t scl_pin);

#endif