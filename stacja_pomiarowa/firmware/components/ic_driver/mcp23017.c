#include "mcp23017.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"
#include "esp_check.h"

uint8_t MCP23017_REGISTER_MAP_BANK[21] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15
};

uint8_t MCP23017_REGISTER_MAP_NON_BANK[21] = {
	0x0, 0x10, 0x1, 0x11, 0x2, 0x12, 0x3, 0x13, 0x4, 0x14, 0x5, 0x6, 0x16, 0x7, 0x17, 0x8, 0x18, 0x9, 0x19, 0xa, 0x1a
};

esp_err_t MCP23017_Write(MCP23017_t *device, uint8_t register_address, uint8_t value)
{
	const char* TAG = "MCP23017_Write";
	uint8_t buf[] = {register_address, value};
	esp_err_t retval = ESP_ERR_TIMEOUT;
	retval = i2c_master_write_to_device(device->i2c_port, device->address, buf, 2, pdMS_TO_TICKS(15));
	while (retval == ESP_ERR_TIMEOUT) {
		ESP_LOGW(TAG, "I2C busy...");
		retval = i2c_master_write_to_device(device->i2c_port, device->address, buf, 2, pdMS_TO_TICKS(15));
	}
	return retval;
}
