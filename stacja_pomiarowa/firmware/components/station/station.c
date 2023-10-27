#include "station.h"

#include "esp_mac.h"
#include "esp_check.h"

static void Station_WatererTaskCode(void *pvParemeters)
{
	while (1)
	{

	}
}

static void Station_MeasurerTaskCode(void *pvParemeters)
{
	while (1)
	{

	}
}

static void Station_ConfigurationReceiverTaskCode(void *pvParemeters)
{
	while (1)
	{

	}
}


Station_t _station = {
	.ip_address = 0,
	.mac_address = {0},
	.software_version = SOFTWARE_VERSION,
	
	.name = "Default name",
	.measurement_period = DEFAULT_MEASUREMENT_PERIOD,

	.waterer_task = NULL,
	.measurer_task = NULL,
	.configuration_receiver_task = NULL
};

EventGroupHandle_t _station_event_group = NULL;

esp_mqtt_client_handle_t _mqtt_client = NULL;

static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t mqtt_event = (esp_mqtt_event_handle_t)event_data;
	switch (mqtt_event->event_id)
	{
		case MQTT_EVENT_SUBSCRIBED:

			break;

		case MQTT_EVENT_DATA:
			xEventGroupSetBits(_station_event_group, EVT_CONFIG_RECEIVED);
			break;

	default:
		break;
	}
	
}

void Station_Start(void)
{
	static const char* TAG = "Station_Start";

	if (esp_read_mac(_station.mac_address, ESP_MAC_WIFI_STA) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot read mac address");
		abort();
	}

	char mac_str[18];
	sprintf(mac_str, "%02X-%02X-%02X-%02X-%02X-%02X", 
		_station.mac_address[0], _station.mac_address[1], _station.mac_address[2],
		_station.mac_address[3], _station.mac_address[4], _station.mac_address[5]);

	esp_mqtt_client_config_t mqtt_cfg = {
		.broker = {
			.address = {
				.uri = "mqtt://192.168.186.99:1883/"
			}
		}/*,

		.credentials = {
			.username = "esp32",
			.authentication = {
				.password = "mqtt"
			}
		}*/
	};

	_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	if (_mqtt_client == NULL) {
		ESP_LOGE(TAG, "Cannot create mqtt client");
		abort();
	}

	ESP_ERROR_CHECK(esp_mqtt_client_register_event(_mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
	ESP_ERROR_CHECK(esp_mqtt_client_start(_mqtt_client));
	esp_mqtt_client_subscribe(_mqtt_client, mac_str, 0);	
	char register_msg[100];
	sprintf(register_msg, "{\"ip\":%lu,\"mac\":\"%s\",\"sv\":%d}", _station.ip_address, mac_str, _station.software_version);
	ESP_LOGI(TAG, "%s", register_msg);

	ESP_ERROR_CHECK(esp_mqtt_client_publish(_mqtt_client, "register", register_msg, strlen(register_msg), 0, 0));

	EventBits_t bits = 0;
	while (bits == 0)
	{
		ESP_LOGI(TAG, "Waiting for configuration...");
		bits = xEventGroupWaitBits(_station_event_group, EVT_CONFIG_RECEIVED, pdFALSE, pdTRUE, pdMS_TO_TICKS(1000)) & EVT_CONFIG_RECEIVED;
	}

	ESP_LOGI(TAG, "register ok");
}

static void Station_StopTasks(void)
{
	if (_station.waterer_task != NULL)
	{
		vTaskDelete(_station.waterer_task);
		_station.waterer_task = NULL;
	}

	if (_station.measurer_task != NULL)
	{
		vTaskDelete(_station.measurer_task);
		_station.measurer_task = NULL;
	}

	if (_station.configuration_receiver_task != NULL)
	{
		vTaskDelete(_station.configuration_receiver_task);
		_station.configuration_receiver_task = NULL;
	}
}

static esp_err_t Station_StartTasks(void)
{
	BaseType_t ret;
	ret = xTaskCreate(Station_WatererTaskCode,
		"Waterer task",
		WATERER_TASK_STACK_DEPTH,
		NULL,
		STATION_TASKS_PRIORITY,
		&_station.waterer_task);
	
	if (ret == pdFALSE)
	{
		goto fail;
	}

	ret = xTaskCreate(Station_MeasurerTaskCode,
		"Measurer task",
		MEASURER_TASK_STACK_DEPTH,
		NULL,
		STATION_TASKS_PRIORITY,
		&_station.measurer_task);

	if (ret == pdFALSE)
	{
		goto fail;
	}

	ret = xTaskCreate(Station_ConfigurationReceiverTaskCode,
		"Configuration receiver task",
		CONFIGURATION_RECEIVER_TASK_STACK_DEPTH,
		NULL,
		STATION_TASKS_PRIORITY,
		&_station.configuration_receiver_task);

	if (ret == pdFALSE)
	{
		goto fail;
	}

	return ESP_OK;

	fail:
	Station_StopTasks();
	return ESP_FAIL;
}	
