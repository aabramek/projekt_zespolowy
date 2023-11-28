package com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception;

import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.ToString;

import java.util.List;

@Getter
@EqualsAndHashCode
@ToString
public class ResponseException extends RuntimeException {

    protected ResponseStatus status;
    protected String code;
    protected List<String> errors;

    public ResponseException(String code, ResponseStatus status) {
        super(String.format("code: %s, status: %s", code, status.name()));
        this.code = code;
        this.status = status;
    }

    public ResponseException(String code, ResponseStatus status, List<String> errors) {
        super(String.format("code: %s, status: %s, errors: %s", code, status.name(), errors));
        this.code = code;
        this.status = status;
        this.errors = errors;
    }

    public ResponseException(String code) {
        super(String.format("code: %s, status: %s", code, ResponseStatus.INTERNAL_SERVER_ERROR.name()));
        this.code = code;
        this.status = ResponseStatus.INTERNAL_SERVER_ERROR;
    }

}
