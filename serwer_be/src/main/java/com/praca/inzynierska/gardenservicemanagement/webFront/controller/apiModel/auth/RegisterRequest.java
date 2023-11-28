package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth;

import lombok.Data;

@Data
public class RegisterRequest {
    private String userName;
    private String password;
}
