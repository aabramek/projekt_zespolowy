package com.praca.inzynierska.gardenservicemanagement.datastore.sensors;

import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface SensorsRepository extends JpaRepository<SensorEntity, Long>  {

    List<SensorEntity> findByStationId(Long stationId);

    List<SensorEntity> findByStationIdAndSensorType(Long stationId, SensorType sensorType);
}
