package com.praca.inzynierska.gardenservicemanagement.webFront.controller;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.api.measurements.MeasurementsApi;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements.MeasurementsDataRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements.MeasurementsDataResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.MeasurementsService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.RestController;

@RestController
@CrossOrigin(origins = "*", maxAge = 3600)
public class MeasurementsController implements MeasurementsApi {

    MeasurementsService measurementsService;

    @Autowired
    public MeasurementsController(MeasurementsService measurementsService) {
        this.measurementsService = measurementsService;
    }

    @Override
    public ResponseEntity<MeasurementsDataResponse> getMeasurementsData(Long id, MeasurementsDataRequest request) {
        return new ResponseEntity<>(measurementsService.getMeasurementsData(id, request), HttpStatus.OK);

    }

}
