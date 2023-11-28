package com.praca.inzynierska.gardenservicemanagement.webFront.service.impl;

import com.praca.inzynierska.gardenservicemanagement.datastore.user.UserRepository;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.account.ChangePasswordRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseException;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseStatus;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.AccountService;
import com.praca.inzynierska.gardenservicemanagement.webFront.validators.AccountValidator;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

@Service
@Slf4j
public class AccountServiceImpl implements AccountService {

    private final String USER_NOT_EXIST = "account.user-not-exist";

    private final UserRepository userRepository;

    public AccountServiceImpl(UserRepository userRepository) {
        this.userRepository = userRepository;
    }

    @Override
    public void changePassword(final ChangePasswordRequest request) {
        var userEntity = userRepository.findById(request.getUserId()).orElseThrow(
                () -> new ResponseException(USER_NOT_EXIST, ResponseStatus.BAD_REQUEST)
        );
        AccountValidator.validateChangePassword(request.getOldPassword(), request.getNewPassword(), userEntity.getPassword());
        userEntity.setPassword(request.getNewPassword());
        userRepository.save(userEntity);
    }
}
