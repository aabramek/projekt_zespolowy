package com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher;

import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.DeviceConfigurationRequest;
public interface MosquittoPublisherProcessor {
     void sendConfigurationMessage(String topic, DeviceConfigurationRequest deviceConfigurationRequest);

     void sendMessageToTopic(String topic, String message);
}
