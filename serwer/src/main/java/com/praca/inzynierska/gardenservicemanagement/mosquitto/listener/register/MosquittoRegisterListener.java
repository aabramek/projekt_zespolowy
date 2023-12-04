package com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register;

import com.google.gson.Gson;
import com.google.gson.JsonSyntaxException;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.model.MosquittoRegisterRequest;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.listener.register.services.MosquittoRegisterProcessor;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.MosquittoPublisherProcessor;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.DeviceConfigurationRequest;
import com.praca.inzynierska.gardenservicemanagement.mosquitto.publisher.model.MosquitoConfigValves;
import com.praca.inzynierska.gardenservicemanagement.webFront.utils.DefaultValves;
import lombok.extern.slf4j.Slf4j;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttClient;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
@Slf4j
public class MosquittoRegisterListener {

    @Value("${mosquitto.topic.register}")
    String registerTopic;

    private final MqttClient mqttClient;
    private final MosquittoRegisterProcessor registerProcessor;
    private final MosquittoPublisherProcessor mosquittoPublisherProcessor;

    @Autowired
    public MosquittoRegisterListener(MqttClient mqttClient, MosquittoRegisterProcessor registerProcessor, MosquittoPublisherProcessor mosquittoPublisherProcessor) {
        this.mqttClient = mqttClient;
        this.registerProcessor = registerProcessor;
        this.mosquittoPublisherProcessor = mosquittoPublisherProcessor;
    }

    @Bean
    public void registerListener() {
        IMqttMessageListener register = (topicName, mqttMessage) -> {
            String stringRequest = new String(mqttMessage.getPayload());
            log.info("Received register request: {}", stringRequest);

            Gson gsonParser = new Gson();
            MosquittoRegisterRequest request;

            try {
                 request = gsonParser.fromJson(stringRequest, MosquittoRegisterRequest.class);
            } catch (JsonSyntaxException e) {
                log.error("registerListener - Parser error!");
                e.printStackTrace();
                return;
            }
            log.info("MosquittoRegisterRequest parsed successfully");

           //TODO - DO ROZWAZENIA SYTUACJA KIEDY URZADZENIE ISTNIEJE ALE KOLEJKI TAKIEJ NIE MA -> ZROBIC JAKIEGOS CHECKA CZY TAKA KOLEJKA ISTNIEJE
           if(registerProcessor.registerStation(request)) {
               var deviceConfiguration = prepareDeviceConfiguration();
               mosquittoPublisherProcessor.sendConfigurationMessage(request.getMac(), deviceConfiguration);
           }
           log.info("Register request from {} was handled.", request.getMac());

        };

        try {
            mqttClient.subscribe(registerTopic, 0, register);
        } catch (Exception e) {
            log.error("Error during creating registerTopic");
            e.printStackTrace();
            throw new RuntimeException();
        }
    }

    private DeviceConfigurationRequest prepareDeviceConfiguration() {
        var deviceConf = DeviceConfigurationRequest.builder();
        deviceConf.measurementPeriod(1)
                  .valves(initValuesForNewDevice())
                  .build();
        return deviceConf.build();
    }

    private MosquitoConfigValves[] initValuesForNewDevice() {
        var valuesTab = new MosquitoConfigValves[16];
        for(int i=0;i<16;i++) {
            valuesTab[i] = DefaultValves.defaultConfiguration(i);
        }
        return valuesTab;
    }

}
