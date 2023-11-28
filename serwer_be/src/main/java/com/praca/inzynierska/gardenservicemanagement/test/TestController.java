package com.praca.inzynierska.gardenservicemanagement.test;

import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesRepository;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.MosquittoPublisherProcessor;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.OperationMode;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class TestController {

    MosquittoPublisherProcessor mosquitoPublisher;
    ValvesRepository valvesRepository;

    @Autowired
    public TestController(MosquittoPublisherProcessor mosquitoPublisher, ValvesRepository valvesRepository) {
        this.mosquitoPublisher = mosquitoPublisher;
        this.valvesRepository = valvesRepository;
    }

    @GetMapping("/")
    public void test() {
        valvesRepository.save(ValvesEntity.builder()
                        .stationId(902L)
                        .pin(1)
                        .enableHigh(true)
                        .operationMode(OperationMode.OFF)
                        .build());

        valvesRepository.save(ValvesEntity.builder()
                .stationId(902L)
                .pin(2)
                .enableHigh(false)
                .operationMode(OperationMode.ON)
                .build());

        valvesRepository.save(ValvesEntity.builder()
                .stationId(902L)
                .pin(3)
                .enableHigh(true)
                .operationMode(OperationMode.AUTO)
                .build());
//        mosquitoPublisher.sendMessageToTopic("register", "test");
//        mosquitoPublisher.sendMessageToTopic("data", "test");
//        mosquitoPublisher.sendMessageToTopic("xxxx", "test");
    }
}
