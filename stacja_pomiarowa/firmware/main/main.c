#include "station.h"
#include "watering.h"
#include "status_indicator.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_check.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_console.h"

static const char* TAG = "Main";

static int set_cmd_function(int argc, char **argv);

static int restart_cmd_function(int argc, char **argv);

static int showcfg_cmd_function(int argc, char **argv);

static int wateringcfg_cmd_function(int argc, char **argv);

static int write_mqtt_port_property(nvs_handle_t nvs, uint32_t port);

static int write_string_property(nvs_handle_t nvs, const char *key,
	const char *value, size_t max_length);

static esp_err_t create_console(void);

static StationHandle_t station = NULL;

void app_main(void)
{
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES
		|| err == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot initialize nvs structure: %s", 
			esp_err_to_name(err));
		abort();
	}

	ESP_LOGI(TAG, "Nvs structure initialized");

	StatusIndicator_t status_indicator;
	err = status_indicator_init(&status_indicator, 17, 16);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to initialize status indicator");
		goto end;
	}

	status_indicator_clear(&status_indicator);

	err = create_console();
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to create console");
		goto end;
	}

	err = station_create(&station);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot create station");
		goto end;
	}

	ESP_LOGI(TAG, "Station created");

	err = station_start(station);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot start station");
		goto end;
	}
	
	ESP_LOGI(TAG, "Station started");

end:	
	status_indicator_indicate(&status_indicator, err == ESP_OK ? OK : FAIL);

	vTaskDelete(NULL);
}

static int set_cmd_function(int argc, char **argv)
{
	if (argc != 3)
	{
		return 1;
	}

	int return_code = 0;
	int selected_property = 0;

	typedef struct
	{
		const char *name;
		size_t max_length;
	}
	property_t;

	const property_t properties[] = {
		{"mqtt_port", -1},
		{"mqtt_host", MQTT_HOST_LEN},
		{"wifi_ssid", 31},
		{"wifi_password", 63}
	};

	for (int i = 0; i < 4; ++i)
	{
		if (strcmp(argv[1], properties[i].name) == 0)
		{
			selected_property = i;
			break;
		}
	}

	if (selected_property > 3)
	{
		return 2;
	}

	nvs_handle_t nvs;
	esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to open nvs storage: %s",
			esp_err_to_name(err));

		return 3;
	}

	if (selected_property == 0)
	{
		return_code = write_mqtt_port_property(nvs, (uint32_t)atoi(argv[2]));
	}
	else
	{
		return_code = write_string_property(nvs, argv[1], argv[2],
			properties[selected_property].max_length);
	}
	
	if (return_code != 0)
	{
		goto end;
	}

	err = nvs_commit(nvs);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to commit changes to nvs: %s",
			esp_err_to_name(err));
	}

end:
	nvs_close(nvs);

	return return_code;
}

static int write_string_property(nvs_handle_t nvs, const char *key,
	const char *value, size_t max_length)
{
	size_t len = strlen(value); 
	if (len > max_length)
	{
		ESP_LOGE(TAG, "%s property shouldn't be longer than %d, is %d",
			key, max_length, len);
		
		return 5;
	}

	esp_err_t err = nvs_set_str(nvs, key, value);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to write %s property: %s", key,
			esp_err_to_name(err));
		
		return 4;
	}

	return 0;
}

static int write_mqtt_port_property(nvs_handle_t nvs, uint32_t port)
{	
	if (port < 1024 || port > 65535)
	{
		ESP_LOGE(TAG, "mqtt_port should be in range [1024, 65535], is %lu",
			port);

		return 5;
	}

	esp_err_t err = nvs_set_u32(nvs, "mqtt_port", (uint32_t)port);
	
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to write mqtt_port property: %s", 
			esp_err_to_name(err));
		
		return 5;
	}

	return 0;
}

static int restart_cmd_function(int argc, char **argv)
{
	esp_restart();
	return 0;
}

static int showcfg_cmd_function(int argc, char **argv)
{
	station_dump_configuration(station);
	return 0;
}

static int wateringcfg_cmd_function(int argc, char **argv)
{
	valves_dump_configuration();
	return 0;
}

static esp_err_t create_console(void)
{
	esp_err_t err = ESP_OK;

	esp_console_repl_t *repl = NULL;
	
	esp_console_repl_config_t repl_cfg = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
	repl_cfg.max_history_len = 1;
	repl_cfg.prompt = "#";
	repl_cfg.max_cmdline_length = 84;
	
	esp_console_cmd_t commands[] = {
		{
			.command = "set",
			.help = "usage set [key] [value] Keys: mqtt_port, mqtt_host",
			.hint = NULL,
			.func = set_cmd_function
		},

		{
			.command = "restart",
			.help = "restarts device",
			.hint = NULL,
			.func = restart_cmd_function
		},

		{
			.command = "stationcfg",
			.help = "shows station configuration",
			.hint = NULL,
			.func = showcfg_cmd_function
		},

		{
			.command = "wateringcfg",
			.help = "shows watering configuration",
			.hint = NULL,
			.func = wateringcfg_cmd_function
		}
	};

	for (int i = 0; i < 4; ++i)
	{
		err = esp_console_cmd_register(&commands[i]) != ESP_OK;
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to register \"%s\" command: %s",
				commands[i].command, esp_err_to_name(err));
		}
	}

	esp_console_dev_uart_config_t hw_config =
		ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    
    err = esp_console_new_repl_uart(&hw_config, &repl_cfg, &repl);
    if (err != ESP_OK)
    {
    	ESP_LOGE(TAG, "Failed to create new uart repl: %s",
    		esp_err_to_name(err));
    	goto end;
    }

	err = esp_console_start_repl(repl);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to start repl: %s", esp_err_to_name(err));
	}

end:
	return err;
}
