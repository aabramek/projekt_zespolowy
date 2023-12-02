package com.praca.inzynierska.gardenservicemanagement.webFront.provider;


import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesEntity;

import java.util.List;

public interface ValvesProvider {
    List<ValvesEntity> getValvesInformation(Long id);
}
