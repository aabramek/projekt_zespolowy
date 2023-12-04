package com.praca.inzynierska.gardenservicemanagement.webFront.service;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements.MeasurementsDataRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.measurements.MeasurementsDataResponse;

public interface MeasurementsService {
    MeasurementsDataResponse getMeasurementsData(Long id, MeasurementsDataRequest request);
}
