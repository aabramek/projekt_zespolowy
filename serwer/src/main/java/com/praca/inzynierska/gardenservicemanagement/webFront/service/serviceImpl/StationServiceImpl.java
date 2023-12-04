package com.praca.inzynierska.gardenservicemanagement.webFront.service.serviceImpl;

import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesRepository;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.*;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseException;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseStatus;
import com.praca.inzynierska.gardenservicemanagement.webFront.provider.MeasurementsProvider;
import com.praca.inzynierska.gardenservicemanagement.webFront.provider.StationProvider;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.StationService;
import com.praca.inzynierska.gardenservicemanagement.webFront.provider.SensorProvider;
import com.praca.inzynierska.gardenservicemanagement.webFront.provider.ValvesProvider;
import com.praca.inzynierska.gardenservicemanagement.webFront.updater.StationUpdater;
import com.praca.inzynierska.gardenservicemanagement.webFront.utils.DefaultValves;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.sql.Timestamp;
import java.time.LocalDateTime;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

@Service
@Slf4j
public class StationServiceImpl implements StationService {


    ValvesRepository valvesRepository;
    StationUpdater stationUpdater;
    ValvesProvider valvesProvider;
    SensorProvider sensorProvider;
    MeasurementsProvider measurementsProvider;
    StationProvider stationProvider;

    @Autowired
    public StationServiceImpl(ValvesRepository valvesRepository,
                              StationUpdater stationUpdater,
                              ValvesProvider valvesProvider,
                              SensorProvider sensorProvider,
                              MeasurementsProvider measurementsProvider,
                              StationProvider stationProvider) {
        this.valvesRepository = valvesRepository;
        this.stationUpdater = stationUpdater;
        this.valvesProvider = valvesProvider;
        this.sensorProvider = sensorProvider;
        this.measurementsProvider = measurementsProvider;
        this.stationProvider = stationProvider;
    }


    @Override
    public StationListResponse getStationList() {
        var stationList = stationProvider.getAllStation().stream().map(this::toStationElement).toList();
        return StationListResponse.builder().stationListElement(stationList).build();
    }

    @Override
    public StationInformationResponse getStationInformation(Long id) {
        //TODO -> INFO TAKIE STACJA NIE ISTNIEJE + DODAĆ DO API
        var station = stationProvider.getStationById(id)
                                     .orElseThrow(() -> new ResponseException("station.not-found", ResponseStatus.NOT_FOUND));

        return StationInformationResponse.builder()
                .id(station.getId())
                .name(station.getName())
                .build();
    }

    @Override
    public StationSettingsResponse getStationSettings(Long id) {
        var station = stationProvider.getStationById(id)
                                     .orElseThrow(() -> new ResponseException("station.not-found", ResponseStatus.NOT_FOUND));

        var valves = valvesRepository.findAllByStationId(station.getId());
        return StationSettingsResponse.builder()
                                      .id(station.getId())
                                      .name(station.getName())
                                      .macAddress(station.getMacAddress())
                                      .measurementPeriod(station.getMeasurementPeriod())
                                      .valvesList(toValvesList(valves))
                                      .build();
    }

    @Override
    @Transactional
    public StationSettingsResponse saveStationSettings(Long id, SaveSettingsRequest request) {
        var station = stationProvider.getStationById(id)
                                     .orElseThrow(() -> new ResponseException("station.not-found", ResponseStatus.NOT_FOUND));

        var valves = valvesRepository.findAllByStationId(station.getId());
        station.setName(request.getName());
        station.setMeasurementPeriod(request.getMeasurementPeriod());

        //TODO -> DO DOROBIENIA ZWROT I OBSŁUGA?
        var updatedValvesList = stationUpdater.toUpdatedValvesList(valves, request.getValvesList(), id);

        return null;
    }

    @Override
    public StationDetailsInformationResponse getStationInformationDetails(Long id) {
        var station = stationProvider.getStationById(id)
                                     .orElseThrow(() -> new ResponseException("station.not-found", ResponseStatus.NOT_FOUND));

        var valvesEntity = valvesProvider.getValvesInformation(id);
        var valvesList = toValvesList(valvesEntity).stream()
                                                   .map(this::toValvesInformation)
                                                   .toList();

        var sensorsList = sensorProvider.getAllSensorsForStation(station.getId());
        var sensorsInformationObject = measurementsProvider.getLastMeasurementsForSensors(sensorsList);

        return StationDetailsInformationResponse.builder()
                                                .stationName(station.getName())
                                                .addressMac(station.getMacAddress())
                                                .addressIp(station.getIpAddress())
                                                .systemVersion(station.getSoftwareVersion())
                                                .registrationDate(Timestamp.valueOf(station.getRegisterDate()).getTime())
                                                .valvesInformationList(valvesList)
                                                .analogSensorInformationList(sensorsInformationObject.getAnalogSensorInformation())
                                                .ds18b20InformationList(sensorsInformationObject.getDs18b20InformationList())
                                                .dht11InformationList(sensorsInformationObject.getDht11InformationList())
                                                .build();
    }

    private ValvesInformation toValvesInformation(Valves valves) {
        return ValvesInformation.builder()
                                .pin(valves.getPin())
                                .operationMode(valves.getOperationMode())
                                .build();
    }


    private List<Valves> toValvesList(List<ValvesEntity> valves) {
        var mappedValves = valves.stream().map(this::toValves).collect(Collectors.toList());
        for(int i=1;i<17;i++) {
            if(!checkValveExist(mappedValves, i)) {
                mappedValves.add(DefaultValves.defaultValvesForWWW(i));
            }
        }

        return mappedValves.stream()
                           .sorted(Comparator.comparing(Valves::getPin))
                           .collect(Collectors.toList());
    }

    private boolean checkValveExist(List<Valves> mappedValves, int i) {
        return mappedValves.stream().anyMatch(it -> it.getPin() == i);
    }

    private Valves toValves(ValvesEntity valvesEntity) {
        return Valves.builder()
                     .pin(valvesEntity.getPin())
                     .operationMode(valvesEntity.getOperationMode())
                     .enableHigh(valvesEntity.isEnableHigh())
                     .schedulesList(toSchedulesList(valvesEntity.getSchedulesList()))
                     .build();
    }

    private List<Schedule> toSchedulesList(List<SchedulesEntity> schedulesList) {
        return schedulesList.stream().map(this::toSchedules).collect(Collectors.toList());
    }

    private Schedule toSchedules(SchedulesEntity schedulesEntity) {
        return Schedule.builder()
                       .id(schedulesEntity.getId())
                       .dayOfWeek(schedulesEntity.getDayOfWeek())
                       .hourStart(schedulesEntity.getHourStart())
                       .minuteStart(schedulesEntity.getMinuteStart())
                       .hourStop(schedulesEntity.getHourStop())
                       .minuteStop(schedulesEntity.getMinuteStop())
                       .build();
    }

    private StationListElement toStationElement(StationsEntity stationsEntity) {
        return StationListElement.builder()
                                 .id(stationsEntity.getId())
                                 .name(stationsEntity.getName())
                                 .build();

    }






}
