package com.praca.inzynierska.gardenservicemanagement.integration.station;

import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.EnableAutoConfiguration;
import org.springframework.boot.autoconfigure.security.servlet.SecurityAutoConfiguration;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;


@SpringBootTest
//@AutoConfigureMockMvc
//@EnableAutoConfiguration(exclude = SecurityAutoConfiguration.class)
public class GetAllStationTest {

    private final StationsRepository stationsRepository;

    @Autowired
    GetAllStationTest(StationsRepository stationsRepository) {
        this.stationsRepository = stationsRepository;
    }

    @Test
    void shouldDoNothing() {
        var x = stationsRepository.findAll();

        Assertions.assertEquals(0, x.size());
        Assertions.assertEquals(1, 1);
        Assertions.assertEquals(1L, 1L);
    }


}
