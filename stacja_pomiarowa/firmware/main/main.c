#include "main.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"

#include "cJSON.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_check.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_mac.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mqtt_client.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/i2c.h"
#include "drivers.h"

#include "station.h"

#define WIFI_SUCCESS 1
#define WIFI_FAILURE 2
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define MCP23017_ADDR 0x27

static uint32_t ip_addr = 0;

TimerHandle_t _period_timer = NULL;

static const char* TAG = "Main";

static EventGroupHandle_t wifi_init_event_group;

static void mqtt_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	esp_mqtt_event_handle_t mqtt_event = (esp_mqtt_event_handle_t)event_data;
	switch (mqtt_event->event_id)
	{
		case MQTT_EVENT_SUBSCRIBED:
			ESP_LOGI(TAG, "Client subscribed topic");
			break;

	case MQTT_EVENT_DATA:
			/*ESP_LOGI(TAG, "%.*s:%.*s", mqtt_event->topic_len, mqtt_event->topic, mqtt_event->data_len, mqtt_event->data);
			cJSON* json = cJSON_ParseWithLength(mqtt_event->data, mqtt_event->data_len);
			if (json)
			{
				cJSON* port = NULL;
				port = cJSON_GetObjectItemCaseSensitive(json, "port");
				cJSON* val = NULL;
				val = cJSON_GetObjectItemCaseSensitive(json, "val");
				if (cJSON_IsString(port) && port->valuestring != NULL && cJSON_IsNumber(val))
				{
					ESP_LOGI(TAG, "Port: %s, val: %d", port->valuestring, val->valueint);
					if (strncmp(port->valuestring, "PORTA", 5) == 0)
					{
						ESP_ERROR_CHECK(MCP23017_Write(MCP23017_ADDR, GPIOA, val->valueint));
					}
					else if (strncmp(port->valuestring, "PORTB", 5) == 0)
					{
						ESP_ERROR_CHECK(MCP23017_Write(MCP23017_ADDR, GPIOB, val->valueint));
					}
				} 
				else
				{
					ESP_LOGW(TAG, "cannot extract json object properties");
				}
			}
			else
			{
				ESP_LOGW(TAG, "json couldn't be parsed");
			}
			free(json);*/
			break;

	default:
		break;
	}
	
}

static void wifi_set_static_ip(esp_netif_t *netif)
{
	
}

static void wifi_init_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
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
			xEventGroupSetBits(wifi_init_event_group, WIFI_FAIL_BIT);
		}
	}
	else
	{
		if (event_id == IP_EVENT_STA_GOT_IP)
		{
			ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
       		ip_addr = event->ip_info.ip.addr;
			xEventGroupSetBits(wifi_init_event_group, WIFI_CONNECTED_BIT);
		}
	}
}


static esp_err_t wifi_initialize(void)
{
	esp_err_t ret = ESP_OK;

	wifi_init_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_init_event_handler, NULL, NULL));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_init_event_handler, NULL, NULL));

	wifi_config_t wifi_cfg = {
		.sta = {
			.ssid = "UPC2194636",
			.password = "hJwtdhP6bnyx",
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

	EventBits_t bits = xEventGroupWaitBits(wifi_init_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

	if (bits & WIFI_CONNECTED_BIT)
	{
		ESP_LOGI(TAG, "Połączono do sieci wifi");
	}
	else 
	{
		ESP_LOGE(TAG, "Nie udało się połączyć");
		ret = ESP_FAIL;
	}

	vEventGroupDelete(wifi_init_event_group);

	return ret;
}

static esp_err_t peripherals_initialize()
{
	return ESP_OK;
}

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	ESP_ERROR_CHECK(peripherals_initialize());
	ESP_ERROR_CHECK(wifi_initialize());

	_period_timer = xTimerCreate("Period Timer", pdMS_TO_TICKS(1000), pdTRUE, PERIOD_TIMER_ID, PeriodTimerCallback);
	if (_period_timer == NULL)
	{
		ESP_LOGE(TAG, "Cannot create period software timer");
		abort();
	}

	uint8_t mac[6];
	char mac_str[18];
	ESP_ERROR_CHECK(esp_efuse_mac_get_default(mac));
	sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	esp_mqtt_client_config_t mqtt_cfg = {
		.broker = {
			.address = {
				.uri = "mqtt://192.168.0.171:9092/"
			}
		},

		.credentials = {
			.username = "esp32",
			.authentication = {
				.password = "mqtt"
			}
		}
	};

	esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	if (mqtt_client == NULL) {
		ESP_LOGE(TAG, "Cannot create mqtt client");
		abort();
	}

	ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_client));
	char register_msg[300];
	sprintf(register_msg, "{\"msg_type\":0,\"station\":{\"ip\":%lu,\"mac\":%llu,\"sv\":%d}}", ip_addr, *((uint64_t*)mac), 0);
	ESP_LOGI(TAG, "%s", register_msg);
	//ESP_ERROR_CHECK(esp_mqtt_client_publish(mqtt_client, "register", mac_str, 17, 0, 0));
	// ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));

	vTaskDelete(NULL);
}
