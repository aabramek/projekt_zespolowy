package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.station;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

@Builder
@Data
@AllArgsConstructor
public class Schedule {

    private Long id;
    private Long minuteStart;
    private Long hourStart;
    private Long minuteStop;
    private Long hourStop;
    private Long dayOfWeek;

}
