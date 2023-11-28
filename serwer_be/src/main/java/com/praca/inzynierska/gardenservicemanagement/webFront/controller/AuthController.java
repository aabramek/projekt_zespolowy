package com.praca.inzynierska.gardenservicemanagement.webFront.controller;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.api.auth.AuthApi;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.LoginRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.LoginResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.RegisterRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.RestController;

@RestController
@CrossOrigin(origins = "*", maxAge = 3600)
public class AuthController implements AuthApi {

    @Autowired
    UserService userService;

    @Override
    public ResponseEntity<LoginResponse> loginUser(LoginRequest request) {
        return new ResponseEntity<>(userService.login(request), HttpStatus.OK);
    }

    @Override
    public ResponseEntity<Void> createUser(RegisterRequest request) {
        userService.create(request);
        return ResponseEntity.ok().body(null);
    }

}
