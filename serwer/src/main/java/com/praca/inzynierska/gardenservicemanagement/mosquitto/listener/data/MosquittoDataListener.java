package com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.data;

import com.google.gson.Gson;
import com.google.gson.JsonSyntaxException;
import com.praca.inzynierska.gardenservicemanagement.common.BinaryParser;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.data.model.MosquittoDataRequest;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.data.services.MosquittoDataProcessor;
import lombok.extern.slf4j.Slf4j;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
@Slf4j
public class MosquittoDataListener {

    @Value("${mosquitto.topic.data}")
    String dataTopic;

    private final MqttClient mqttClient;
    private final MosquittoDataProcessor registerProcessor;

    @Autowired
    public MosquittoDataListener(MqttClient mqttClient, MosquittoDataProcessor registerProcessor) {
        this.mqttClient = mqttClient;
        this.registerProcessor = registerProcessor;
    }

    @Bean
    public void dataListener() {
        IMqttMessageListener test = (topicName, mqttMessage) -> {
            String stringRequest = new String(mqttMessage.getPayload());
            log.info("Received register request: {}", stringRequest);

            Gson gsonParser = new Gson();
            MosquittoDataRequest request;
            try {
                 request = gsonParser.fromJson(stringRequest, MosquittoDataRequest.class);
            } catch (JsonSyntaxException e) {
                log.error("dataListener - Parser error !");
                e.printStackTrace();
                return;
            }

            System.out.println("MosquittoDataRequest parsed successfully");

            registerProcessor.saveMeasurementData(request);
            log.info("Data request from {} was handled.", BinaryParser.getMacAddressFromInt64(request.getMac()));
        };

        try {
            mqttClient.subscribe(dataTopic, 0, test);
        } catch (Exception e) {
            log.error("Error during creating dataTopic");
            e.printStackTrace();
            throw new RuntimeException();
        }
    }



}
