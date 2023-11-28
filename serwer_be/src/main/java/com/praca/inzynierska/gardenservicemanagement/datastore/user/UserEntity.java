package com.praca.inzynierska.gardenservicemanagement.datastore.user;

import jakarta.persistence.Entity;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.Id;
import jakarta.persistence.Table;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.io.Serializable;

@Entity
@AllArgsConstructor
@NoArgsConstructor
@Data
@Table(name = "USER")
public class UserEntity implements Serializable {

    @Id
    @GeneratedValue
    private Long id;
    private String userName;
    private String password;

}
