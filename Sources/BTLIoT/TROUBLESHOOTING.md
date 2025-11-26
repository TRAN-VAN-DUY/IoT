# Troubleshooting Guide - Cửa vẫn bị lỗi

## Kiểm tra từng bước:

### 1. **Backend Java có chạy không?**
```cmd
# Kiểm tra port 8080
netstat -an | findstr :8080

# Nếu thấy LISTENING thì backend đang chạy
```

### 2. **MQTT có kết nối không?**
- Mở IntelliJ console
- Tìm dòng: "Connected to MQTT broker successfully!"
- Nếu thấy lỗi SSL hoặc authentication → Kiểm tra HiveMQ credentials

### 3. **ESP8266 có online không?**
- Check ESP8266 Serial monitor
- Tìm: "WiFi connected" và "MQTT OK!"

### 4. **Test từng component:**

#### A. Test Backend Only:
```javascript
// Paste vào browser console (F12):
fetch("http://localhost:8080/door/status").then(r=>r.json()).then(console.log)
```

#### B. Test MQTT Commands:
```javascript
// Test mở cửa:
fetch("http://localhost:8080/door/open").then(r=>r.json()).then(console.log)
// Test đóng cửa:
fetch("http://localhost:8080/door/close").then(r=>r.json()).then(console.log)
```

#### C. Test ESP8266:
- Gửi message manual qua HiveMQ webclient
- Topic: `gate/control`
- Payload: `OPEN` hoặc `CLOSE`

## Các lỗi thường gặp:

### **Lỗi 1: Backend không chạy**
**Triệu chứng:** Console error "Failed to fetch"
**Giải pháp:** 
1. Restart IntelliJ application
2. Check port 8080 available
3. Run: `mvn spring-boot:run`

### **Lỗi 2: MQTT không kết nối**
**Triệu chứng:** Backend chạy nhưng không có data từ ESP8266
**Giải pháp:**
1. Check HiveMQ credentials
2. Check internet connection
3. Try HiveMQ public broker: `broker.hivemq.com:1883`

### **Lỗi 3: ESP8266 offline**
**Triệu chứng:** Backend + MQTT OK, nhưng không có response
**Giải pháp:**
1. Check WiFi connection
2. Check GPIO pin connections
3. Upload lại code ESP8266

### **Lỗi 4: CORS/Network**
**Triệu chứng:** Frontend không gọi được API
**Giải pháp:**
1. Check @CrossOrigin annotation
2. Try different browser
3. Disable browser security (testing only)

## Quick Fix Commands:

### Restart everything:
```cmd
# 1. Stop Java backend (Ctrl+C)
# 2. Restart ESP8266
# 3. Start backend again
cd "c:\Users\Dinh_Huu_Kien\Desktop\DuyWeb"
run.bat
```

### Alternative MQTT test:
```java
// In application.properties, try public broker:
mqtt.server=broker.hivemq.com
mqtt.port=1883
# Comment out username/password
```

## Debug Steps:
1. Run `debug-test.js` in browser console
2. Check IntelliJ console logs
3. Check ESP8266 Serial monitor
4. Test each component separately
