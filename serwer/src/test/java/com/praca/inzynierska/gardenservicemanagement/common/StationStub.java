package com.praca.inzynierska.gardenservicemanagement.common;

import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsEntity;

import java.time.LocalDateTime;
import java.util.UUID;

public class StationStub {

//    static

    public static StationsEntity stationsEntityStub() {
        var entity = new StationsEntity();
        var randomMac = UUID.randomUUID().toString();

        entity.setMacAddress(randomMac);
        entity.setName("Station " + randomMac);
        entity.setIpAddress("192.168.0.0");
        entity.setMeasurementPeriod(1);
        entity.setSoftwareVersion("1.0.0");
        entity.setRegisterDate(LocalDateTime.now());
        return entity;
    }

}
