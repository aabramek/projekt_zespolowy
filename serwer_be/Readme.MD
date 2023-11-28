# SWAGGER

`http://localhost:8082/swagger-ui/index.html#`


# RABBITMQ - INSTALLATION

- `docker pull rabbitmq:3.12.7-management`
- `docker run --rm -it -p 15672:15672 -p 5672:5672 rabbitmq:3.12.7-management`
- `http://localhost:15672/` `guest / guest`

---

# MOSQUITO - INSTALLATION

- `docker-compose up`

---

# EXAMPLE DATA 

### REGISTER QUEUE
`{"ip":285255872,"mac":3477672765554641344,"sv":0}`

### DATA QUEUE

    {
    "mac": 3477672765554641344,
    "wilgotnosc_gleby": [
        {"input_line": 1,"value": 2},
        {"input_line": 2,"value": 2}
    ],
    "sun": [
    {"input_line": 1,"value": 1},
    {"input_line": 2,"value": 2}
    ],
	"dht11": [
		{"input_line": 1,"value_temp": 1,"value_humidity": 1},
		{"input_line": 2,"value_temp": 2,"value_humidity": 2}
	],
	"ds18b20": [
		{"rom_code": 1,"value": 1},
		{"rom_code": 2,"value": 2}
	],
	"timestamp": 123123123
	}



---

# MOSQUITO

## REGISTER MOSQUITO REQUEST

    mosquitto_pub -h localhost -p 1883 -t register -m "{"ip":285255872,"mac":4477672765554641344,"sv":0}"


## DATA MOSQUITO REQUEST


    mosquitto_pub -h localhost -p 1883 -t data -m "{
    "mac": 1477672765554641344,
    "wilgotnosc_gleby": [
        {"input_line": 1,"value": 2},
        {"input_line": 2,"value": 2}
    ],
    "sun": [
    {"input_line": 1,"value": 1},
    {"input_line": 2,"value": 2}
    ],
        "dht11": [
                {"input_line": 1,"value_temp": 1,"value_humidity": 1},
                {"input_line": 2,"value_temp": 2,"value_humidity": 2}
        ],
        "ds18b20": [
                {"rom_code": 1,"value": 1},
                {"rom_code": 2,"value": 2}
        ],
        "timestamp": 123123123
        }"
