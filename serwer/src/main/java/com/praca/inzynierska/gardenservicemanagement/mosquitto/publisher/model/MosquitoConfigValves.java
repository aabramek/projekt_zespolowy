package com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.io.Serializable;

@Data
@Builder
@AllArgsConstructor
public class MosquitoConfigValves implements Serializable {
    int pin; //0-15

    @JsonProperty("operation_mode")
    int operationMode;

    @JsonProperty("enable_high")
    int enableHigh;

    Schedules schedules[];
}
