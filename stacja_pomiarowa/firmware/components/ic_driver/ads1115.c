#include "ads1115.h"

#include "freertos/FreeRTOS.h"

#include "driver/i2c.h"

esp_err_t ADS1115_WriteRegister(ADS1115_t *device, uint8_t register_address,
	uint16_t value)
{
	uint8_t buf[] = { register_address, (uint8_t)(value >> 8), (uint8_t)value };
	
	esp_err_t retval = i2c_master_write_to_device(device->i2c_port,
		device->address, buf, sizeof(buf), pdMS_TO_TICKS(15));
	
	return retval;
}

esp_err_t ADS1115_ReadRegister(ADS1115_t *device, uint8_t register_address,
	uint16_t *value)
{
	esp_err_t retval = i2c_master_write_to_device(device->i2c_port,
		device->address, &register_address, sizeof(register_address),
		pdMS_TO_TICKS(15));
	
	if (retval != ESP_OK)
	{
		goto end;
	}

	uint8_t buf[2];
	retval = i2c_master_read_from_device(device->i2c_port, device->address,
		buf, sizeof(buf), pdMS_TO_TICKS(15));
	
	if (retval != ESP_OK)
	{
		goto end;
	}

	*value = ((uint16_t)buf[0]) << 8 | buf[1];

end:
	return retval;
}

void ADS1115_AddressMux(ADS1115_t *device, uint16_t mux_address)
{
	device->configuration = (device->configuration & MUX_MASK) | mux_address;
}

esp_err_t ADS1115_StartConversion(ADS1115_t *device)
{
	return ADS1115_WriteRegister(device, CONFIG_REG, device->configuration
		| (1 << OS_BIT));
}

void ADS1115_SetPGA(ADS1115_t *device, uint16_t pga)
{
	device->configuration = (device->configuration & PGA_MASK) | pga;
}

void ADS1115_ComparatorEnable(ADS1115_t *device)
{
	device->configuration &= COMP_QUEUE_MASK;
}

void ADS1115_SetDataRate(ADS1115_t *device, uint16_t data_rate)
{
	device->configuration = (device->configuration & DATA_RATE_MASK)
		| data_rate;
}