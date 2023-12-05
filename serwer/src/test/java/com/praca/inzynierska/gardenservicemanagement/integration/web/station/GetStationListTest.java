package com.praca.inzynierska.gardenservicemanagement.integration.web.station;

import com.praca.inzynierska.gardenservicemanagement.common.StationStub;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import org.junit.jupiter.api.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.http.MediaType;
import org.springframework.test.web.servlet.MockMvc;

import static org.hamcrest.Matchers.*;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

@SpringBootTest
@TestInstance(TestInstance.Lifecycle.PER_CLASS)
@AutoConfigureMockMvc
@AutoConfigureTestDatabase
public class GetStationListTest {

    private final StationsRepository stationsRepository;
    private final MockMvc mvc;

    @Autowired
    GetStationListTest(StationsRepository stationsRepository, MockMvc mvc) {
        this.stationsRepository = stationsRepository;
        this.mvc = mvc;
    }

    @AfterAll
    void clean() {
        stationsRepository.deleteAll();
    }

    @BeforeEach
    void setUp() {
        stationsRepository.deleteAll();
    }

    @Test
    void shouldReturnAllStationsWhenExist() throws Exception {
        stationsRepository.save(StationStub.stationsEntityStub());
        stationsRepository.save(StationStub.stationsEntityStub());
        stationsRepository.save(StationStub.stationsEntityStub());

        mvc.perform(get("/api/station/get/list")
                        .contentType(MediaType.APPLICATION_JSON))
                        .andExpect(status().isOk())
                        .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                        .andExpect(jsonPath("$.stationListElement",hasSize(equalTo(3))));

    }

    @Test
    void shouldReturnEmptyListWhenStationNotExist() throws Exception {
        mvc.perform(get("/api/station/get/list")
                        .contentType(MediaType.APPLICATION_JSON))
                .andExpect(status().isOk())
                .andExpect(content().contentTypeCompatibleWith(MediaType.APPLICATION_JSON))
                .andExpect(jsonPath("$.stationListElement",hasSize(greaterThanOrEqualTo(0))));

    }


}
