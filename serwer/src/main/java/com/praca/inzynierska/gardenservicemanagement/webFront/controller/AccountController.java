package com.praca.inzynierska.gardenservicemanagement.webFront.controller;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.api.account.AccountApi;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.account.ChangePasswordRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.AccountService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.RestController;

@RestController
@CrossOrigin(origins = "*", maxAge = 3600)
public class AccountController implements AccountApi {

    @Autowired
    AccountService accountService;

    @Override
    public void changeUserPassword(ChangePasswordRequest request) {
        accountService.changePassword(request);
    }

}
