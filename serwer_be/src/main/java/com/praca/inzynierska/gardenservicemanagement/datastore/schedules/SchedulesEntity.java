package com.praca.inzynierska.gardenservicemanagement.datastore.schedules;

import com.fasterxml.jackson.annotation.JsonBackReference;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesEntity;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.RequiredArgsConstructor;

@Entity
@Table(name = "SCHEDULES")

@Data
@RequiredArgsConstructor
@AllArgsConstructor
@Builder
public class SchedulesEntity {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "ID")
    private Long id;

    @Column(name = "MINUTE_START")
    private Long minuteStart;

    @Column(name = "HOUR_START")
    private Long hourStart;

    @Column(name = "MINUTE_STOP")
    private Long minuteStop;

    @Column(name = "HOUR_STOP")
    private Long hourStop;

    @Column(name = "DAY_OF_WEEK")
    private Long dayOfWeek;

    @ManyToOne(cascade = {CascadeType.PERSIST, CascadeType.MERGE}, fetch = FetchType.EAGER)
    @JoinColumn(name = "VALVES_ID", referencedColumnName = "id")
    @JsonBackReference
    private ValvesEntity valvesEntity;

    @Override
    public String toString() {
        return "SchedulesEntity{" +
                "id=" + id +
                ", minuteStart=" + minuteStart +
                ", hourStart=" + hourStart +
                ", minuteStop=" + minuteStop +
                ", hourStop=" + hourStop +
                ", dayOfWeek=" + dayOfWeek +
                '}';
    }
}
