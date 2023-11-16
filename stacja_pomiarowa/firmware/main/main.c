#include "station.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_check.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/i2c.h"

static const char* TAG = "Main";

/*static void wifi_set_static_ip(esp_netif_t *netif)
{
	
}
*/

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	
	if (ret != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot initialize wifi nvs structure");
		abort();
	}
	
	StationHandle_t station = NULL;
	if (Station_Create(&station) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot create station");
		abort();
	}

	Station_Start(station);

	Station_DumpConfiguration(station);

	vTaskDelete(NULL);
}
