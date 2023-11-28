package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth;


import lombok.Data;

@Data
public class LoginRequest {
    private String userName;
    private String password;
}
