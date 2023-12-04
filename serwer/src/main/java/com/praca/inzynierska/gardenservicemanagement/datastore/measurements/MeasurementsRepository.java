package com.praca.inzynierska.gardenservicemanagement.datastore.measurements;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

import java.time.LocalDateTime;
import java.util.List;


@Repository
public interface MeasurementsRepository extends JpaRepository<MeasurementsEntity, Long> {


    @Query("SELECT m FROM MeasurementsEntity m " +
            "WHERE m.sensorId IN :ids " +
            "AND m.timestamp = " + "(SELECT max(x.timestamp) FROM MeasurementsEntity x WHERE x.sensorId = m.sensorId)"
    )
    List<MeasurementsEntity> getLastMeasurementsForSensors(@Param("ids") List<Long> ids);

    @Query("SELECT m FROM MeasurementsEntity m " +
            "WHERE m.sensorId IN :sensorIds " +
            "AND m.timestamp >= :endDate " +
            "AND m.timestamp <= :startDate")
    List<MeasurementsEntity> getMeasurementsForSensorsAndBetweenDate(@Param("sensorIds") List<Long> sensorIds,
                                                                     @Param("startDate") Long startDate,
                                                                     @Param("endDate") Long endDate);

}
