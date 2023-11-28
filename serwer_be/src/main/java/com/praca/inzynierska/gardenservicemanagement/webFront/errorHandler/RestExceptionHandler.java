package com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.DefaultError;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseException;
import com.praca.inzynierska.gardenservicemanagement.webFront.errorHandler.exception.ResponseStatus;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.http.ResponseEntity;
import org.springframework.util.CollectionUtils;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import org.springframework.web.servlet.mvc.method.annotation.ResponseEntityExceptionHandler;

@RestControllerAdvice
@Slf4j
public class RestExceptionHandler extends ResponseEntityExceptionHandler {

    @ExceptionHandler(ResponseException.class)
    public ResponseEntity<DefaultError> handleBadRequestException(ResponseException ex) throws JsonProcessingException {
        ObjectMapper obj = new ObjectMapper();
        var defaultHttpErrorObject = toHttpDefaultErrorObject(ex);

        return ResponseEntity.status(toHttpStatus(ex.getStatus())).body(defaultHttpErrorObject);
    }

    private int toHttpStatus(ResponseStatus responseStatus) {
        return responseStatus.getCode();
    }

    private DefaultError toHttpDefaultErrorObject(ResponseException ex) {
        var object = DefaultError.builder();
        if(StringUtils.isNotEmpty(ex.getCode())) {
            object.code(ex.getCode());
        }
        if(!CollectionUtils.isEmpty(ex.getErrors())) {
            object.errors(ex.getErrors());
        }
        return object.build();
    }

}
