package com.praca.inzynierska.gardenservicemanagement.datastore.valves;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface ValvesRepository extends JpaRepository<ValvesEntity, Long> {

    List<ValvesEntity> findAllByStationId(Long stationId);
}
