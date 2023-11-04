#include "ads1115.h"

#include "freertos/FreeRTOS.h"

#include "driver/i2c.h"

esp_err_t ADS1115_WriteRegister(ADS1115_t *device, uint8_t register_address, uint16_t value)
{
	static const char *TAG = "ADS1115_WriteRegister";
	uint8_t buf[] = { register_address, (uint8_t)(value >> 8), (uint8_t)value };
	esp_err_t retval = i2c_master_write_to_device(device->i2c_port, device->address, buf, sizeof(buf), pdMS_TO_TICKS(15));
	while (retval == ESP_ERR_TIMEOUT)
	{
		ESP_LOGW(TAG, "I2C busy...");
		retval = i2c_master_write_to_device(device->i2c_port, device->address, buf, sizeof(buf), pdMS_TO_TICKS(15));
	}
	return retval;
}

esp_err_t ADS1115_ReadRegister(ADS1115_t *device, uint8_t register_address, uint16_t *value)
{
	static const char *TAG = "ADS1115_ReadRegister";
	esp_err_t retval = i2c_master_write_to_device(device->i2c_port, device->address, &register_address, sizeof(register_address), pdMS_TO_TICKS(15));
	while (retval == ESP_ERR_TIMEOUT)
	{
		ESP_LOGW(TAG, "I2C busy...");
		retval = i2c_master_write_to_device(device->i2c_port, device->address, &register_address, sizeof(register_address), pdMS_TO_TICKS(15));
	}
	if (retval != ESP_OK)
	{
		return retval;
	}

	uint8_t buf[2];
	retval = i2c_master_read_from_device(device->i2c_port, device->address, buf, sizeof(buf), pdMS_TO_TICKS(15));
	while (retval == ESP_ERR_TIMEOUT)
	{
		ESP_LOGW(TAG, "I2C busy...");
		retval = i2c_master_read_from_device(device->i2c_port, device->address, buf, sizeof(buf), pdMS_TO_TICKS(15));
	}
	if (retval == ESP_OK)
	{
		*value = ((uint16_t)buf[0]) << 8 | buf[1];
	}
	return retval;
}

uint16_t ADS1115_AddressMux(uint16_t configuration, uint16_t mux_address)
{
	return ((configuration & MUX_MASK) | mux_address);
}

esp_err_t ADS1115_StartConversion(ADS1115_t *device, uint16_t configuration)
{
	return ADS1115_WriteRegister(device, CONFIG_REG, configuration | (1 << OS_BIT));
}

uint16_t ADS1115_SetPGA(uint16_t configuration, uint16_t pga)
{
	return ((configuration & PGA_MASK) | pga);
}

uint16_t ADS1115_ComparatorEnable(uint16_t configuration)
{
	return (configuration & COMP_QUEUE_MASK);
}

uint16_t ADS1115_SetDataRate(uint16_t configuration, uint16_t data_rate)
{
	return ((configuration & DATA_RATE_MASK) | data_rate);
}