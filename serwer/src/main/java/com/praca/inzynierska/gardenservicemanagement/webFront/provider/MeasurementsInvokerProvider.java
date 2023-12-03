package com.praca.inzynierska.gardenservicemanagement.webFront.provider;

import com.praca.inzynierska.gardenservicemanagement.datastore.measurements.MeasurementType;
import com.praca.inzynierska.gardenservicemanagement.datastore.measurements.MeasurementsEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.measurements.MeasurementsRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.SensorType;
import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.model.Sensor;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.AnalogSensorInformation;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.Dht11Information;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.Ds18b20Information;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.SensorInformationObject;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.ObjectUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;


import java.sql.Timestamp;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.time.temporal.ChronoField;
import java.util.List;
import java.util.stream.Collectors;

@Service
@Slf4j
public class MeasurementsInvokerProvider implements MeasurementsProvider {
    
    MeasurementsRepository measurementsRepository;

    @Autowired
    public MeasurementsInvokerProvider(final MeasurementsRepository measurementsRepository) {
        this.measurementsRepository = measurementsRepository;
    }

    @Override
    public SensorInformationObject getLastMeasurementsForSensors(final List<Sensor> sensors) {
        var lastMeasurements = measurementsRepository.getLastMeasurementsForSensors(sensors.stream()
                                                                                           .map(Sensor::getId)
                                                                                           .toList());
        var dth11List = toDth11List(sensors.stream()
                                           .filter(sensor -> sensor.getSensorType().equals(SensorType.DHT))
                                           .toList(), lastMeasurements);

        var analogList = toAnalogList(sensors.stream()
                                             .filter(sensor -> sensor.getSensorType().equals(SensorType.SH) ||
                                                               sensor.getSensorType().equals(SensorType.S)).toList(), lastMeasurements);

        var ds18 = toDs18List(sensors.stream()
                                     .filter(sensor -> sensor.getSensorType().equals(SensorType.DS))
                                     .toList(), lastMeasurements);


        return SensorInformationObject.builder()
                                      .analogSensorInformation(analogList)
                                      .dht11InformationList(dth11List)
                                      .ds18b20InformationList(ds18)
                                      .build();
    }
    @Override
    public List<MeasurementsEntity> getMeasurementsForSensors(List<Long> sensorId, LocalDateTime startDate, LocalDateTime endDate) {
        LocalDateTime startOfDate = startDate.with(ChronoField.NANO_OF_DAY, LocalTime.MIN.toNanoOfDay());
        LocalDateTime endOfDate = startDate.with(ChronoField.NANO_OF_DAY, LocalTime.MAX.toNanoOfDay());
        return measurementsRepository.getMeasurementsForSensorsAndBetweenDate(sensorId,
                                                                              Timestamp.valueOf(startOfDate).getTime(),
                                                                              Timestamp.valueOf(endOfDate).getTime());
    }

    private List<Ds18b20Information> toDs18List(final List<Sensor> list, final List<MeasurementsEntity> lastMeasurements) {
        return list.stream().map(sensor -> toDs18Information(sensor, lastMeasurements)).collect(Collectors.toList());
    }

    private List<AnalogSensorInformation> toAnalogList(final List<Sensor> list, final List<MeasurementsEntity> lastMeasurements) {
        return list.stream().map(sensor -> toAnalogInformation(sensor, lastMeasurements)).collect(Collectors.toList());
    }

    private List<Dht11Information> toDth11List(final List<Sensor> list, final List<MeasurementsEntity> lastMeasurements) {
        return list.stream().map(sensor -> toDht11Information(sensor, lastMeasurements)).collect(Collectors.toList());
    }

    private Dht11Information toDht11Information(final Sensor sensor, final List<MeasurementsEntity> lastMeasurements) {
        var measurements = findMeasurementsForSensor(sensor.getId(), lastMeasurements);

        var temp = measurements.stream()
                               .filter(el -> el.getType().equals(MeasurementType.AIR_TEMP))
                               .findFirst();

        var airHumidity = measurements.stream()
                                      .filter(el -> el.getType().equals(MeasurementType.AIR_HUMIDITY))
                                      .findFirst();

        return Dht11Information.builder()
                               .inputLine(sensor.getAddress())
                               .isActive(sensor.isActive())
                               .temperature(temp.isPresent() ? temp.get().getValue() : null)
                               .airHumidity(airHumidity.isPresent() ? airHumidity.get().getValue() : null)
                               .lastMeasurement(temp.map(MeasurementsEntity::getTimestamp).orElse(null))
                               .build();
    }

    private AnalogSensorInformation toAnalogInformation(final Sensor sensor, final List<MeasurementsEntity> lastMeasurements) {
        var measurements = findMeasurementsForSensor(sensor.getId(), lastMeasurements);

        var insolation = measurements.stream()
                                         .filter(el -> el.getType().equals(MeasurementType.INSOLATION))
                                         .findFirst();

        var soilHumidity = measurements.stream()
                                           .filter(el -> el.getType().equals(MeasurementType.SOIL_HUMIDITY))
                                           .findFirst();

        return AnalogSensorInformation.builder()
                                      .inputLine(sensor.getAddress())
                                      .isActive(sensor.isActive())
                                      .insolation(insolation.isPresent() ? insolation.get().getValue() : null)
                                      .humidity(soilHumidity.isPresent() ? soilHumidity.get().getValue() : null)
                                      .lastMeasurement(resolveMeasurementDate(insolation.orElse(null), soilHumidity.orElse(null)))
                                      .build();
    }

    private Ds18b20Information toDs18Information(final Sensor sensor, final List<MeasurementsEntity> lastMeasurements) {
        var measurements = findMeasurementsForSensor(sensor.getId(), lastMeasurements);

        var temp = measurements.stream()
                               .filter(el -> el.getType().equals(MeasurementType.AIR_TEMP))
                               .findFirst();

        return Ds18b20Information.builder()
                                 .address(sensor.getAddress().toString())
                                 .isActive(sensor.isActive())
                                 .temperature(temp.isPresent() ? temp.get().getValue() : null)
                                 .lastMeasurement(temp.map(MeasurementsEntity::getTimestamp).orElse(null))
                                 .build();

    }

    private List<MeasurementsEntity> findMeasurementsForSensor(final Long id, final List<MeasurementsEntity> lastMeasurements) {
        return lastMeasurements.stream()
                               .filter(el -> el.getSensorId().equals(id))
                               .toList();
    }

    private Long resolveMeasurementDate(final MeasurementsEntity insolation, final MeasurementsEntity soilHumidity) {
        if(ObjectUtils.isNotEmpty(insolation)) return insolation.getTimestamp();
        if(ObjectUtils.isNotEmpty(soilHumidity)) return soilHumidity.getTimestamp();
        return null;
    }


}
