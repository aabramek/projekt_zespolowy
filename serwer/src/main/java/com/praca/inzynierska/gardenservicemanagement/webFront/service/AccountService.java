package com.praca.inzynierska.gardenservicemanagement.webFront.service;

import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.account.ChangePasswordRequest;

public interface AccountService {
    void changePassword(ChangePasswordRequest request);
}
