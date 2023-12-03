package com.praca.inzynierska.gardenservicemanagement.webFront.provider;

import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesEntity;
import com.praca.inzynierska.gardenservicemanagement.datastore.valves.ValvesRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class ValvesInvokerProvider implements ValvesProvider {

    ValvesRepository valvesRepository;

    @Autowired
    public ValvesInvokerProvider(ValvesRepository valvesRepository) {
        this.valvesRepository = valvesRepository;
    }

    @Override
    public List<ValvesEntity> getValvesInformation(Long id) {
        return valvesRepository.findAllByStationId(id);
    }
    
}
