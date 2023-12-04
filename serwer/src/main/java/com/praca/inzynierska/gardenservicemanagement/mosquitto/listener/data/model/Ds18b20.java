package com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.data.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.io.Serializable;

@Data
@Builder
@AllArgsConstructor
@NoArgsConstructor
public class Ds18b20 implements Serializable {

    @JsonProperty("rom_code")
    int romCode;

    @JsonProperty("value")
    double value;
}
