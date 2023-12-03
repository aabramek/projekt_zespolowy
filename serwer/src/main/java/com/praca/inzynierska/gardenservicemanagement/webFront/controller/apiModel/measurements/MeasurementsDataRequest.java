package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements;

import com.praca.inzynierska.gardenservicemanagement.datastore.sensors.SensorType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.time.LocalDateTime;

@Builder
@Data
@AllArgsConstructor
public class MeasurementsDataRequest {
    SensorType sensorType;
    LocalDateTime startDate;
    LocalDateTime endDate;
}
