package com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception;

import lombok.*;

import java.util.ArrayList;
import java.util.List;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Builder
public class DefaultError {
    private String code;
    List<String> errors = new ArrayList<>();
}
