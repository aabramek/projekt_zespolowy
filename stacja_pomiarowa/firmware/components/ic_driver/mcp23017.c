#include "mcp23017.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"
#include "esp_check.h"

esp_err_t MCP23017_Write(uint8_t device_addr, uint8_t register_addr, uint8_t value)
{
	uint8_t buf[] = {register_addr, value};
	return i2c_master_write_to_device(I2C_NUM_0, device_addr, buf, 2, pdMS_TO_TICKS(100));
}

esp_err_t MCP23017_Init(uint8_t device_addr, uint32_t sda_pin, uint32_t scl_pin)
{
	const char* TAG = "MCP23017_Init";

	esp_err_t status;

	i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = sda_pin,
		.scl_io_num = scl_pin,
		.sda_pullup_en = false,
		.scl_pullup_en = false,
		.master.clk_speed = 50000,
		.clk_flags = 0
	};

	status = i2c_param_config(I2C_NUM_0, &i2c_config);
	if (status != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot configure i2c peripheral");
		goto end;
	}

	status = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	if (status != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot install i2c driver");
		goto end;
	}

	status = MCP23017_Write(device_addr, 0x0A, 0x20);
	if (status != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot send data to ic");
		goto end;
	}

	end:
	return status;
}