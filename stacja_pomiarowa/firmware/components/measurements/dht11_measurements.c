#include "dht11_measurements.h"
#include "dht11.h"
#include "station.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>

#define RMT_CALLBACK_NOTIFICATION_INDEX 1

static const char *TAG = "DHT11Measurement";

typedef struct
{
	size_t num_symbols;
	TaskHandle_t task;
}
Rmt_Callback_Context_t;

static bool rmt_done_callback(rmt_channel_handle_t rx_ch,
	const rmt_rx_done_event_data_t *edata, void *context);

void DHT11MeasurementTaskCode(void *pvParameters)
{
	TaskHandle_t task = xTaskGetCurrentTaskHandle();
	
	DHT11_t dht11;
	DHT11MeasurementResult_t result;

	result.available = false;

	rmt_symbol_word_t *buffer = malloc(sizeof(rmt_symbol_word_t) * 64);
	
	Rmt_Callback_Context_t rmt_callback_context = {
		.num_symbols = 0,
		.task = task
	};

	DHT11_Init(&dht11, 23, 19, rmt_done_callback, &rmt_callback_context);

	while (1)
	{
		ulTaskNotifyTakeIndexed(0, pdTRUE, portMAX_DELAY);
		
		DHT11_Read(&dht11, buffer, sizeof(rmt_symbol_word_t) * 43);
		
		ulTaskNotifyTakeIndexed(RMT_CALLBACK_NOTIFICATION_INDEX, pdTRUE,
			pdMS_TO_TICKS(60));
		
		if (rmt_callback_context.num_symbols == 43)
		{
			DHT11_Parse(buffer, 43, &result.reading);
			result.available = true;
		}
		else
		{
			result.available = false;
		}
		
		xTaskNotifyIndexed((TaskHandle_t)pvParameters,
			DHT11_MEASUREMENTS_NOTIFICATION_INDEX, (uint32_t)&result,
			eSetValueWithoutOverwrite);
	}
}

static bool rmt_done_callback(rmt_channel_handle_t rx_ch,
	const rmt_rx_done_event_data_t *edata, void *context)
{
	BaseType_t higher_task_awoken = pdFALSE;
	
	Rmt_Callback_Context_t *ctx = (Rmt_Callback_Context_t*)context;

	ctx->num_symbols = edata->num_symbols;

	vTaskNotifyGiveIndexedFromISR(ctx->task, RMT_CALLBACK_NOTIFICATION_INDEX,
		&higher_task_awoken);
	
	return higher_task_awoken;
}
