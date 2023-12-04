package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;

import lombok.Builder;
import lombok.Data;

@Data
@Builder
public class StationInformationResponse {
    private Long id;
    private String name;
}
