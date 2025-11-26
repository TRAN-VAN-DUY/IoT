package com.duygate.controller;

import com.duygate.model.GateData;
import com.duygate.service.MqttService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.Map;

@RestController
@RequestMapping("/door")
@CrossOrigin(origins = "*") // Allow CORS for frontend
public class GateController {

    @Autowired
    private MqttService mqttService;

    @GetMapping("/data")
    public ResponseEntity<GateData> getDoorData() {
        try {
            GateData data = mqttService.getCurrentData();
            return ResponseEntity.ok(data);
        } catch (Exception e) {
            System.err.println("Error getting door data: " + e.getMessage());
            // Return default data if error occurs
            GateData defaultData = new GateData("UNKNOWN", 0, System.currentTimeMillis());
            return ResponseEntity.ok(defaultData);
        }
    }

    @RequestMapping(value = "/open", method = {RequestMethod.GET, RequestMethod.POST})
    public ResponseEntity<Map<String, String>> openDoor() {
        try {
            mqttService.sendCommand("OPEN");
            
            Map<String, String> response = new HashMap<>();
            response.put("status", "success");
            response.put("message", "Open command sent");
            response.put("command", "OPEN");
            
            return ResponseEntity.ok(response);
        } catch (Exception e) {
            Map<String, String> response = new HashMap<>();
            response.put("status", "error");
            response.put("message", "Failed to send open command: " + e.getMessage());
            
            return ResponseEntity.internalServerError().body(response);
        }
    }

    @RequestMapping(value = "/close", method = {RequestMethod.GET, RequestMethod.POST})
    public ResponseEntity<Map<String, String>> closeDoor() {
        try {
            mqttService.sendCommand("CLOSE");
            
            Map<String, String> response = new HashMap<>();
            response.put("status", "success");
            response.put("message", "Close command sent");
            response.put("command", "CLOSE");
            
            return ResponseEntity.ok(response);
        } catch (Exception e) {
            Map<String, String> response = new HashMap<>();
            response.put("status", "error");
            response.put("message", "Failed to send close command: " + e.getMessage());
            
            return ResponseEntity.internalServerError().body(response);
        }
    }

    @GetMapping("/status")
    public ResponseEntity<Map<String, Object>> getStatus() {
        Map<String, Object> status = new HashMap<>();
        status.put("mqttConnected", mqttService.isConnected());
        status.put("currentData", mqttService.getCurrentData());
        status.put("timestamp", System.currentTimeMillis());
        
        return ResponseEntity.ok(status);
    }

    @RequestMapping(value = "/mode", method = {RequestMethod.GET, RequestMethod.POST})
    public ResponseEntity<Map<String, String>> changeMode(@RequestParam String mode) {
        try {
            System.out.println("Changing gate mode to: " + mode);
            
            // Gửi lệnh thay đổi chế độ qua MQTT
            mqttService.sendCommand("MODE_" + mode.toUpperCase());
            
            Map<String, String> response = new HashMap<>();
            response.put("status", "success");
            response.put("message", "Mode changed to " + mode);
            response.put("mode", mode.toUpperCase());
            
            return ResponseEntity.ok(response);
        } catch (Exception e) {
            Map<String, String> response = new HashMap<>();
            response.put("status", "error");
            response.put("message", "Failed to change mode: " + e.getMessage());
            
            return ResponseEntity.internalServerError().body(response);
        }
    }
}
