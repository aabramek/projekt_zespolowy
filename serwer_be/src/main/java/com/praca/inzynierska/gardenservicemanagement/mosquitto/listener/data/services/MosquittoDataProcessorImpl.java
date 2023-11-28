package com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.data.services;

import com.praca.inzynierska.gardenservicemanagement.common.BinaryParser;
import com.praca.inzynierska.gardenservicemanagement.datastore.measurements.MeasurementType;
import com.praca.inzynierska.gardenservicemanagement.datastore.measurements.MeasurementsEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.measurements.MeasurementsRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.data.model.*;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

@Component
@Slf4j
public class MosquittoDataProcessorImpl implements MosquittoDataProcessor {

    private final MeasurementsRepository measurementsRepository;
    private final StationsRepository stationsRepository;

    @Autowired
    public MosquittoDataProcessorImpl(MeasurementsRepository measurementsRepository, StationsRepository stationsRepository) {
        this.measurementsRepository = measurementsRepository;
        this.stationsRepository = stationsRepository;
    }

    @Override
    public void saveMeasurementData(MosquittoDataRequest request) {
        var macAddress = BinaryParser.getMacAddressFromInt64(request.getMac());
        var station = stationsRepository.findByMacAddress(macAddress);
        if(station.isEmpty()) {
            log.error("Station {} doest not exist!", macAddress);
            return;
        }

        var stationId = station.get().getId();
        var measurementToSave = new ArrayList<MeasurementsEntity>();

        if(request.getSun() != null) {
            Arrays.stream(request.getSun()).forEach(it -> measurementToSave.add(sunMeasurementToEntity(it, request.getTimestamp(), stationId)));
        }
        if(request.getSoilHumidity() != null) {
            Arrays.stream(request.getSoilHumidity()).forEach(it -> measurementToSave.add(wilgotnoscMeasurementToEntity(it, request.getTimestamp(), stationId)));
        }
        if(request.getSoilHumidity() != null) {
            Arrays.stream(request.getDth11()).forEach(it -> measurementToSave.addAll(dth11MeasurementToEntity(it, request.getTimestamp(), stationId)));
        }
        if(request.getSoilHumidity() != null) {
            Arrays.stream(request.getDs18b20()).forEach(it -> measurementToSave.add(dscosMeasurementToEntity(it, request.getTimestamp(), stationId)));
        }

        measurementsRepository.saveAll(measurementToSave);
        log.info("Saved all measurements from device {}", macAddress);
    }

    private MeasurementsEntity sunMeasurementToEntity(Sun sun, long timestamp, Long stationId) {
        return MeasurementsEntity.builder()
                                 .idStation(stationId)
                                 .value(sun.getValue())
                                 .type(MeasurementType.INSOLATION.name())
                                 .timestamp(timestamp)
                                 .build();
    }

    private MeasurementsEntity wilgotnoscMeasurementToEntity(SoilHumidity wilgotnosc, long timestamp, Long stationId) {
        return MeasurementsEntity.builder()
                                 .idStation(stationId)
                                 .value(wilgotnosc.getValue())
                                 .type(MeasurementType.SOIL_HUMIDITY.name())
                                 .timestamp(timestamp)
                                 .build();
    }

    private List<MeasurementsEntity> dth11MeasurementToEntity(Dth11 dth11, long timestamp, Long stationId) {
        var humidityObject = MeasurementsEntity.builder()
                                               .idStation(stationId)
                                               .value(dth11.getValueHumidity())
                                               .type(MeasurementType.AIR_HUMIDITY.name())
                                               .timestamp(timestamp)
                                               .build();

        var tempObject = MeasurementsEntity.builder()
                                           .idStation(stationId)
                                           .value(dth11.getValueTemp())
                                           .type(MeasurementType.AIR_TEMP.name())
                                           .timestamp(timestamp)
                                           .build();

        return List.of(humidityObject, tempObject);
    }

    private MeasurementsEntity dscosMeasurementToEntity(Ds18b20 ds18b20, long timestamp, Long stationId) {
        return MeasurementsEntity.builder()
                                 .idStation(stationId)
                                 .value(ds18b20.getValue())
                                 .type(MeasurementType.AIR_TEMP.name())
                                 .timestamp(timestamp)
                                 .build();
    }

}
