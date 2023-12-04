package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;

import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.OperationMode;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

@Builder
@Data
@AllArgsConstructor
public class ValvesInformation {
    private int pin;
    private OperationMode operationMode;

}
