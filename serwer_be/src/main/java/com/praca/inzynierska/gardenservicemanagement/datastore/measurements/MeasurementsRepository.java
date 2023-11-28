package com.praca.inzynierska.gardenservicemanagement.datastore.measurements;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;


@Repository
public interface MeasurementsRepository extends JpaRepository<MeasurementsEntity, Long> {

}
