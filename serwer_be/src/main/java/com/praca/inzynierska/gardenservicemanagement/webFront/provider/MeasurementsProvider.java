package com.praca.inzynierska.gardenservicemanagement.webFront.provider;

import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.model.Sensor;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.SensorInformationObject;

import java.util.List;

public interface MeasurementsProvider {
    SensorInformationObject getLastMeasurementsForSensors(List<Sensor> sensors);
}
