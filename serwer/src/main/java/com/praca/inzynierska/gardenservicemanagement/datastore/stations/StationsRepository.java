package com.praca.inzynierska.gardenservicemanagement.datastore.stations;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;


@Repository
public interface StationsRepository extends JpaRepository<StationsEntity, Long> {
    boolean existsByMacAddress(String macAddress);
    Optional<StationsEntity> findByMacAddress(String macAddress);
}
