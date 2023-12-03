package com.praca.inzynierska.gardenservicemanagement.webFront.provider;

import com.praca.inzynierska.gardenservicemanagement.datastore.measurements.MeasurementsEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.model.Sensor;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.SensorInformationObject;

import java.time.LocalDateTime;
import java.util.List;

public interface MeasurementsProvider {
    SensorInformationObject getLastMeasurementsForSensors(List<Sensor> sensors);

    List<MeasurementsEntity> getMeasurementsForSensors(List<Long> sensorId, LocalDateTime startDate, LocalDateTime endDate);
}
