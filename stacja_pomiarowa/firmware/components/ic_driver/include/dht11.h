#ifndef DHT11_H
#define	DHT11_H

#include "esp_check.h"
#include "driver/rmt_rx.h"
#include "driver/gpio.h"

typedef struct 
{
	rmt_channel_handle_t rx_channel;
	gpio_num_t init_signal_gpio_num;
}
DHT11_t;

typedef struct
{
	uint8_t rh_integral;
	uint8_t rh_decimal;
	uint8_t temp_integral;
	uint8_t temp_decimal;
	uint8_t checksum;
}
DHT11_Reading_t;

esp_err_t DHT11_Init(DHT11_t *device, gpio_num_t gpio_num,
	gpio_num_t init_signal_gpio_num,
	rmt_rx_done_callback_t rmt_rx_done_callback, void *callback_user_data);

esp_err_t DHT11_Read(DHT11_t *device, rmt_symbol_word_t *buffer,
	size_t buffer_len);

esp_err_t DHT11_Parse(rmt_symbol_word_t *symbols, size_t symbols_num,
	DHT11_Reading_t *result);

#endif
