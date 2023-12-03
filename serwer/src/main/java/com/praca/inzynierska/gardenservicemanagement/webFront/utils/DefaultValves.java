package com.praca.inzynierska.gardenservicemanagement.webFront.utils;

import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.OperationMode;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.MosquitoConfigValves;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station.Valves;
import org.springframework.beans.factory.annotation.Value;

import java.util.Collections;


public class DefaultValves {

    @Value("${app.valves.default.operationMode}")
    private static String operationMode;

    @Value("${app.valves.default.enableHigh}")
    private static int enableHigh;


    //TODO -> ZROBIENIE TEGO POLA NA PARAMETRZE
//    OperationMode.valueOf(operationMode).value
    public static MosquitoConfigValves defaultConfiguration(final int pin) {
        return new MosquitoConfigValves(pin, OperationMode.OFF.value, enableHigh, null);
    }

//    OperationMode.valueOf(operationMode)
    public static Valves defaultValvesForWWW(final int pin) {
        return new Valves(pin, OperationMode.OFF, enableHigh != 0, Collections.emptyList());
    }

}
