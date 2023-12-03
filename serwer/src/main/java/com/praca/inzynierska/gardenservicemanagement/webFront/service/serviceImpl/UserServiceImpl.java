package com.praca.inzynierska.gardenservicemanagement.webFront.service.serviceImpl;

import com.praca.inzynierska.gardenservicemanagement.datastore.user.UserEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.user.UserRepository;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.LoginRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.LoginResponse;
import com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.auth.RegisterRequest;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseException;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseStatus;
import com.praca.inzynierska.gardenservicemanagement.webFront.service.UserService;
import com.praca.inzynierska.gardenservicemanagement.webFront.validators.UserValidator;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
@Slf4j
public class UserServiceImpl implements UserService {

    @Autowired
    private UserRepository userRepository;

    @Override
    public LoginResponse login(LoginRequest request) {
        List<String> errors = UserValidator.validateUserCredentials(request.getUserName(), request.getPassword());
        if (!errors.isEmpty()) {
            log.error("Wrogn credensial! - {}", errors);
            throw new ResponseException("auth.missing-information", ResponseStatus.UNAUTHORIZED, errors);
        }

        return userRepository.findUserByUserNameAndPassword(request.getUserName(), request.getPassword())
                .map(it -> new LoginResponse(it.getUserName()))
                .orElseThrow(() -> new ResponseException("auth.authorization-error", ResponseStatus.USER_NOT_FOUND));

    }

    @Override
    public void create(RegisterRequest request) {
        userRepository.save(new UserEntity(null, request.getUserName(), request.getPassword()));
    }
}
