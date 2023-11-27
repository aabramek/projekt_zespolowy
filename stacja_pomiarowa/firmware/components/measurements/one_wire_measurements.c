#include "one_wire_measurements.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds18b20.h"
#include "driver/gpio.h"
#include "esp_check.h"

static const char *TAG = "OneWireMeasurement";

void OneWireMeasurementTaskCode(void *pvParameters)
{
	onewire_bus_handle_t bus = NULL;
	
	onewire_bus_config_t bus_config = {
		.bus_gpio_num = GPIO_NUM_18
	};
	
	onewire_bus_rmt_config_t rmt_config = {
		.max_rx_bytes = 10
	};
	
	ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus));
	
	
	ds18b20_device_handle_t ds18b20s[ONE_WIRE_MAX_SENSORS_COUNT];
	onewire_device_iter_handle_t iter = NULL;
	
	onewire_device_t next_onewire_device;
	esp_err_t search_result;
	int device_num;
	float temperature;

	OneWireMeasurementResult_t result = {
		.readings = NULL,
		.num_readings = 0
	};

	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		device_num = 0;
		search_result = ESP_OK;
		ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));
		
		do
		{
			search_result = onewire_device_iter_get_next(iter, 
				&next_onewire_device);
			if (search_result == ESP_OK)
			{
				ds18b20_config_t ds_cfg;
				if (ds18b20_new_device(&next_onewire_device, &ds_cfg,
					&ds18b20s[device_num]) == ESP_OK)
				{
					++device_num;
				}
			}
		}
		while(search_result != ESP_ERR_NOT_FOUND
			&& device_num < ONE_WIRE_MAX_SENSORS_COUNT);

		ESP_ERROR_CHECK(onewire_del_device_iter(iter));	

		if (device_num > result.num_readings)
		{
			result.readings = realloc(result.readings,
				device_num * sizeof(OneWireReading_t));
		}
		result.num_readings = device_num;

		for (int i = 0; i < device_num; ++i)
		{
			ds18b20_trigger_temperature_conversion(ds18b20s[i]);
			ds18b20_get_temperature(ds18b20s[i], &temperature);
			result.readings[i].address = ds18b20s[i]->addr;
			result.readings[i].temperature = temperature;
			ESP_ERROR_CHECK(ds18b20_del_device(ds18b20s[i]));
		}

		xTaskNotifyIndexed((TaskHandle_t)pvParameters,
			ONE_WIRE_MEASUREMENTS_NOTIFICATION_INDEX, (uint32_t)&result,
			eSetValueWithoutOverwrite);
	}
}