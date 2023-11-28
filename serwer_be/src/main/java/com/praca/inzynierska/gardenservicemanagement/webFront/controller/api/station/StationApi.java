package com.praca.inzynierska.gardenservicemanagement.webFront.controller.api.station;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.SaveSettingsRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.StationInformationResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.StationListResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.StationSettingsResponse;
import io.swagger.v3.oas.annotations.responses.ApiResponse;
import io.swagger.v3.oas.annotations.responses.ApiResponses;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;

import static com.praca.inzynierska.gardenservicemanagement.webFront.utils.Constraint.APP_ROOT;

public interface StationApi {

    @GetMapping(value = APP_ROOT + "/station/get/list", produces = MediaType.APPLICATION_JSON_VALUE)
    ResponseEntity<StationListResponse> getStationList();

    @ApiResponses(value = {
            @ApiResponse(responseCode = "404", description = "station.not-found: Station not find")})
    @GetMapping(value = APP_ROOT + "/station/get/{id}", produces = MediaType.APPLICATION_JSON_VALUE)
    ResponseEntity<StationInformationResponse> getStation(@PathVariable Long id);

    @ApiResponses(value = {
            @ApiResponse(responseCode = "404", description = "station.not-found: Station not find")})
    @GetMapping(value = APP_ROOT + "/station/settings/get/{id}", produces = MediaType.APPLICATION_JSON_VALUE)
    ResponseEntity<StationSettingsResponse> getStationSettings(@PathVariable Long id);

    @ApiResponses(value = {
            @ApiResponse(responseCode = "404", description = "station.not-found: Station not find")})
    @PostMapping(value = APP_ROOT + "/station/settings/get/{id}/save", consumes = MediaType.APPLICATION_JSON_VALUE, produces = MediaType.APPLICATION_JSON_VALUE)
    ResponseEntity<StationSettingsResponse> saveStationSettings(@PathVariable Long id, @RequestBody SaveSettingsRequest request);

}
