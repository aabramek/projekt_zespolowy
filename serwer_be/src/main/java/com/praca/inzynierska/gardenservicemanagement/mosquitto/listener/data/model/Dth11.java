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
public class Dth11 implements Serializable {

    @JsonProperty("input_line")
    int inputLine;

    @JsonProperty("value_temp")
    double valueTemp;

    @JsonProperty("value_humidity")
    double valueHumidity;
}
