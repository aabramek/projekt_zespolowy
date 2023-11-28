package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;

import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.util.List;

@Builder
@Data
@AllArgsConstructor
public class SaveSettingsRequest {

    private String name;
    private String macAddress;
    private int measurementPeriod;

    @JsonProperty("valvesList")
    private List<Valves> valvesList;
}
