#include "station.h"

#include "esp_mac.h"
#include "esp_check.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_check.h"
#include "esp_netif.h"
#include "esp_log.h"

#include <stdio.h>

#define WIFI_BITS_SET(bits) (bits & (EVT_WIFI_CONNECTED | EVT_WIFI_FAIL))

// ============================= //
// STATIC FUNCTIONS DECLARATIONS //
// ============================= //

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data);

static esp_err_t wifi_connect(StationHandle_t station);

static void period_timer_callback(TimerHandle_t xTimer);

static void Station_WatererTaskCode(void *pvParemeters);

static void Station_MeasurerTaskCode(void *pvParemeters);

static void Station_ConfigurationReceiverTaskCode(void *pvParemeters);

static void Station_StopTasks(StationHandle_t station);

static void mqtt_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data);

// ===================== //
// FUNCTIONS DEFINITIONS //
// ===================== //

esp_err_t Station_Create(StationHandle_t *station)
{
	const char *TAG = "Station_Create";

	StationHandle_t s = calloc(1, sizeof(Station_t));
	
	if (station == NULL)
	{
		return ESP_FAIL;
	}

	uint8_t mac[6];
	if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot read mac address");
		return ESP_FAIL;
	}

	sprintf(s->configuration.mac_address,
		"%02X-%02X-%02X-%02X-%02X-%02X", 
		mac[0], mac[1], mac[2],
		mac[3], mac[4], mac[5]);

	s->configuration.software_version = SOFTWARE_VERSION;
	s->configuration.measurement_period = DEFAULT_MEASUREMENT_PERIOD;
	
	s->event_group = xEventGroupCreate();
	if (s->event_group == NULL)
	{
		ESP_LOGE(TAG, "Cannot create event group");
		return ESP_FAIL;
	}

	esp_mqtt_client_config_t mqtt_cfg = {
		.broker = {
			.address = {
				.uri = "mqtt://192.168.0.220:11337/"
			}
		},

		.credentials = {
			.username = "esp32",
			.authentication = {
				.password = "mqtt"
			}
		}
	};

	s->mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	if (s->mqtt_client == NULL)
	{
		ESP_LOGE(TAG, "Cannot create mqtt client");
		return ESP_FAIL;
	}

	s->period_timer = xTimerCreate("Station period timer",
		pdMS_TO_TICKS(1000 * 60), pdTRUE, NULL, period_timer_callback);

	if (s->period_timer == NULL)
	{
		ESP_LOGE(TAG, "Cannot create period timer");
		return ESP_FAIL;
	}

	*station = s;
	return ESP_OK;
}

