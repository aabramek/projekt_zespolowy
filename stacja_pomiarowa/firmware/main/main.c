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

#define WIFI_BITS_SET(bits) (bits & (EVT_WIFI_CONNECTED | EVT_WIFI_FAIL))

static const char* TAG = "Main";

static void wifi_set_static_ip(esp_netif_t *netif)
{
	
}

static void wifi_init_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT)
	{
		if (event_id == WIFI_EVENT_STA_START)
		{
			esp_wifi_connect();
		}
		else if (event_id == WIFI_EVENT_STA_CONNECTED)
		{

		}
		else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
		{
			ESP_LOGW(TAG, "Wifi connection lost, retrying");
			esp_wifi_connect();
		}
	}
	else
	{
		if (event_id == IP_EVENT_STA_GOT_IP)
		{
			ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
       		_station.ip_address = event->ip_info.ip.addr;
			xEventGroupSetBits(_station_event_group, EVT_WIFI_CONNECTED);
		}
	}
}


static esp_err_t wifi_connect(/*const unsigned char *ssid, const unsigned char* password*/)
{
	esp_err_t ret = ESP_OK;

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_init_event_handler, NULL, NULL));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_init_event_handler, NULL, NULL));

	wifi_config_t wifi_cfg = {
		.sta = {
			.ssid = "Galaxy S22 389D",
			.password = "tom12345",
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,
			.pmf_cfg = {
				.capable = true,
				.required = false
			}
		}
	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
	ESP_ERROR_CHECK(esp_wifi_start());

	EventBits_t bits = 0;
	do
	{
		ESP_LOGI(TAG, "Waiting for wifi connection...");
		bits |= xEventGroupWaitBits(_station_event_group, EVT_WIFI_CONNECTED | EVT_WIFI_FAIL, pdTRUE, pdFALSE, pdMS_TO_TICKS(1000));
	}
	while (!WIFI_BITS_SET(bits));

	if (bits & EVT_WIFI_FAIL)
	{
		ret = ESP_FAIL;
	}

	return ret;
}

static esp_err_t peripherals_initialize()
{
	return ESP_OK;
}

void app_main(void)
{
	_station_event_group = xEventGroupCreate();
	if (_station_event_group == NULL)
	{
		ESP_LOGE(TAG, "Cannot create station event group");
		abort();
	}

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
	
	if (wifi_connect(/*"UPC2194636", "hJwtdhP6bnyx"*/) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot connect to wifi");
		abort();
	}
	else
	{
		ESP_LOGI(TAG, "Successfully connected to wifi");
	}

	Station_Start();

	vTaskDelete(NULL);
}
