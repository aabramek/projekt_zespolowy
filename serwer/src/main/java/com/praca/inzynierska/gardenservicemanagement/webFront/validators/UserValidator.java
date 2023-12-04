package com.praca.inzynierska.gardenservicemanagement.webFront.validators;

import org.springframework.util.StringUtils;

import java.util.ArrayList;
import java.util.List;

public class UserValidator {
    public static List<String> validateUserCredentials(String email, String password) {
        List<String> errors = new ArrayList<>();
        if (StringUtils.isEmpty(email)) {
            errors.add("User name  is empty");
        }
        if (StringUtils.isEmpty(password)) {
            errors.add("Password is empty");
        }
        return errors;
    }

}
