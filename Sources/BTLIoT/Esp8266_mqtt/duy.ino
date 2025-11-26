#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "DESKTOP-UU7757E 2780";
const char* password = "10012004";

// HiveMQ Cloud broker settings
const char* mqtt_server = "6e49800075574b02a0817f9863e74cc1.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;                 // SSL Port
const char* mqtt_user = "tranduy";         // MQTT Username
const char* mqtt_pass = "Duy1234@";        // MQTT Password
// Static client ID to avoid memory issues
const char* mqtt_client_id = "ESP8266_GateController_01";

// MQTT topics
const char* topic_status = "gate/status";
const char* topic_distance = "gate/distance";
const char* topic_control = "gate/control";
const char* topic_test = "gate/test"; // Topic for test data

// Hardware pins
const int servoPin = 2;   // GPIO2 (D4 trên NodeMCU)
const int trig = 14;      // GPIO14 (D5 trên NodeMCU)
const int echo = 12;      // GPIO12 (D6 trên NodeMCU)
// LCD I2C sử dụng: SDA = GPIO4 (D2), SCL = GPIO5 (D1)

// Objects
Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Variables
bool gateOpen = false;
unsigned long lastMsg = 0;
int lastDistance = 0;
bool autoMode = true; // true = tự động, false = thủ công
bool lastGateState = false; // Lưu trạng thái cửa trước đó
unsigned long lastStatusSent = 0; // Thời gian gửi status cuối
unsigned long autoCloseTime = 0; // Thời gian tự động đóng
bool autoCloseEnabled = false; // Flag để tự động đóng
unsigned long lastTestMsg = 0; // Thời gian gửi test message cuối

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo LCD
  lcd.init();         
  lcd.backlight();    

  // Servo và cảm biến
  myservo.attach(servoPin);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  myservo.write(45); // trạng thái đóng
  gateOpen = false;

  // Màn hình khởi động
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("He thong mo cong");
  lcd.setCursor(0, 1);
  lcd.print("Khoi dong...");
  delay(2000);
  
  // Kết nối WiFi
  setupWiFi();
  
  Serial.print("MQTT Client ID: ");
  Serial.println(mqtt_client_id);
  
  // Thiết lập MQTT với SSL
  espClient.setInsecure(); // Bỏ qua xác thực SSL certificate (cho demo)
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  lcd.clear();
}

void loop() {
  // Đảm bảo kết nối MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long duration;
  int distance;

  // Phát xung đo khoảng cách
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = int(duration / 2 / 29.412);

  // Hiển thị khoảng cách
  lcd.setCursor(0, 0);
  lcd.print("Khoang cach: ");
  lcd.setCursor(13, 0);
  lcd.print("    "); // xóa dư ký tự
  lcd.setCursor(13, 0);
  lcd.print(distance);

  // Gửi dữ liệu lên MQTT mỗi 2 giây
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    // Gửi khoảng cách
    if (distance != lastDistance) {
      publishDistance(distance);
      lastDistance = distance;
    }
  }

  // Gửi test data mỗi 3 giây để test connection
  if (now - lastTestMsg > 3000) {
    lastTestMsg = now;
    publishTestData();
  }

  // Kiểm tra tự động đóng cửa (non-blocking)
  if (autoCloseEnabled && gateOpen && millis() >= autoCloseTime) {
    if (autoMode) { // Chỉ auto close khi ở chế độ auto
      closeGate();
      autoCloseEnabled = false;
    }
  }

  // Kiểm tra điều kiện mở cổng tự động (chỉ khi ở chế độ AUTO)
  if (autoMode) {
    if (distance >= 5 && distance <= 10) {
      if (!gateOpen) {
        openGateAuto(); // Sử dụng hàm mở tự động
      }
    } else {
      if (!gateOpen) { // Chỉ hiển thị "Đang chờ" khi cửa đóng
        lcd.setCursor(0, 1);
        lcd.print("Auto - Cho...  ");
      }
    }
  } else {
    // Chế độ manual - chỉ hiển thị trạng thái
    if (!gateOpen) {
      lcd.setCursor(0, 1);
      lcd.print("Manual - Ready ");
    }
  }

  // Reduced delay and yield to prevent WDT reset
  delay(100);
  yield(); // Feed the watchdog
}

// Hàm kết nối WiFi
void setupWiFi() {
  delay(10);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ket noi WiFi...");
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  lcd.setCursor(0, 1);
  lcd.print("WiFi OK!");
  delay(2000);
}

