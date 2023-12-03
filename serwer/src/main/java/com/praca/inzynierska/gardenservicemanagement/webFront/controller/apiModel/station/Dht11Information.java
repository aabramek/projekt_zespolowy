package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;


import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.time.LocalDateTime;

@Builder
@Data
@AllArgsConstructor
public class Dht11Information {

    long inputLine;
    boolean isActive;
    Double temperature;
    Double airHumidity;
    Long lastMeasurement;
}
