package com.duygate.model;

import com.fasterxml.jackson.annotation.JsonProperty;

public class GateData {
    @JsonProperty("status")
    private String status;
    
    @JsonProperty("distance")
    private int distance;
    
    @JsonProperty("timestamp")
    private long timestamp;
    
    @JsonProperty("unit")
    private String unit;

    // Constructors
    public GateData() {}

    public GateData(String status, int distance, long timestamp) {
        this.status = status;
        this.distance = distance;
        this.timestamp = timestamp;
        this.unit = "cm";
    }

    // Getters and Setters
    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public int getDistance() {
        return distance;
    }

    public void setDistance(int distance) {
        this.distance = distance;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public String getUnit() {
        return unit;
    }

    public void setUnit(String unit) {
        this.unit = unit;
    }

    @Override
    public String toString() {
        return "GateData{" +
                "status='" + status + '\'' +
                ", distance=" + distance +
                ", timestamp=" + timestamp +
                ", unit='" + unit + '\'' +
                '}';
    }
}
