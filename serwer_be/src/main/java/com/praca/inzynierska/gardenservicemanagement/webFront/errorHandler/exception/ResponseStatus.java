package com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception;

public enum ResponseStatus {
    BAD_REQUEST(400),
    UNAUTHORIZED(401),
    USER_NOT_FOUND(403),
    NOT_FOUND(404),
    INTERNAL_SERVER_ERROR(500);

    private final int code;

    ResponseStatus(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }
}
