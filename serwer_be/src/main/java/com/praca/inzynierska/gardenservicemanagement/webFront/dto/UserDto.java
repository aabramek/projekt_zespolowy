package com.praca.inzynierska.gardenservicemanagement.webFront.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;


@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class UserDto {

    private Long id;
    private String userName;
    private String password;

//    public static UserEntity toEntity(UserDto userDto) {
//        final UserEntity user = new UserEntity();
//        user.setId(userDto.getId());
//        user.setFirstName(userDto.getFirstName());
//        user.setLastname(userDto.getLastName());
//        user.setEmail(userDto.getEmail());
//        user.setPassword(userDto.getPassword());
//        user.setCategory(userDto.getCategory() != null ? userDto.getCategory().stream().map(CategoryDto::toEntity).collect(Collectors.toList()) : null);
//        return user;
//    }
//
//    public static UserDto fromEntity(UserEntity user) {
//        return UserDto.builder()
//                .id(user.getId())
//                .firstName(user.getFirstName())
//                .lastName(user.getLastname())
//                .userName(user.getUserName())
//                .email(user.getEmail())
//                .password(user.getPassword())
//                .category(user.getCategory() != null ? user.getCategory().stream().map(CategoryDto::fromEntity).collect(Collectors.toList()) : null)
//                .build();
//    }
}
