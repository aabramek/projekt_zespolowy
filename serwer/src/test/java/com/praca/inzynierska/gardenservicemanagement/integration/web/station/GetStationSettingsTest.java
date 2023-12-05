package com.praca.inzynierska.gardenservicemanagement.integration.web.station;

import com.praca.inzynierska.gardenservicemanagement.common.StationStub;
import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesRepository;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.OperationMode;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInstance;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.http.MediaType;
import org.springframework.test.web.servlet.MockMvc;

import java.util.List;

import static org.hamcrest.Matchers.*;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

@SpringBootTest
@TestInstance(TestInstance.Lifecycle.PER_CLASS)
@AutoConfigureMockMvc
@AutoConfigureTestDatabase
public class GetStationSettingsTest {

    private final StationsRepository stationsRepository;
    private final ValvesRepository valvesRepository;
    private final SchedulesRepository schedulesRepository;
    private final MockMvc mvc;

    @Autowired
    GetStationSettingsTest(StationsRepository stationsRepository, MockMvc mvc, ValvesRepository valvesRepository, SchedulesRepository schedulesRepository) {
        this.stationsRepository = stationsRepository;
        this.mvc = mvc;
        this.valvesRepository = valvesRepository;
        this.schedulesRepository = schedulesRepository;
    }

    @AfterAll
    void clean() {
        stationsRepository.deleteAll();
        schedulesRepository.deleteAll();
        valvesRepository.deleteAll();
    }

    @BeforeEach
    void setUp() {
        stationsRepository.deleteAll();
        schedulesRepository.deleteAll();
        valvesRepository.deleteAll();
    }

    @Test
    void shouldReturnAllStationsWhenExist() throws Exception {
        //GIVEN
        var savedStation = stationsRepository.save(StationStub.stationsEntityStub());
        prepareDatabaseData(savedStation.getId());

        //THEN
        mvc.perform(get(String.format("/api/station/settings/get/%d", savedStation.getId()))
                        .contentType(MediaType.APPLICATION_JSON))
                        .andExpect(status().isOk())
                        .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                        .andExpect(jsonPath("$.valvesList", hasSize(greaterThanOrEqualTo(3))))
                        .andExpect(jsonPath("$.macAddress",equalTo(savedStation.getMacAddress())))
                        .andExpect(jsonPath("$.measurementPeriod",equalTo(savedStation.getMeasurementPeriod())))
                        .andExpect(jsonPath("$.valvesList[0].schedulesList",hasSize(equalTo(2))))
                        .andExpect(jsonPath("$.valvesList[0].pin",equalTo(0)))
                        .andExpect(jsonPath("$.valvesList[0].enableHigh",equalTo(false)))
                        .andExpect(jsonPath("$.valvesList[0].operationMode",equalTo("OFF")))
                        .andExpect(jsonPath("$.valvesList[1].schedulesList",hasSize(equalTo(1))))
                        .andExpect(jsonPath("$.valvesList[1].pin",equalTo(1)))
                        .andExpect(jsonPath("$.valvesList[1].enableHigh",equalTo(true)))
                        .andExpect(jsonPath("$.valvesList[1].operationMode",equalTo("AUTO")))
                        .andExpect(jsonPath("$.valvesList[2].schedulesList",hasSize(equalTo(0))))
                        .andExpect(jsonPath("$.valvesList[2].enableHigh",equalTo(true)))
                        .andExpect(jsonPath("$.valvesList[2].operationMode",equalTo("ON")))
                        .andExpect(jsonPath("$.valvesList[2].pin",equalTo(2)));

    }

    @Test
    void shouldReturn404codeWhenStationNotExist() throws Exception {
        mvc.perform(get(String.format("/api/station/settings/get/%d", 999))
                        .contentType(MediaType.APPLICATION_JSON))
                        .andExpect(status().isNotFound());
    }

    private void prepareDatabaseData(Long stationId) {

        var valves1 = ValvesEntity.builder()
                                .stationId(stationId)
                                .pin(0)
                                .operationMode(OperationMode.OFF)
                                .enableHigh(false)
                                .build();

        var valves2 = ValvesEntity.builder()
                                  .stationId(stationId)
                                  .pin(1)
                                  .operationMode(OperationMode.AUTO)
                                  .enableHigh(true)
                                  .build();

        var valves3 = ValvesEntity.builder()
                                  .stationId(stationId)
                                  .pin(2)
                                  .operationMode(OperationMode.ON)
                                  .enableHigh(true)
                                  .build();

        var schedule1 = SchedulesEntity.builder()
                                        .dayOfWeek(1L)
                                        .hourStop(3L)
                                        .hourStart(2L)
                                        .minuteStop(30L)
                                        .minuteStop(30L)
                                        .valvesEntity(valves1)
                                        .build();

        var schedule2 = SchedulesEntity.builder()
                                        .dayOfWeek(2L)
                                        .hourStop(3L)
                                        .hourStart(2L)
                                        .minuteStop(30L)
                                        .minuteStop(30L)
                                        .valvesEntity(valves1)
                                        .build();

        var schedule3 = SchedulesEntity.builder()
                                        .dayOfWeek(2L)
                                        .hourStop(3L)
                                        .hourStart(2L)
                                        .minuteStop(30L)
                                        .minuteStop(30L)
                                        .valvesEntity(valves2)
                                        .build();

        valves1.setSchedulesList(List.of(schedule1, schedule2));
        valves2.setSchedulesList(List.of(schedule3));
        valvesRepository.saveAll(List.of(valves1, valves2 ,valves3));
    }

}
