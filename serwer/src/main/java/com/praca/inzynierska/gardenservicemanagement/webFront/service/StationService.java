package com.praca.inzynierska.gardenservicemanagement.webFront.service;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.*;

public interface StationService {
    StationListResponse getStationList();

    StationInformationResponse getStationInformation(Long id);

    StationSettingsResponse getStationSettings(Long id);

    StationSettingsResponse saveStationSettings(Long id, SaveSettingsRequest request);

    StationDetailsInformationResponse getStationInformationDetails(Long id);
}
