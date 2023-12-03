package com.praca.inzynierska.gardenservicemanagement.datastore.schedules;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface SchedulesRepository extends JpaRepository<SchedulesEntity, Long> {
}
