package com.praca.inzynierska.gardenservicemanagement.webFront.updater;

import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesRepository;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.Schedule;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.Valves;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

@Service
public class StationInvokerUpdater implements StationUpdater {

    ValvesRepository valvesRepository;
    SchedulesRepository schedulesRepository;

    @Autowired
    public StationInvokerUpdater(ValvesRepository valvesRepository, SchedulesRepository schedulesRepository) {
        this.valvesRepository = valvesRepository;
        this.schedulesRepository = schedulesRepository;
    }

    
    @Override
    public List<ValvesEntity> toUpdatedValvesList(List<ValvesEntity> valves, List<Valves> updatedValves, Long stationId) {
        var updatedValvesList =  updatedValves.stream().map(it -> updateValves(it, valves, stationId))
                .collect(Collectors.toList());

        return valvesRepository.saveAll(updatedValvesList);
        
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
