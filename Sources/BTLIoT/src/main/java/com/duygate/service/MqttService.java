package com.duygate.service;

import com.duygate.model.GateData;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;
import java.security.cert.X509Certificate;
import java.util.concurrent.atomic.AtomicReference;

@Service
public class MqttService implements MqttCallback {

    @Value("${mqtt.server}")
    private String mqttServer;

    @Value("${mqtt.port}")
    private int mqttPort;

    @Value("${mqtt.username}")
    private String mqttUsername;

    @Value("${mqtt.password}")
    private String mqttPassword;

    @Value("${mqtt.client.id}")
    private String clientId;

    @Value("${mqtt.topic.status}")
    private String statusTopic;

    @Value("${mqtt.topic.distance}")
    private String distanceTopic;

    @Value("${mqtt.topic.control}")
    private String controlTopic;

    private MqttClient mqttClient;
    private ObjectMapper objectMapper = new ObjectMapper();
    
    // Atomic references for thread-safe data storage
    private AtomicReference<String> currentStatus = new AtomicReference<>("CLOSED");
    private AtomicReference<Integer> currentDistance = new AtomicReference<>(0);
    private AtomicReference<Long> lastUpdateTime = new AtomicReference<>(System.currentTimeMillis());

    @PostConstruct
    public void initialize() {
        try {
            // Create MQTT client
            String brokerUrl = "ssl://" + mqttServer + ":" + mqttPort;
            mqttClient = new MqttClient(brokerUrl, clientId);
            
            // Set callback
            mqttClient.setCallback(this);
            
            // Configure connection options
            MqttConnectOptions options = new MqttConnectOptions();
            options.setUserName(mqttUsername);
            options.setPassword(mqttPassword.toCharArray());
            options.setCleanSession(true);
            options.setConnectionTimeout(30);
            options.setKeepAliveInterval(60);
            
            // SSL Configuration - accept all certificates (for demo purposes)
            SSLContext sslContext = SSLContext.getInstance("TLS");
            sslContext.init(null, new TrustManager[]{new X509TrustManager() {
                @Override
                public void checkClientTrusted(X509Certificate[] chain, String authType) {}
                
                @Override
                public void checkServerTrusted(X509Certificate[] chain, String authType) {}
                
                @Override
                public X509Certificate[] getAcceptedIssuers() { return new X509Certificate[0]; }
            }}, null);
            
            options.setSocketFactory(sslContext.getSocketFactory());
            
            // Connect to MQTT broker
            System.out.println("Connecting to MQTT broker: " + brokerUrl);
            mqttClient.connect(options);
            
            // Subscribe to topics
            mqttClient.subscribe(statusTopic, 1);
            mqttClient.subscribe(distanceTopic, 1);
            
            System.out.println("Connected to MQTT broker successfully!");
            
        } catch (Exception e) {
            System.err.println("Failed to connect to MQTT broker: " + e.getMessage());
            e.printStackTrace();
        }
    }

    @PreDestroy
    public void cleanup() {
        try {
            if (mqttClient != null && mqttClient.isConnected()) {
                mqttClient.disconnect();
                mqttClient.close();
            }
        } catch (Exception e) {
            System.err.println("Error disconnecting MQTT client: " + e.getMessage());
        }
    }

    @Override
    public void connectionLost(Throwable cause) {
        System.err.println("MQTT connection lost: " + cause.getMessage());
        // Implement reconnection logic here if needed
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        String payload = new String(message.getPayload());
        System.out.println("MQTT Message received:");
        System.out.println("   Topic: " + topic);
        System.out.println("   Payload: " + payload);
        
        try {
            JsonNode jsonNode = objectMapper.readTree(payload);
            
            if (topic.equals(statusTopic)) {
                String status = jsonNode.get("status").asText();
                long timestamp = jsonNode.get("timestamp").asLong();
                
                currentStatus.set(status);
                lastUpdateTime.set(timestamp);
                
                System.out.println("Updated gate status: " + status);
                
            } else if (topic.equals(distanceTopic)) {
                int distance = jsonNode.get("distance").asInt();
                long timestamp = jsonNode.get("timestamp").asLong();
                
                currentDistance.set(distance);
                lastUpdateTime.set(timestamp);
                
                System.out.println("Updated distance: " + distance + "cm");
            }
            
        } catch (Exception e) {
            System.err.println("Error parsing MQTT message: " + e.getMessage());
            System.err.println("Raw payload: " + payload);
            e.printStackTrace();
        }
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        System.out.println("Message delivery complete");
    }

    public void sendCommand(String command) {
        try {
            System.out.println("=== SENDING MQTT COMMAND ===");
            System.out.println("Command: " + command);
            System.out.println("Topic: " + controlTopic);
            System.out.println("Client connected: " + (mqttClient != null && mqttClient.isConnected()));
            
            if (mqttClient != null && mqttClient.isConnected()) {
                MqttMessage message = new MqttMessage(command.getBytes());
                message.setQos(1);
                mqttClient.publish(controlTopic, message);
                System.out.println("Command sent successfully: " + command);
            } else {
                System.err.println("MQTT client not connected! Cannot send command: " + command);
                if (mqttClient == null) {
                    System.err.println("MQTT client is null");
                } else {
                    System.err.println("MQTT client connection status: " + mqttClient.isConnected());
                }
            }
        } catch (Exception e) {
            System.err.println("Error sending MQTT command: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public GateData getCurrentData() {
        return new GateData(
            currentStatus.get(),
            currentDistance.get(),
            lastUpdateTime.get()
        );
    }

    public boolean isConnected() {
        return mqttClient != null && mqttClient.isConnected();
    }
}
