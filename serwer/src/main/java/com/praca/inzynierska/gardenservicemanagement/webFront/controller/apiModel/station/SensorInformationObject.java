package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.util.List;

@Builder
@Data
@AllArgsConstructor
public class SensorInformationObject {
    List<AnalogSensorInformation> analogSensorInformation;
    List<Ds18b20Information> ds18b20InformationList;
    List<Dht11Information> dht11InformationList;
}
