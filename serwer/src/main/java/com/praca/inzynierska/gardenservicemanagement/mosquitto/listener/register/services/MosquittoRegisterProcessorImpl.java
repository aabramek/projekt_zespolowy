package com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.services;

import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import com.praca.inzynierska.gardenservicemanagement.common.BinaryParser;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.model.MosquittoRegisterRequest;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import java.time.LocalDateTime;

@Slf4j
@Component
public class MosquittoRegisterProcessorImpl implements MosquittoRegisterProcessor {

    private final StationsRepository stationsRepository;

    public MosquittoRegisterProcessorImpl(StationsRepository stationsRepository) {
        this.stationsRepository = stationsRepository;
    }


    @Override
    public boolean registerStation(MosquittoRegisterRequest request) {
        var macAddress = BinaryParser.getMacAddressFromInt64(Long.parseLong(request.getMac()));

        if(stationsRepository.existsByMacAddress(macAddress)) {
             log.error(String.format("Station MAC: %s already exist!", macAddress));
             return false;
        }

        var newStations = StationsEntity.builder()
                                        .name(String.format("Station MAC: %s", macAddress))
                                        .macAddress(macAddress)
                                        .ipAddress(BinaryParser.getIpAddressFromInt32(request.getIp()))
                                        .softwareVersion(String.valueOf(request.getSv()))
                                        .measurementPeriod(1)
                                        .registerDate(LocalDateTime.now())
                                        .build();
        stationsRepository.save(newStations);
        log.info(String.format("Station MAC: %s saved successfully!", macAddress));
        return true;
    }
}
