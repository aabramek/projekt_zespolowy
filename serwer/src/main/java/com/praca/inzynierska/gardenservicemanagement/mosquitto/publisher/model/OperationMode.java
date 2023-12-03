package com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model;

public enum OperationMode {

    OFF(0, "OFF"),
    ON(1, "ON"),
    AUTO(2, "AUTO");

    OperationMode(int value, String name) {
        this.value = value;
        this.name = name;
    }

    public final int value;
    public final String name;

}