esp_err_t Station_Start(StationHandle_t station)
{
	static const char* TAG = "Station_Start";

	ESP_ERROR_CHECK(wifi_connect(station));

	EventBits_t bits = 0;
	do
	{
		ESP_LOGI(TAG, "Waiting for wifi connection");
		bits = xEventGroupWaitBits(station->event_group, EVT_WIFI_CONNECTED,
			pdTRUE, pdTRUE, pdMS_TO_TICKS(1000)) & EVT_WIFI_CONNECTED;
	}
	while (bits == 0);

	if (esp_mqtt_client_register_event( station->mqtt_client,
		ESP_EVENT_ANY_ID, mqtt_event_handler, station->event_group) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot register mqtt event");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "Registered mqtt event");

	if (esp_mqtt_client_start(station->mqtt_client) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot start mqtt client");
	}
	ESP_LOGI(TAG, "Started mqtt client");

	bits = 0;
	do
	{
		ESP_LOGI(TAG, "Waiting for mqtt client connection");
		bits = xEventGroupWaitBits(station->event_group, EVT_MQTT_CONNECTED,
			pdTRUE, pdTRUE, pdMS_TO_TICKS(1000)) & EVT_MQTT_CONNECTED;
	}
	while (bits == 0);

	ESP_LOGI(TAG, "Mqtt client connected");

	if (esp_mqtt_client_subscribe(station->mqtt_client,
		station->configuration.mac_address, 0) == -1)
	{
		ESP_LOGE(TAG, "Cannot subscribe to configuration receiver topic");
		return ESP_FAIL;
	}

	char register_msg[100];
	
	sprintf(register_msg, "{\"ip\":%lu,\"mac\":\"%s\",\"sv\":%d}",
		station->configuration.ip_address, station->configuration.mac_address, 
		station->configuration.software_version);

	ESP_LOGI(TAG, "%s", register_msg);

	if (esp_mqtt_client_publish(station->mqtt_client, "register", register_msg,
		strlen(register_msg), 2, 0) == -1)
	{
		ESP_LOGE(TAG, "Cannot send registration message");
		return ESP_FAIL;
	}

	do
	{
		bits = xEventGroupWaitBits(station->event_group, EVT_CONFIG_RECEIVED,
			pdFALSE, pdTRUE, pdMS_TO_TICKS(1000)) & EVT_CONFIG_RECEIVED;
		
		ESP_LOGI(TAG, "Waiting for configuration...");
	}
	while (bits == 0);

	ESP_LOGI(TAG, "Registration message receved");
	return ESP_OK;
}

void Station_DumpConfiguration(StationHandle_t station)
{
	fprintf(stdout,

		"Station configuration:\n"
		"ip: %lu.%lu.%lu.%lu\n"
		"mac: %s\n"
		"name: %s\n"
		"measurement_period: %u\n",
		
		IP_TO_STRING(station->configuration.ip_address),
		station->configuration.mac_address,
		station->configuration.name,
		station->configuration.measurement_period);
}

// ============================ //
// STATIC FUNCTIONS DEFINITIONS //
// ============================ //

static void Station_StopTasks(StationHandle_t station)
{
	if (station->tasks.waterer_task != NULL)
	{
		vTaskDelete(station->tasks.waterer_task);
		station->tasks.waterer_task = NULL;
	}

	if (station->tasks.measurer_task != NULL)
	{
		vTaskDelete(station->tasks.measurer_task);
		station->tasks.measurer_task = NULL;
	}

	if (station->tasks.configuration_receiver_task != NULL)
	{
		vTaskDelete(station->tasks.configuration_receiver_task);
		station->tasks.configuration_receiver_task = NULL;
	}
}
/*
static esp_err_t Station_StartTasks(void)
{
	BaseType_t ret;
	ret = xTaskCreate(Station_WatererTaskCode,
		"Waterer task",
		WATERER_TASK_STACK_DEPTH,
		NULL,
		STATION_TASKS_PRIORITY,
		&station->tasks.waterer_task);
	
	if (ret == pdFALSE)
	{
		goto fail;
	}

	ret = xTaskCreate(Station_MeasurerTaskCode,
		"Measurer task",
		MEASURER_TASK_STACK_DEPTH,
		NULL,
		STATION_TASKS_PRIORITY,
		&station->tasks.measurer_task);

	if (ret == pdFALSE)
	{
		goto fail;
	}

	ret = xTaskCreate(Station_ConfigurationReceiverTaskCode,
		"Configuration receiver task",
		CONFIGURATION_RECEIVER_TASK_STACK_DEPTH,
		NULL,
		STATION_TASKS_PRIORITY,
		&station->tasks.configuration_receiver_task);

	if (ret == pdFALSE)
	{
		goto fail;
	}

	return ESP_OK;

fail:
	Station_StopTasks();
	return ESP_FAIL;
}	*/

static esp_err_t wifi_connect(StationHandle_t station)
{
	const char *TAG = "wifi_connect";

	const char *wifi_ssid[] = {
		"Galaxy S22 389D",
		"UPC2194636"
	};

	const char *wifi_passwd[] = {
		"tom12345",
		"hJwtdhP6bnyx"
	};

	esp_err_t ret = ESP_OK;

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler,
		station, NULL));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, 
		station, NULL));

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

	/*EventBits_t bits = 0;
	do
	{
		ESP_LOGI(TAG, "Waiting for wifi connection...");
		bits |= xEventGroupWaitBits(station->event_group,
			EVT_WIFI_CONNECTED | EVT_WIFI_FAIL, pdTRUE, pdFALSE,
			pdMS_TO_TICKS(1000));
	}
	while (!WIFI_BITS_SET(bits));

	if (bits & EVT_WIFI_FAIL)
	{
		ret = ESP_FAIL;
	}*/

	return ret;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data)
{
	const char *TAG = "wifi_event_handler";

	StationHandle_t station = (StationHandle_t)arg;
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
       		station->configuration.ip_address = event->ip_info.ip.addr;
			xEventGroupSetBits(station->event_group, EVT_WIFI_CONNECTED);
		}
	}
}

static void mqtt_event_handler(void *arg, esp_event_base_t event_base, 
	int32_t event_id, void *event_data)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)arg;
	
	esp_mqtt_event_handle_t mqtt_event = (esp_mqtt_event_handle_t)event_data;
	
	switch (mqtt_event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		xEventGroupSetBits(event_group, EVT_MQTT_CONNECTED);
		break;

	case MQTT_EVENT_DATA:
		
		xEventGroupSetBits(event_group, EVT_CONFIG_RECEIVED);
		break;

	default:
		break;
	}
	
}

static void period_timer_callback(TimerHandle_t xTimer)
{

}

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
