package com.praca.inzynierska.gardenservicemanagement.webFront.controller.api.measurements;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements.MeasurementsDataRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements.MeasurementsDataResponse;
import io.swagger.v3.oas.annotations.responses.ApiResponse;
import io.swagger.v3.oas.annotations.responses.ApiResponses;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;

import static com.praca.inzynierska.gardenservicemanagement.webFront.utils.Constraint.APP_ROOT;

public interface MeasurementsApi {
    @ApiResponses(value = {
            @ApiResponse(responseCode = "404", description = "station.not-found: Station not fond")})
    @PostMapping(value = APP_ROOT + "/measurements/data/{id}", consumes = MediaType.APPLICATION_JSON_VALUE, produces = MediaType.APPLICATION_JSON_VALUE)
    ResponseEntity<MeasurementsDataResponse> getMeasurementsData(@PathVariable Long id, @RequestBody MeasurementsDataRequest request);


}
