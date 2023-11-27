#include "mux.h"

#include "driver/gpio.h"

esp_err_t mux_init(void)
{
	gpio_config_t gpio_cfg = {
		.pin_bit_mask = (1ULL << GPIO_NUM_32) | (1ULL << GPIO_NUM_33) 
			| (1ULL << GPIO_NUM_5),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = 0
	};

	return gpio_config(&gpio_cfg);
}

esp_err_t mux_address(uint32_t address)
{
	if (address > 7)
		return ESP_ERR_INVALID_ARG;

	gpio_set_level(GPIO_NUM_32, address & 1);
	gpio_set_level(GPIO_NUM_33, address & 2);
	gpio_set_level(GPIO_NUM_5, address & 4);

	return ESP_OK;
}