package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;


@Builder
@Data
@AllArgsConstructor
public class Ds18b20Information {
    String address;
    boolean isActive;
    Double temperature;
    Long lastMeasurement;
}
