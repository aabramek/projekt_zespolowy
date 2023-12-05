package com.praca.inzynierska.gardenservicemanagement.integration.web.station;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.praca.inzynierska.gardenservicemanagement.common.StationStub;
import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesRepository;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.StationSettingsResponse;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInstance;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.http.MediaType;
import org.springframework.test.web.servlet.MockMvc;

import static org.hamcrest.Matchers.*;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

@SpringBootTest
@TestInstance(TestInstance.Lifecycle.PER_CLASS)
@AutoConfigureMockMvc
@AutoConfigureTestDatabase
public class SaveStationSettingsTest {

    private final StationsRepository stationsRepository;
    private final ValvesRepository valvesRepository;
    private final SchedulesRepository schedulesRepository;
    private final MockMvc mvc;

    @Autowired
    public SaveStationSettingsTest(StationsRepository stationsRepository, ValvesRepository valvesRepository, SchedulesRepository schedulesRepository, MockMvc mvc) {
        this.stationsRepository = stationsRepository;
        this.valvesRepository = valvesRepository;
        this.schedulesRepository = schedulesRepository;
        this.mvc = mvc;
    }

    @Test
    void shouldNotSaveWhenStationNotExist() throws Exception {
        ObjectMapper objectMapper = new ObjectMapper();
        var savedStation = stationsRepository.save(StationStub.stationsEntityStub());

        mvc.perform(post(String.format("/api/station/settings/%d/save",savedStation.getId()+1))
                        .contentType(MediaType.APPLICATION_JSON)
                        .content(objectMapper.writeValueAsString(new StationSettingsResponse())))
                        .andExpect(status().isNotFound());
        
    }
}
