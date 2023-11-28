package com.praca.inzynierska.gardenservicemanagement.webFront.service.impl;

import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesRepository;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.*;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseException;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseStatus;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.StationService;
import com.praca.inzynierska.gardenservicemanagement.webFront.utils.DefaultValves;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.Comparator;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

@Service
@Slf4j
public class StationServiceImpl implements StationService {

    StationsRepository stationsRepository;
    ValvesRepository valvesRepository;
    SchedulesRepository schedulesRepository;

    @Autowired
    public StationServiceImpl(StationsRepository stationsRepository, ValvesRepository valvesRepository, SchedulesRepository schedulesRepository) {
        this.stationsRepository = stationsRepository;
        this.valvesRepository = valvesRepository;
        this.schedulesRepository = schedulesRepository;
    }


    @Override
    public StationListResponse getStationList() {
        var stationList = stationsRepository.findAll().stream().map(this::toStationElement).toList();
        return StationListResponse.builder().stationListElement(stationList).build();
    }

    @Override
    public StationInformationResponse getStationInformation(Long id) {
        //TODO -> INFO TAKIE STACJA NIE ISTNIEJE + DODAĆ DO API
        var station = stationsRepository.findById(id)
                                        .orElseThrow(() -> new ResponseException("station.not-found", ResponseStatus.NOT_FOUND));

        return StationInformationResponse.builder()
                .id(station.getId())
                .name(station.getName())
                .build();
    }

    @Override
    public StationSettingsResponse getStationSettings(Long id) {
        var station = stationsRepository.findById(id)
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
        var station = stationsRepository.findById(id)
                .orElseThrow(() -> new ResponseException("station.not-found", ResponseStatus.NOT_FOUND));

        var valves = valvesRepository.findAllByStationId(station.getId());
        station.setName(request.getName());
        station.setMeasurementPeriod(request.getMeasurementPeriod());

        var updatedValvesList = toUpdatedValvesList(valves, request.getValvesList(), id);
        valvesRepository.saveAll(updatedValvesList);

        return null;
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

    private List<ValvesEntity> toUpdatedValvesList(List<ValvesEntity> valves, List<Valves> updatedValves, Long stationId) {
        return updatedValves.stream().map(it -> updateValves(it, valves, stationId))
                                     .collect(Collectors.toList());
    }

    private ValvesEntity updateValves(Valves valves, List<ValvesEntity> valvesList, Long stationId) {
        if(valvesList.stream().anyMatch(it -> it.getPin() == valves.getPin())) {
            //update valves
            var valvesToUpdate = valvesList.stream().filter(it -> it.getPin() == valves.getPin()).findFirst().get();
            valvesToUpdate.setOperationMode(valves.getOperationMode());
            valvesToUpdate.setEnableHigh(valves.isEnableHigh());
            var scheduleToDelete = deleteRemovedSchedulers(valvesToUpdate.getSchedulesList(), valves.getSchedulesList());

            var updatedSchedules = updateSchedulerList(valvesToUpdate.getSchedulesList(), valves.getSchedulesList());
            valvesToUpdate.setSchedulesList(updatedSchedules);
            valvesToUpdate.getSchedulesList().forEach(it -> it.setValvesEntity(valvesToUpdate));


            schedulesRepository.deleteAllById(scheduleToDelete);
            return valvesToUpdate;
        } else {
            var schedulerEntityList = toSchedulerEntityList(valves.getSchedulesList());
            var valvesEntity = ValvesEntity.builder()
                                           .pin(valves.getPin())
                                           .stationId(stationId)
                                           .operationMode(valves.getOperationMode())
                                           .enableHigh(valves.isEnableHigh())
                                           .build();

            valvesEntity.setSchedulesList(schedulerEntityList);
            valvesEntity.getSchedulesList().forEach(it -> it.setValvesEntity(valvesEntity));
            return valvesEntity;
        }
    }

    private List<Long> deleteRemovedSchedulers(List<SchedulesEntity> oldSchedulesList, List<Schedule> schedulesList) {
        return oldSchedulesList.stream()
                               .filter(it -> !searchSchedulerExist(it, schedulesList))
                               .map(SchedulesEntity::getId)
                               .collect(Collectors.toList());
    }

    private boolean searchSchedulerExist(SchedulesEntity it, List<Schedule> schedulesList) {
        return schedulesList.stream()
                            .filter(el -> el.getId() != null)
                            .anyMatch(el -> el.getId().equals(it.getId()));
    }

    private List<SchedulesEntity> updateSchedulerList(List<SchedulesEntity> schedulesEntityList, List<Schedule> schedulesList) {
        return schedulesList.stream().map(it -> {
                                        if(it.getId() != null ) {
                                            //UPDATE RECORD
                                            return updateSchedulerElement(it, schedulesEntityList.stream()
                                                                                                 .filter(el -> Objects.equals(el.getId(), it.getId()))
                                                                                                 .findFirst()
                                                                                                 .get());
                                        } else {
                                            return toSchedulerEntity(it);
                                        }})
                                        .collect(Collectors.toList());
    }

    private SchedulesEntity updateSchedulerElement(Schedule it, SchedulesEntity schedulesEntity) {
         schedulesEntity.setHourStart(it.getHourStart());
         schedulesEntity.setMinuteStart(it.getMinuteStart());
         schedulesEntity.setMinuteStop(it.getMinuteStop());
         schedulesEntity.setHourStop(it.getHourStop());
         schedulesEntity.setDayOfWeek(it.getDayOfWeek());
         return schedulesEntity;
    }

    private List<SchedulesEntity> toSchedulerEntityList(List<Schedule> schedulesList) {
        return schedulesList.stream().map(this::toSchedulerEntity).collect(Collectors.toList());
    }

    private SchedulesEntity toSchedulerEntity(Schedule schedule) {
        return SchedulesEntity.builder()
                              .minuteStart(schedule.getMinuteStart())
                              .hourStart(schedule.getHourStart())
                              .minuteStop(schedule.getMinuteStop())
                              .hourStop(schedule.getHourStop())
                              .dayOfWeek(schedule.getDayOfWeek())
                              .build();
    }

}
