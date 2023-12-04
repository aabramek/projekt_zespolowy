package com.praca.inzynierska.gardenservicemanagement.webFront.controller.apiModel.account;

import lombok.Data;

@Data
public class ChangePasswordRequest {
    private Long userId;
    private String oldPassword;
    private String newPassword;
}
