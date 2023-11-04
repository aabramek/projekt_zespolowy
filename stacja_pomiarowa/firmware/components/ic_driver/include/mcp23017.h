#ifndef MCP23017_H
#define MCP23017_H

#include <inttypes.h>

#include "esp_err.h"
#include "driver/i2c.h"

#define IODIRA 0
#define IODIRB 1
#define IPOLA 2
#define IPOLB 3
#define GPINTENA 4
#define GPINTENB 5
#define DEFVALA 6
#define DEFVALB 7
#define INTCONA 8
#define INTCONB 9
#define IOCON 10
#define GPPUA 11
#define GPPUB 12
#define INTFA 13
#define INTFB 14
#define INTCAPA 15
#define INTCAPB 16
#define GPIOA 17
#define GPIOB 18
#define OLATA 19
#define OLATB 20

extern uint8_t MCP23017_REGISTER_MAP_BANK[21];
extern uint8_t MCP23017_REGISTER_MAP_NON_BANK[21];

typedef struct
{
	uint8_t address;
	i2c_port_t i2c_port;
}
MCP23017_t;

esp_err_t MCP23017_Write(MCP23017_t *device, uint8_t register_address, uint8_t value);

#endif