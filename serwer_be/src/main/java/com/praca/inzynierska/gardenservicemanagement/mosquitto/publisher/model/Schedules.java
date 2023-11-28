package com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.io.Serializable;

@Data
@Builder
@AllArgsConstructor
public class Schedules implements Serializable {
    int minuteOn;
    int minuteOff;
    int hourOn;
    int hourOf;
    int dayOfWeek;
}

