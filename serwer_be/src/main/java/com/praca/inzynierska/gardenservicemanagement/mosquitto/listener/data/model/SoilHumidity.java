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
public class SoilHumidity implements Serializable {

    @JsonProperty("input_line")
    int inputLine;

    @JsonProperty("value")
    double value;
}


