#include "dht11.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

esp_err_t DHT11_Init(DHT11_t *device,  gpio_num_t gpio_num, gpio_num_t init_signal_gpio_num, rmt_rx_done_callback_t rmt_rx_done_callback void *callback_user_data)
{
	esp_err_t retval = ESP_OK;
	
	device->rx_channel = NULL;
	device->init_signal_gpio_num = -1;

	rmt_rx_channel_config_t rx_config = {
		.gpio_num = gpio_num,
		.clk_src = RMT_CLK_SRC_DEFAULT,
		.resolution_hz = 1000000,
		.mem_block_symbols = 64,
		
		.flags.invert_in = false,
		.flags.with_dma = false,
		.flags.io_loop_back = false,
	};
	
	rmt_channel_handle_t rx_ch = NULL;
	retval = rmt_new_rx_channel(&rx_config, &rx_ch);
	if (retval != ESP_OK)
	{
		goto end;
	}

	rmt_rx_event_callbacks_t cbs = {
		.on_recv_done = rmt_rx_done_callback
	};

	retval = rmt_rx_register_event_callbacks(rx_ch, &cbs, callback_user_data);
	if (retval != ESP_OK)
	{
		goto end;
	}

	retval = rmt_enable(rx_ch);
	if (retval != ESP_OK)
	{
		goto end;
	}

	gpio_config_t gpio_cfg = {
    	.pin_bit_mask = (1LL << init_signal_gpio_num),
    	.mode = GPIO_MODE_OUTPUT,
    	.pull_up_en = GPIO_PULLUP_DISABLE,
    	.pull_down_en = GPIO_PULLDOWN_DISABLE,
    	.intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_cfg);

	device->rx_channel = rx_ch;
	device->init_signal_gpio_num = init_signal_gpio_num;				
end:
	return retval;
}

esp_err_t DHT11_Read(DHT11_t *device, rmt_symbol_word_t *buffer, size_t buffer_len)
{
	esp_err_t retval = ESP_OK;
	rmt_receive_config_t receive_config = {
		.signal_range_min_ns = 1250,
		.signal_range_max_ns = 20000000
	};
	rmt_receive(device->rx_channel, buffer, buffer_len, &receive_config);
	gpio_set_level(device->init_signal_gpio_num, 1);
	vTaskDelay(pdMS_TO_TICKS(20));
	gpio_set_level(device->init_signal_gpio_num, 0);
	return retval;
}

esp_err_t DHT11_Parse(rmt_symbol_word_t *symbols, size_t symbols_num, DHT11_Reading_t *result)
{
	if (symbols_num != 43)
		return ESP_FAIL;
 	/*for (size_t i = 0; i < symbols_num; i++) {
       	printf("{%d:%d},{%d:%d}\r\n", symbols[i].level0, symbols[i].duration0, symbols[i].level1, symbols[i].duration1);
    }*/
    memset(result, 0, sizeof(DHT11_Reading_t));
	size_t i = 2;
	while (i < 10)
	{
		result->rh_integral |= (symbols[i].duration1 < 40 ? 0 : 1);
		if (i < 9)
		{
			result->rh_integral <<= 1;
		}
		++i;
	}
	while (i < 18)
	{
		result->rh_decimal |= (symbols[i].duration1 < 40 ? 0 : 1);
		if (i < 17)
		{
			result->rh_decimal <<= 1;
		}
		++i;
	}
	while (i < 26)
	{
		result->temp_integral |= (symbols[i].duration1 < 40 ? 0 : 1);
		if (i < 25)
		{
			result->temp_integral <<= 1;
		}
		++i;
	}
	while (i < 34)
	{
		result->temp_decimal |= (symbols[i].duration1 < 40 ? 0 : 1);
		if (i < 33)
		{
			result->temp_decimal <<= 1;
		}
		++i;
	}
	return ESP_OK;
}