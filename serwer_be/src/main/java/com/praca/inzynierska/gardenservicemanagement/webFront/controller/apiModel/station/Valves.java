package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;

import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.OperationMode;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.util.List;

@Builder
@Data
@AllArgsConstructor
public class Valves {
    private int pin;
    private OperationMode operationMode;
    private boolean enableHigh;
    private List<Schedule> schedulesList;

}
