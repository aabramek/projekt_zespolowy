package com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.io.Serializable;

@Data
@Builder
@AllArgsConstructor
public class DeviceConfigurationRequest  implements Serializable {
    int measurementPeriod;

    @JsonProperty("valves")
    MosquitoConfigValves valves[];
}

