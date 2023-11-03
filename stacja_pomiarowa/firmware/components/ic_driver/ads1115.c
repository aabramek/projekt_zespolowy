#include "ads1115.h"

#include "freertos/FreeRTOS.h"

#include "driver/i2c.h"

#define RETVAL_CHECK_DELETE_CMD if (retval != ESP_OK) goto delete_cmd_link;
#define RETVAL_CHECK_END if (retval != ESP_OK) goto end;

esp_err_t ADS1115_Init(ADS1115_t *ads1115)
{
	esp_err_t retval = ESP_OK;
	retval = ADS1115_WriteRegister(ads1115, CONFIG_REG, ads1115->configuration); RETVAL_CHECK_END;
	retval = ADS1115_WriteRegister(ads1115, LO_THRESH_REG, 0); RETVAL_CHECK_END;
	retval = ADS1115_WriteRegister(ads1115, HI_THRESH_REG, 0xFFFF); RETVAL_CHECK_END;
end:
	return retval;
}

esp_err_t ADS1115_WriteRegister(ADS1115_t *ads1115, uint8_t register_address, uint16_t value)
{
	static const char *TAG = "ADS1115_WriteRegister";

	esp_err_t retval = ESP_OK;
	i2c_cmd_handle_t i2c_cmd = i2c_cmd_link_create(); RETVAL_CHECK_END;

	retval = i2c_master_start(i2c_cmd); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_write_byte(i2c_cmd, (ads1115->address << 1) | I2C_MASTER_WRITE, true); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_write_byte(i2c_cmd, register_address, true); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_write_byte(i2c_cmd, (uint8_t)(value >> 8), true); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_write_byte(i2c_cmd, (uint8_t)value, true); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_stop(i2c_cmd); RETVAL_CHECK_DELETE_CMD;

	TickType_t timeout = pdMS_TO_TICKS(15);
	do
	{
		retval = i2c_master_cmd_begin(I2C_NUM_0, i2c_cmd, timeout);
		if (retval == ESP_ERR_TIMEOUT)
		{
			ESP_LOGW(TAG, "I2C busy...");
		}
	}
	while (retval == ESP_ERR_TIMEOUT);

delete_cmd_link:
	i2c_cmd_link_delete(i2c_cmd);
end:
	return retval;
}

esp_err_t ADS1115_ReadRegister(ADS1115_t *ads1115, int8_t register_address, uint16_t *value)
{
	static const char *TAG = "ADS1115_ReadRegister";
	esp_err_t retval = ESP_OK;

	i2c_cmd_handle_t i2c_cmd = i2c_cmd_link_create(); RETVAL_CHECK_END;

	retval = i2c_master_start(i2c_cmd); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_write_byte(i2c_cmd, (ads1115->address << 1) | I2C_MASTER_WRITE, true); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_write_byte(i2c_cmd, register_address, true); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_stop(i2c_cmd); RETVAL_CHECK_DELETE_CMD;

	TickType_t timeout = pdMS_TO_TICKS(15);
	do
	{
		retval = i2c_master_cmd_begin(I2C_NUM_0, i2c_cmd, timeout);
		if (retval == ESP_ERR_TIMEOUT)
		{
			ESP_LOGW(TAG, "I2C busy...");
		}
	}
	while (retval == ESP_ERR_TIMEOUT);

	i2c_cmd_link_delete(i2c_cmd);

	i2c_cmd = i2c_cmd_link_create(); RETVAL_CHECK_END;
	retval = i2c_master_start(i2c_cmd); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_write_byte(i2c_cmd, (ads1115->address << 1) | I2C_MASTER_READ, true); RETVAL_CHECK_DELETE_CMD;
	uint8_t msb, lsb;
	retval = i2c_master_read_byte(i2c_cmd, &msb, I2C_MASTER_ACK); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_read_byte(i2c_cmd, &lsb, I2C_MASTER_ACK); RETVAL_CHECK_DELETE_CMD;
	retval = i2c_master_stop(i2c_cmd); RETVAL_CHECK_DELETE_CMD;

	do
	{
		retval = i2c_master_cmd_begin(I2C_NUM_0, i2c_cmd, timeout);
		if (retval == ESP_ERR_TIMEOUT)
		{
			ESP_LOGW(TAG, "I2C busy...");
		}
	}
	while (retval == ESP_ERR_TIMEOUT);

	if (retval == ESP_OK)
	{
		*value = ((uint16_t)msb) << 8 | lsb;
	}

delete_cmd_link:
	i2c_cmd_link_delete(i2c_cmd);
end:
	return retval;	
}

esp_err_t ADS1115_AddressMux(ADS1115_t *ads1115, uint8_t addr)
{
	ads1115->configuration = (ads1115->configuration & MUX_MASK) | (((uint16_t)addr) << MUX_BITS);
	return ADS1115_WriteRegister(ads1115, CONFIG_REG, ads1115->configuration);
}

esp_err_t ADS1115_StartConversion(ADS1115_t *ads1115)
{
	return ADS1115_WriteRegister(ads1115, CONFIG_REG, ads1115->configuration | (1 << OS_BIT));
}

esp_err_t ADS1115_SetPGA(ADS1115_t *ads1115, uint8_t pga)
{
	ads1115->configuration = (ads1115->configuration & PGA_MASK) | (((uint16_t)pga) << PGA_BITS);
	return ADS1115_WriteRegister(ads1115, CONFIG_REG, ads1115->configuration);
}

esp_err_t ADS1115_ComparatorEnable(ADS1115_t *ads1115)
{
	ads1115->configuration = ads1115->configuration & COMP_QUEUE_MASK;
	return ADS1115_WriteRegister(ads1115, CONFIG_REG, ads1115->configuration);
}

esp_err_t ADS1115_SetDataRate(ADS1115_t *ads1115, uint8_t data_rate)
{
	ads1115->configuration = (ads1115->configuration & DATA_RATE_MASK) | (((uint16_t)data_rate) << DATA_RATE_BITS);
	return ADS1115_WriteRegister(ads1115, CONFIG_REG, ads1115->configuration);
}