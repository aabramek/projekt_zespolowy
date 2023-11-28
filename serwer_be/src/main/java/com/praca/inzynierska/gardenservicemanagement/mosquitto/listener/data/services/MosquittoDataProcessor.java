package com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.data.services;

import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.data.model.MosquittoDataRequest;

public interface MosquittoDataProcessor {
    void saveMeasurementData(MosquittoDataRequest request);
}
