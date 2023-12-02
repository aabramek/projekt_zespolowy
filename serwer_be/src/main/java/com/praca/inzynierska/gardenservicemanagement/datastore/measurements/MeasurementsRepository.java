package com.praca.inzynierska.gardenservicemanagement.datastore.measurements;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import java.util.List;


@Repository
public interface MeasurementsRepository extends JpaRepository<MeasurementsEntity, Long> {


    @Query("SELECT m FROM MeasurementsEntity m " +
            "WHERE m.sensorId IN :ids " +
                  "AND m.timestamp = " +
            "(SELECT max(x.timestamp) FROM MeasurementsEntity x WHERE x.sensorId = m.sensorId)"
    )
    List<MeasurementsEntity> getLastMeasurementsForSensors(List<Long> ids);
}
