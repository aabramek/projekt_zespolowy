package com.praca.inzynierska.gardenservicemanagement.webFront.controller;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.api.station.StationApi;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.*;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.StationService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.RestController;

@RestController
@CrossOrigin(origins = "*", maxAge = 3600)
public class StationController implements StationApi {

    @Autowired
    StationService stationService;

    @Override
    public ResponseEntity<StationListResponse> getStationList() {
        return new ResponseEntity<>(stationService.getStationList(), HttpStatus.OK);
    }

    @Override
    public ResponseEntity<StationSettingsResponse> getStationSettings(Long id) {
        return new ResponseEntity<>(stationService.getStationSettings(id), HttpStatus.OK);
    }

    @Override
    public ResponseEntity<StationSettingsResponse> saveStationSettings(Long id, SaveSettingsRequest request) {
        return new ResponseEntity<>(stationService.saveStationSettings(id, request), HttpStatus.OK);
    }

    @Override
    public ResponseEntity<StationDetailsInformationResponse> getStationInformationDetails(Long id) {
        return new ResponseEntity<>(stationService.getStationInformationDetails(id), HttpStatus.OK);
    }


}
