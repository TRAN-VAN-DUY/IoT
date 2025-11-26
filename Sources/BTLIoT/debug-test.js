// Test MQTT Connection
// Paste this into browser console (F12) to test

console.log("=== TESTING GATE CONTROLLER ===");

// Test 1: Backend connection
fetch("http://localhost:8080/door/status")
  .then(response => response.json())
  .then(data => {
    console.log("Backend Status:", data);
    console.log("MQTT Connected:", data.mqttConnected);
    console.log("Current Data:", data.currentData);
  })
  .catch(error => {
    console.error("Backend Connection Failed:", error);
    console.log("Solutions:");
    console.log("1. Check if Java backend is running");
    console.log("2. Check if port 8080 is available");
    console.log("3. Try restarting IntelliJ application");
  });

// Test 2: Manual commands
console.log("\n=== MANUAL TESTS ===");
console.log("Run these commands one by one:");
console.log("fetch('http://localhost:8080/door/open').then(r=>r.json()).then(console.log)");
console.log("fetch('http://localhost:8080/door/close').then(r=>r.json()).then(console.log)");
console.log("fetch('http://localhost:8080/door/data').then(r=>r.json()).then(console.log)");
