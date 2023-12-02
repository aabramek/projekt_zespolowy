package com.praca.inzynierska.gardenservicemanagement.datastore.sensors;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.RequiredArgsConstructor;

@Entity
@Table(name = "SENSORS")

@Data
@RequiredArgsConstructor
@AllArgsConstructor
@Builder
public class SensorEntity {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "ID")
    private Long id;

    @Column(name = "SENSOR_TYPE")
    @Enumerated(EnumType.STRING)
    private SensorType sensorType;

    @Column(name = "ADDRESS")
    private Long address;

    @Column(name = "ACTIVE")
    private boolean isActive;

    @Column(name = "STATION_ID")
    private Long stationId;
}
