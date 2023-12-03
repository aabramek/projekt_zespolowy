package com.praca.inzynierska.gardenservicemanagement.webFront.provider;

import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class StationInvokerProvider implements StationProvider {

    StationsRepository stationsRepository;

    @Autowired
    public StationInvokerProvider(StationsRepository stationsRepository) {
        this.stationsRepository = stationsRepository;
    }

    @Override
    public Optional<StationsEntity> getStationById(Long id) {
        return stationsRepository.findById(id);
    }

    @Override
    public List<StationsEntity> getAllStation() {
        return stationsRepository.findAll();
    }
}
