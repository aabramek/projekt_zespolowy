package com.praca.inzynierska.gardenservicemanagement.datastore.measurements;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.RequiredArgsConstructor;

@Entity
@Table(name = "MEASUREMENTS")

@Data
@RequiredArgsConstructor
@AllArgsConstructor
@Builder

public class MeasurementsEntity {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "ID")
    private Long id;

    @Column(name = "ID_STATION", nullable = false)
    private Long idStation;

    @Column(name = "TYPE")
    private String type;

    @Column(name = "VALUE")
    private double value;

    @Column(name = "TIMESTAMP")
    private Long timestamp;

}
