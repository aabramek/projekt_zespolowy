package com.praca.inzynierska.gardenservicemanagement.webFront.service;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.LoginRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.LoginResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.RegisterRequest;

public interface UserService {
    LoginResponse login(LoginRequest user);
    void create(RegisterRequest user);
}
