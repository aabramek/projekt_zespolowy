package com.praca.inzynierska.gardenservicemanagement.datastore.stations;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.RequiredArgsConstructor;

import java.time.LocalDateTime;

@Entity
@Table(name = "STATIONS")

@Data
@RequiredArgsConstructor
@AllArgsConstructor
@Builder
public class StationsEntity {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "ID")
    private Long id;

    @Column(name = "MAC_ADDRESS", unique = true, nullable = false)
    private String macAddress;

    @Column(name = "NAME")
    private String name;

    @Column(name = "MEASUREMENT_PERIOD")
    private int measurementPeriod;

    @Column(name = "SOFTWARE_VERSION")
    private String softwareVersion;

    @Column(name = "IP_ADDRESS")
    private String ipAddress;

    @Column(name = "REGISTER_DATE")
    private LocalDateTime registerDate;

}
