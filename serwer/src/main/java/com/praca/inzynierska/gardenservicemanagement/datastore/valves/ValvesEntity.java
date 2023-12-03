package com.praca.inzynierska.gardenservicemanagement.datastore.valves;


import com.praca.inzynierska.gardenservicemanagement.datastore.schedules.SchedulesEntity;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.OperationMode;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;

@Entity
@AllArgsConstructor
@NoArgsConstructor
@Data
@Builder
@Table(name = "VALVES")
public class ValvesEntity {

    @Id
    @GeneratedValue
    private Long id;

    @Column(name = "STATION_ID")
    private Long stationId;

    @Column(name = "PIN")
    private int pin;

    @Column(name = "OPERATION_MODE")
    @Enumerated(EnumType.STRING)
    private OperationMode operationMode;

    @Column(name = "ENABLE_HIGH")
    private boolean enableHigh;

    @OneToMany(mappedBy = "valvesEntity",cascade = CascadeType.ALL, fetch = FetchType.EAGER)
    private List<SchedulesEntity> schedulesList;
}
