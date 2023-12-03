package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

@Builder
@Data
@AllArgsConstructor
public class SensorData {
    String sensorId;
    Double value;
    Long timestamp;
}
