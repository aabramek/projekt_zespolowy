--
CREATE TABLE stations (
	id SERIAL PRIMARY KEY,
	mac_address VARCHAR(17) NOT NULL UNIQUE CHECK(mac_address <> ''),
	name VARCHAR(30) NOT NULL UNIQUE CHECK(name <> ''),
	measurement_period SMALLINT NOT NULL CHECK(measurement_period > 0),
	software_version VARCHAR(6) NOT NULL CHECK(software_version <> ''),
	ip_address VARCHAR(16) NOT NULL UNIQUE CHECK(ip_address <> '')
);

--
CREATE TYPE sensor_type AS ENUM('at', 's', 'ah', 'sh');

CREATE TABLE sensors (
	id SERIAL PRIMARY KEY,
	sensor_type sensor_type NOT NULL,
	pin INTEGER NOT NULL CHECK(pin >= 0 AND pin <= 71),
	active BOOLEAN NOT NULL,
	station_id INTEGER REFERENCES stations,
	limit_protection_id INTEGER REFERENCES limit_protections,
	UNIQUE(pin, station_id)
);

--
CREATE TABLE measurements (
	id SERIAL PRIMARY KEY,
	value INTEGER NOT NULL,
	timestamp TIMESTAMP NOT NULL,
	sensor_id INTEGER REFERENCES sensors
);

--
CREATE TABLE valves (
	id SERIAL PRIMARY KEY,
	name VARCHAR(45) NOT NULL,
	pin INTEGER NOT NULL CHECK(pin >= 0 AND pin <= 15),
	station_id INTEGER REFERENCES stations,
	UNIQUE(pin, station_id)
);

--
CREATE TABLE schedules (
	id SERIAL PRIMARY KEY,
	minute_start INTEGER NOT NULL CHECK(minute_start >= 0 AND minute_start <= 59),
	hour_start INTEGER NOT NULL CHECK(hour_start >= 0 AND hour_start <= 23),
	minute_stop INTEGER NOT NULL CHECK(minute_stop >= 0 AND minute_stop <= 59),
	hour_stop INTEGER NOT NULL CHECK(hour_stop >= 0 AND hour_stop <= 23),
	day_of_week INTEGER NOT NULL CHECK(day_of_week >= 0 AND day_of_week <= 6),
	CHECK((hour_start = hour_stop AND minute_start < minute_stop) OR (hour_start < hour_stop))
);

--
CREATE TYPE limit_protection_caluclation_method AS ENUM('min', 'max', 'avg');

CREATE TABLE limit_protections (
	id SERIAL PRIMARY KEY,
	name VARCHAR(45) NOT NULL,
	threshold INT NOT NULL,
	method limit_protection_caluclation_method NOT NULL,
	measurement_type sensor_type NOT NULL,
	below_threshold BOOLEAN NOT NULL
);