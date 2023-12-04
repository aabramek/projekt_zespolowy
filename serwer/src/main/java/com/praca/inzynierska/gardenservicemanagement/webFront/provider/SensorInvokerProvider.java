package com.praca.inzynierska.gardenservicemanagement.webFront.provider;

import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.SensorType;
import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.SensorsRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.mapper.SensorMapper;
import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.model.Sensor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class SensorInvokerProvider implements SensorProvider {
    
    SensorsRepository sensorsRepository;

    @Autowired
    public SensorInvokerProvider(SensorsRepository sensorsRepository) {
        this.sensorsRepository = sensorsRepository;
    }

    @Override
    public List<Sensor> getAllSensorsForStation(Long stationId) {
        var sensorList = sensorsRepository.findByStationId(stationId);
        return sensorList.stream().map(SensorMapper::toSensor).toList();
    }

    @Override
    public List<Sensor> getAllForStationAndType(Long stationId, SensorType sensorType) {
        return sensorsRepository.findByStationIdAndSensorType(stationId, sensorType)
                                .stream()
                                .map(SensorMapper::toSensor)
                                .toList();
    }
}
