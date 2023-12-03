package com.praca.inzynierska.gardenservicemanagement.mosquitto;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class MosquitoConfiguration {

    @Value("${mosquitto.username}")
    String userName;

    @Value("${mosquitto.password}")
    String password;

    @Value("${mosquitto.broker.host}")
    String host;

    @Value("${mosquitto.broker.port}")
    String port;

    @Bean
    public MqttClient MosquitoClient() {
        String broker = String.format("tcp://%s:%s", host, port);
        String clientId = "garden_client";

        try {
            MqttClient client = new MqttClient(broker, clientId, new MemoryPersistence());

            MqttConnectOptions options = new MqttConnectOptions();
//            options.setUserName(username);
//            options.setPassword(password.toCharArray());
            options.setConnectionTimeout(60);
            options.setKeepAliveInterval(60);

            client.connect(options);
            return client;
        } catch (Exception e) {
            e.printStackTrace();
        }
        throw new RuntimeException();
    }
}
