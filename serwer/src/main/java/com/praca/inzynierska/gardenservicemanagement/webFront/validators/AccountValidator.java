package com.praca.inzynierska.gardenservicemanagement.webFront.validators;

import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseException;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseStatus;
import org.springframework.util.StringUtils;



public class AccountValidator {

    private static final String MISSING_INFORMATION = "account.missing-information";
    private static final String WRONG_OLD_PASSWORD = "account.wrong-old-password";
    private static final String PASSWORD_TO_WEAK = "account.password-to-weak";
    private static final String PASSWORD_ARE_THE_SAME = "account.password-already-used";

    public static void validateChangePassword(final String oldPassword, final String newPassword, final String savedPassword) {
        if (StringUtils.isEmpty(oldPassword) || StringUtils.isEmpty(newPassword)) {
            throw new ResponseException(MISSING_INFORMATION, ResponseStatus.BAD_REQUEST);
        }

        if (newPassword.length() < 5) {
            throw new ResponseException(PASSWORD_TO_WEAK, ResponseStatus.BAD_REQUEST);
        }

        if (!oldPassword.equals(savedPassword)) {
            throw new ResponseException(WRONG_OLD_PASSWORD, ResponseStatus.BAD_REQUEST);
        }

        if (newPassword.equals(savedPassword)) {
            throw new ResponseException(PASSWORD_ARE_THE_SAME, ResponseStatus.BAD_REQUEST);
        }

    }
}


