package com.praca.inzynierska.gardenservicemanagement.datastore.sensors.model;

import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.SensorType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@AllArgsConstructor
@NoArgsConstructor
@Data
@Builder
public class Sensor {

    private Long id;
    private SensorType sensorType;
    private Long address;
    private boolean isActive;
    private Long stationId;
}
