package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.time.LocalDateTime;

@Builder
@Data
@AllArgsConstructor
public class AnalogSensorInformation {
    long inputLine;
    boolean isActive;
    Double humidity;
    Double insolation;
    Long lastMeasurement;
}
