package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;



import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.time.LocalDateTime;
import java.util.List;

@Builder
@Data
@AllArgsConstructor
public class StationDetailsInformationResponse {
    String stationName;
    String addressMac;
    String addressIp;
    String systemVersion;
    Long registrationDate;
    List<AnalogSensorInformation> analogSensorInformationList;
    List<Ds18b20Information> ds18b20InformationList;
    List<Dht11Information> dht11InformationList;
    List<ValvesInformation> valvesInformationList;
}
