#include "analog_measurements.h"
#include "ads1115.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"

static const char *TAG = "AnalogMeasurement";

void AnalogMeasurementTaskCode(void *pvParameters)
{
	ADS1115_t adc = {
		.address = ADDR_GND,
		.configuration = ADS1115_DEFAULT_CONFIG,
		.i2c_port = I2C_NUM_0
	};

	ADS1115_SetPGA(&adc, PGA_6_144V);
	ADS1115_SaveConfiguration(&adc);

	AnalogMeasurementResult_t result;
	int32_t reading;

	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		for (uint16_t i = 0, mux_address = MUX_ADDR_A0; i < 4; ++i, 
			mux_address += 0x1000)
		{
			ADS1115_AddressMux(&adc, mux_address);
			ADS1115_SaveConfiguration(&adc);
			ADS1115_StartConversion(&adc);
			
			vTaskDelay(pdMS_TO_TICKS(10));
			
			ADS1115_ReadRegister(&adc, CONVERSION_REG, 
				(uint16_t*)&reading);

			reading = reading * 375 / 2 / 1000;
			result.values[i] = reading;
		}		

		xTaskNotifyIndexed((TaskHandle_t)pvParameters,
			ANALOG_MEASUREMENTS_NOTIFICATION_INDEX, (uint32_t)&result,
			eSetValueWithoutOverwrite);
	}
}