// Callback khi nhận tin nhắn MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  // Xử lý lệnh điều khiển từ app
  if (String(topic) == topic_control) {
    if (message == "OPEN") {
      openGate();
    } else if (message == "CLOSE") {
      closeGate();
    } else if (message == "MODE_AUTO") {
      autoMode = true;
      Serial.println("Switched to AUTO mode");
      lcd.setCursor(0, 1);
      lcd.print("Auto mode      ");
      autoCloseEnabled = false; // Reset auto close when switching modes
    } else if (message == "MODE_MANUAL") {
      autoMode = false;
      autoCloseEnabled = false; // Disable auto close in manual mode
      Serial.println("Switched to MANUAL mode");
      lcd.setCursor(0, 1);
      lcd.print("Manual mode    ");
    }
  }
}

// Hàm kết nối lại MQTT
void reconnect() {
  static unsigned long lastReconnectAttempt = 0;
  unsigned long now = millis();
  
  // Chỉ thử reconnect mỗi 5 giây để tránh WDT reset
  if (now - lastReconnectAttempt > 5000) {
    lastReconnectAttempt = now;
    
    Serial.print("Attempting MQTT connection...");
    
    // Kết nối với username và password
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      
      // Subscribe vào topic điều khiển
      client.subscribe(topic_control);
      
      lcd.setCursor(0, 1);
      lcd.print("MQTT OK!       ");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      lcd.setCursor(0, 1);
      lcd.print("MQTT loi...    ");
    }
  }
}

// Hàm mở cổng
void openGate() {
  if (!gateOpen) {
    lcd.setCursor(0, 1);
    lcd.print("Manual Open    ");
    myservo.write(160); // mở cổng
    gateOpen = true;
    
    // Gửi trạng thái lên MQTT
    publishStatus("OPEN");
    
    Serial.println("Gate opened manually - staying open until close command");
    // Remove debounce delay to prevent WDT reset
  }
}

// Hàm mở cổng tự động (từ cảm biến) - Non-blocking version
void openGateAuto() {
  if (!gateOpen) {
    lcd.setCursor(0, 1);
    lcd.print("Auto Welcome!  ");
    myservo.write(160); // mở cổng
    gateOpen = true;
    
    // Gửi trạng thái lên MQTT
    publishStatus("OPEN");
    
    Serial.println("Gate opened by sensor - will auto close in 5s");
    
    // Lưu thời gian để tự động đóng sau (non-blocking)
    autoCloseTime = millis() + 5000; // 5 seconds from now
    autoCloseEnabled = true;
  }
}

// Hàm đóng cổng
void closeGate() {
  if (gateOpen) {
    myservo.write(45);  // đóng cổng
    gateOpen = false;
    
    lcd.setCursor(0, 1);
    lcd.print("Gate closed    ");
    
    // Gửi trạng thái lên MQTT
    publishStatus("CLOSED");
    
    Serial.println("Gate closed");
    // Remove debounce delay to prevent WDT reset
  }
}

// Hàm gửi trạng thái cổng (chỉ khi có thay đổi)
void publishStatus(String status) {
  if (client.connected()) {
    // Chỉ gửi khi trạng thái thực sự thay đổi
    bool currentState = (status == "OPEN");
    if (currentState != lastGateState || (millis() - lastStatusSent > 10000)) {
      DynamicJsonDocument doc(200);
      doc["status"] = status;
      doc["timestamp"] = millis();
      doc["mode"] = autoMode ? "AUTO" : "MANUAL";
      
      String jsonString;
      serializeJson(doc, jsonString);
      
      client.publish(topic_status, jsonString.c_str());
      
      Serial.print("Published status: ");
      Serial.println(jsonString);
      
      lastGateState = currentState;
      lastStatusSent = millis();
    }
  }
}

// Hàm gửi khoảng cách
void publishDistance(int distance) {
  if (client.connected()) {
    DynamicJsonDocument doc(200);
    doc["distance"] = distance;
    doc["unit"] = "cm";
    doc["timestamp"] = millis();
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    client.publish(topic_distance, jsonString.c_str());
    
    Serial.print("Published distance: ");
    Serial.println(jsonString);
  }
}

// Hàm gửi test data với giá trị ngẫu nhiên
void publishTestData() {
  if (client.connected()) {
    DynamicJsonDocument doc(200);
    
    // Tạo các giá trị ngẫu nhiên để test
    int randomValue = random(1, 1000);
    float randomFloat = random(0, 10000) / 100.0; // Random float 0.00-99.99
    bool randomBool = random(0, 2) == 1;
    
    doc["random_int"] = randomValue;
    doc["random_float"] = randomFloat;
    doc["random_bool"] = randomBool;
    doc["uptime"] = millis();
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["client_id"] = mqtt_client_id;
    doc["timestamp"] = millis();
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    client.publish(topic_test, jsonString.c_str());
    
    Serial.print("Published test data: ");
    Serial.println(jsonString);
  } else {
    Serial.println("MQTT not connected - cannot send test data");
  }
}
