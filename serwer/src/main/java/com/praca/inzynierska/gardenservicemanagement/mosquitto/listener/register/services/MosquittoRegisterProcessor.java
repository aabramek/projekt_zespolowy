package com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.services;

import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.model.MosquittoRegisterRequest;

public interface MosquittoRegisterProcessor {
    boolean registerStation(MosquittoRegisterRequest request);
}
