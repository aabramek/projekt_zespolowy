package com.praca.inzynierska.gardenservicemanagement.webFront.controller.api.account;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.account.ChangePasswordRequest;
import io.swagger.v3.oas.annotations.responses.ApiResponse;
import io.swagger.v3.oas.annotations.responses.ApiResponses;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;

import static com.praca.inzynierska.gardenservicemanagement.webFront.utils.Constraint.APP_ROOT;

public interface AccountApi {

    @ApiResponses(value = {
            @ApiResponse(responseCode = "400", description = "account.missing-information: Missing change password information"),
            @ApiResponse(responseCode = "400", description = "account.user-not-exist: User not exist"),
            @ApiResponse(responseCode = "400", description = "account.password-to-weak: New password to weak"),
            @ApiResponse(responseCode = "400", description = "account.password-already-used: New and old password should be different"),
            @ApiResponse(responseCode = "400", description = "account.wrong-old-password: Wrong old password")})
    @PostMapping(value = APP_ROOT + "/account/change/password", consumes = MediaType.APPLICATION_JSON_VALUE)
    void changeUserPassword(@RequestBody ChangePasswordRequest request);


}
