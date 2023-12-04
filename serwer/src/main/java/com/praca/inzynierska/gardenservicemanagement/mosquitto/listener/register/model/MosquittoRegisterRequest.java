package com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.model;

import lombok.Data;

import java.io.Serializable;

@Data
public class MosquittoRegisterRequest implements Serializable {
    Long ip;
    String mac;
    int sv;
}
