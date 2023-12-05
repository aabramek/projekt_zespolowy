package com.praca.inzynierska.gardenservicemanagement.webFront.service.serviceImpl;

import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.model.Sensor;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements.MeasurementsDataRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements.MeasurementsDataResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseException;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseStatus;
import com.praca.inzynierska.gardenservicemanagement.webFront.provider.MeasurementsProvider;
import com.praca.inzynierska.gardenservicemanagement.webFront.provider.SensorProvider;
import com.praca.inzynierska.gardenservicemanagement.webFront.provider.StationProvider;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.MeasurementsService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.stream.Collectors;

@Service
public class MeasurementsServiceImpl implements MeasurementsService {

    StationProvider stationProvider;
    SensorProvider sensorProvider;
    MeasurementsProvider measurementsProvider;

    @Autowired
    public MeasurementsServiceImpl(StationProvider stationProvider, SensorProvider sensorProvider, MeasurementsProvider measurementsProvider) {
        this.stationProvider = stationProvider;
        this.sensorProvider = sensorProvider;
        this.measurementsProvider = measurementsProvider;
    }

    @Override
    public MeasurementsDataResponse getMeasurementsData(Long id, MeasurementsDataRequest request) {
        var station = stationProvider.getStationById(id)
                                     .orElseThrow(() -> new ResponseException("station.not-found", ResponseStatus.NOT_FOUND));
        var sensorList = sensorProvider.getAllForStationAndType(station.getId(), request.getSensorType());
        var measurementsList = measurementsProvider.getMeasurementsForSensors(sensorList.stream()
                                                                                        .map(Sensor::getId)
                                                                                        .collect(Collectors.toList()),
                                                                                        request.getStartDate(),
                                                                                        request.getEndDate());


        System.out.println(measurementsList);



        return null;
    }
}
