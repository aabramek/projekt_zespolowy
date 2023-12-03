package com.praca.inzynierska.gardenservicemanagement.webFront.provider;

import com.praca.inzynierska.gardenservicemanagement.datastore.stations.StationsEntity;

import java.util.List;
import java.util.Optional;

public interface StationProvider {
    Optional<StationsEntity> getStationById(Long id);
    List<StationsEntity> getAllStation();
}
