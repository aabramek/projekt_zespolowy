#include "sh_measurement.h"
#include "mux.h"

#include <inttypes.h>

#include "esp_check.h"

#include "driver/gpio.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

/*uint32_t sh_sensors = 0;

static const char* TAG = "SH_Measurement";
static adc_oneshot_unit_handle_t adc_unit_handle = NULL;
static adc_cali_handle_t adc_cali_handle = NULL;
static const adc_channel_t channels[] = {
	ADC_CHANNEL_0, ADC_CHANNEL_3, ADC_CHANNEL_6, ADC_CHANNEL_7
};

esp_err_t sh_measurement_init(void)
{
	esp_err_t status;
	
	// konfiguracja schematu kalibracji
	adc_cali_line_fitting_config_t adc_cali_config = {
		.unit_id = 	ADC_UNIT_1,
		.atten = 	ADC_ATTEN_DB_11,
		.bitwidth = ADC_BITWIDTH_10
	};

	status = adc_cali_create_scheme_line_fitting(&adc_cali_config, &adc_cali_handle);

	if (status != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot create calibration scheme for adc");
		return status;
	}

	// konfiguracja adc
	adc_oneshot_unit_init_cfg_t adc_cfg = {
		.unit_id = 	ADC_UNIT_1,
		.ulp_mode = ADC_ULP_MODE_DISABLE
	};
	
	status = adc_oneshot_new_unit(&adc_cfg, &adc_unit_handle);

	if (status != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot create adc oneshot unit");
		adc_cali_delete_scheme_line_fitting(adc_cali_handle);
		return status;
	}

	// konfiguracja kanałów adc
	adc_oneshot_chan_cfg_t adc_channel_cfg = {
		.atten = ADC_ATTEN_DB_11,
		.bitwidth = ADC_BITWIDTH_10
	};

	for (int i = 0; i < 4; ++i)
	{
		status = adc_oneshot_config_channel(adc_unit_handle, channels[i], &adc_channel_cfg);
		if (status != ESP_OK)
		{
			ESP_LOGE(TAG, "Cannot configure channel #%d of adc", channels[i]);
			adc_oneshot_del_unit(adc_unit_handle);
			adc_cali_delete_scheme_line_fitting(adc_cali_handle);
			return status;
		}
	}

	ESP_LOGI(TAG, "Peripherals has been successfully initialized");

	return ESP_OK;
}

uint32_t sh_measurement_add_sensors(uint32_t sensors)
{
	
	sh_sensors |= (1 << input_line);
}

uint32_t sh_measurement_remove_sensor(uint32_t input_line)
{
	if (input_line > 31)
		return ESP_ERR_INVALID_ARG;

	if (sh_measurement_check_sensor(input_line))
		return INPUT_LINE_FREE;

	sh_sensors &= ~(1 << input_line);

	return ESP_OK;
}

uint32_t sh_measurement_check_sensor(uint32_t input_line)
{
	return sh_sensors & (1 << input_line);
}

void address_mux(int addr)
{
	gpio_set_level(MUX_ADDR_A, addr & 1);
	gpio_set_level(MUX_ADDR_B, addr & 2);
	gpio_set_level(MUX_ADDR_C, addr & 4);
}

esp_err_t sh_measure(uint32_t* results)
{
	if (adc_unit_handle == NULL || adc_cali_handle == NULL)
	{
		return ESP_ERR_INVALID_STATE;
	}

	int status = ESP_OK;
	int idx = 0;
	uint32_t sensor = 1;

	for (int i = 0; i < 4; ++i)
	{	
		for (int j = 0; j < 8; ++j, sensor <<= 1)
		{
			if (sh_measurement_check_sensor(sensor) == 0)
			{
				continue;
			}

			address_mux(j);

			int value_raw;
			int value_calibrated;
			
			status = adc_oneshot_read(adc_unit_handle, channels[i], &value_raw);
			
			if (status == ESP_ERR_TIMEOUT)
			{
				ESP_LOGW(TAG, "Timeout while measuring soil humidity");
			}
			else
			{
				status = adc_cali_raw_to_voltage(adc_cali_handle, value_raw, &value_calibrated);
				if (status != ESP_OK)
				{
					ESP_LOGW(TAG, "Cannot convert raw data to calibrated voltage");
					results[idx] = -1;
				}
				else
				{
					results[idx] = value_calibrated;
				}
			}
		}
	}


	return status;
}*/