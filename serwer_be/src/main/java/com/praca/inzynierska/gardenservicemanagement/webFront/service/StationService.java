package com.praca.inzynierska.gardenservicemanagement.webFront.service;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.SaveSettingsRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.StationInformationResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.StationListResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.StationSettingsResponse;

public interface StationService {
    StationListResponse getStationList();

    StationInformationResponse getStationInformation(Long id);

    StationSettingsResponse getStationSettings(Long id);

    StationSettingsResponse saveStationSettings(Long id, SaveSettingsRequest request);
}
