package com.praca.inzynierska.gardenservicemanagement.datastore.sensors.mapper;

import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.SensorEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.model.Sensor;

public class SensorMapper {
    public static Sensor toSensor(SensorEntity sensorEntity) {
        return Sensor.builder()
                     .id(sensorEntity.getId())
                     .sensorType(sensorEntity.getSensorType())
                     .address(sensorEntity.getAddress())
                     .isActive(sensorEntity.isActive())
                     .stationId(sensorEntity.getStationId())
                     .build();
    }
}
