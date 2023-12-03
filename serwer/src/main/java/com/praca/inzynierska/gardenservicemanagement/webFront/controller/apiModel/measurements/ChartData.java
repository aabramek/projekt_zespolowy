package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements;

import com.praca.inzynierska.gardenservicemanagement.datastore.measurements.MeasurementType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

@Builder
@Data
@AllArgsConstructor
public class ChartData {

    SensorData[] sensorData;
    MeasurementType measurementType;
}
