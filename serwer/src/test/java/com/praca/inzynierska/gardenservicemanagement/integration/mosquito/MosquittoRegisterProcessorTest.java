package com.praca.inzynierska.gardenservicemanagement.integration.mosquito;

import com.praca.inzynierska.gardenservicemanagement.common.BinaryParser;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.model.MosquittoRegisterRequest;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.services.MosquittoRegisterProcessor;
import org.junit.jupiter.api.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;

@SpringBootTest
@TestInstance(TestInstance.Lifecycle.PER_CLASS)
@AutoConfigureMockMvc
@AutoConfigureTestDatabase
public class MosquittoRegisterProcessorTest {

    private final Integer DEFAULT_MEASUREMENTS_VALUE = 1;
    private final StationsRepository stationsRepository;
    private final MosquittoRegisterProcessor mosquittoRegisterProcessor;

    @Autowired
    MosquittoRegisterProcessorTest(StationsRepository stationsRepository, MosquittoRegisterProcessor mosquittoRegisterProcessor) {
        this.stationsRepository = stationsRepository;
        this.mosquittoRegisterProcessor = mosquittoRegisterProcessor;
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
    void shouldRegisterNewStation() {
        //GIVEN
        var request = createRequest();

        //WHEN
        var result = mosquittoRegisterProcessor.registerStation(request);
        var station = stationsRepository.findAll().get(0);

        //THEN
        Assertions.assertEquals(stationsRepository.findAll().size(), 1);
        Assertions.assertTrue(result);
        Assertions.assertEquals(station.getIpAddress(), BinaryParser.getIpAddressFromInt32(request.getIp()));
        Assertions.assertEquals(station.getMacAddress(), BinaryParser.getMacAddressFromInt64(Long.parseLong(request.getMac())));
        Assertions.assertEquals(station.getMeasurementPeriod(), DEFAULT_MEASUREMENTS_VALUE);
    }

    @Test
    void shouldNotRegisterNewStationWhenExist() {
        //GIVEN
        var request = createRequest();
        //WHEN
        var result = mosquittoRegisterProcessor.registerStation(request);
        var result2 = mosquittoRegisterProcessor.registerStation(request);

        //THEN
        Assertions.assertEquals(stationsRepository.findAll().size(), DEFAULT_MEASUREMENTS_VALUE);
        Assertions.assertTrue(result);
        Assertions.assertFalse(result2);
    }

    private MosquittoRegisterRequest createRequest() {
        var request = new MosquittoRegisterRequest();
        request.setIp(285255872L);
        request.setMac("3477672765554641344");
        request.setSv(0);
        return request;
    }

}
