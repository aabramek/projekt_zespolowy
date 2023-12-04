package com.praca.inzynierska.gardenservicemanagement.webFront.controller.api.auth;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.LoginRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.LoginResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.RegisterRequest;
import io.swagger.v3.oas.annotations.responses.ApiResponse;
import io.swagger.v3.oas.annotations.responses.ApiResponses;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;

import static com.praca.inzynierska.gardenservicemanagement.webFront.utils.Constraint.APP_ROOT;


public interface AuthApi {

    @ApiResponses(value = {
            @ApiResponse(responseCode = "401", description = "auth.missing-information: Missing login information"),
            @ApiResponse(responseCode = "403", description = "auth.authorization-error: Wrong login information")})
    @PostMapping(value = APP_ROOT + "/auth/login", consumes = MediaType.APPLICATION_JSON_VALUE, produces = MediaType.APPLICATION_JSON_VALUE)
    ResponseEntity<LoginResponse> loginUser(@RequestBody LoginRequest request);

    @PostMapping(value = APP_ROOT + "/auth/register", consumes = MediaType.APPLICATION_JSON_VALUE, produces = MediaType.APPLICATION_JSON_VALUE)
    ResponseEntity<Void> createUser(@RequestBody RegisterRequest request);
}
