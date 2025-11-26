# Gate Controller Backend

Backend Java Spring Boot để điều khiển cửa tự động qua MQTT HiveMQ Cloud.

## Cấu hình MQTT
- **Server**: `6e49800075574b02a0817f9863e74cc1.s1.eu.hivemq.cloud`
- **Port**: `8883` (SSL)
- **Username**: `dinhkien`
- **Password**: `Kien10@01`

## MQTT Topics
- `gate/status` - Nhận trạng thái cửa từ ESP8266
- `gate/distance` - Nhận dữ liệu khoảng cách từ cảm biến
- `gate/control` - Gửi lệnh điều khiển đến ESP8266

## API Endpoints

### GET `/door/data`
Lấy dữ liệu hiện tại của cửa
```json
{
  "status": "OPEN",
  "distance": 25,
  "timestamp": 1674123456789,
  "unit": "cm"
}
```

### POST `/door/open`
Gửi lệnh mở cửa
```json
{
  "status": "success",
  "message": "Open command sent",
  "command": "OPEN"
}
```

### POST `/door/close`
Gửi lệnh đóng cửa
```json
{
  "status": "success", 
  "message": "Close command sent",
  "command": "CLOSE"
}
```

### GET `/door/status`
Kiểm tra trạng thái kết nối
```json
{
  "mqttConnected": true,
  "currentData": {...},
  "timestamp": 1674123456789
}
```

## Cách chạy

### Yêu cầu
- Java 17+
- Maven 3.6+

### Chạy ứng dụng
1. **Windows**: Double click `run.bat`
2. **Linux/Mac**: `chmod +x run.sh && ./run.sh`
3. **Manual**: 
   ```bash
   mvn clean install
   mvn spring-boot:run -Dspring-boot.run.jvmArguments="--enable-native-access=ALL-UNNAMED"
   ```

Ứng dụng sẽ chạy tại: `http://localhost:8080`

## Cách chạy trên IntelliJ IDEA

### Bước 1: Mở project
1. **File** → **Open** → Chọn thư mục `DuyWeb`
2. IntelliJ sẽ tự động detect Maven project
3. Chờ IntelliJ import dependencies (có thể mất vài phút)

### Bước 2: Cấu hình JDK
1. **File** → **Project Structure** (Ctrl+Alt+Shift+S)
2. **Project** → **Project SDK** → Chọn **JDK 17**
3. **Project language level** → **17**
4. Click **OK**

### Bước 3: Chạy ứng dụng
**Cách 1: Từ Main Class**
1. Mở file `src/main/java/com/duygate/GateControllerApplication.java`
2. Click chuột phải vào class → **Run 'GateControllerApplication'**
3. Hoặc click nút play màu xanh bên cạnh `main` method

**Cách 2: Từ Maven**
1. Mở **Maven** tab (bên phải IDE)
2. Expand **gate-controller** → **Plugins** → **spring-boot**
3. Double-click **spring-boot:run**

### Bước 4: Kiểm tra
- Console sẽ hiển thị: `Started GateControllerApplication`
- Ứng dụng chạy tại: `http://localhost:8080`
- Test API: `http://localhost:8080/door/status`

### Run Configuration tự động
IntelliJ sẽ tạo run configuration với:
- **Main class**: `com.duygate.GateControllerApplication`
- **VM options**: `--enable-native-access=ALL-UNNAMED`
- **Use classpath of module**: `gate-controller`

### Troubleshooting IntelliJ
1. **Maven không load**: **File** → **Reload Maven Projects**
2. **Dependencies lỗi**: **Maven** tab → Click refresh button
3. **JDK không đúng**: **File** → **Project Structure** → **Project SDK**
4. **Port 8080 bị chiếm**: Thay đổi port trong `application.properties`

### Debug Mode
- Click **Debug** thay vì **Run** để debug
- Đặt breakpoint bằng cách click vào số dòng
- F8: Step over, F7: Step into, F9: Resume

## Cấu trúc project
```
src/
├── main/
│   ├── java/com/duygate/
│   │   ├── GateControllerApplication.java
│   │   ├── controller/GateController.java
│   │   ├── service/MqttService.java
│   │   └── model/GateData.java
│   └── resources/
│       └── application.properties
└── pom.xml
```
