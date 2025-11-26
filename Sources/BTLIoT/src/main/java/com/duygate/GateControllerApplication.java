package com.duygate;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class GateControllerApplication {

    public static void main(String[] args) {
        System.out.println("Starting Gate Controller Application...");
        SpringApplication.run(GateControllerApplication.class, args);
        System.out.println("Gate Controller Application started successfully!");
    }
}
